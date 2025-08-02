#!/usr/bin/env ruby

require 'yaml'
require 'erb'

# Represents a syscall argument
class Arg
  attr_reader :type, :name, :pointer

  def initialize(type:, name:)
    @pointer = type.count('*')
    @type = type.delete('*')
    @name = name
    @pointer = 2 if type == 'args'
  end

  def to_s
    if type == 'args'
      "char *const *#{name}"
    else
      "#{@type} #{'*' * @pointer}#{@name}"
    end
  end

  def format
    if @pointer == 1 && @type =~ /(const )?char/
      '%s'
    elsif @pointer.positive?
      '%p'
    else
      case @type
      when 'char', 'int', 'pid_t', 'off_t', 'dev_t',
           'mode_t', 'time_t', 'ssize_t', 'long', /^enum/
        '%zi'
      when 'size_t', 'nfds_t', 'socklen_t'
        '%zu'
      when 'sighandler_t'
        '%p'
      else
        raise "unknown type: #{type}"
      end
    end
  end

  def pointer?
    @pointer.positive?
  end

  def cast(type = 'intptr_t')
    "(#{type})#{name}"
  end
end

# Represents a syscall entry
class Syscall
  attr_reader :number, :name, :return, :args

  def initialize(hash)
    @number = hash['number']
    @name = hash['name']
    @return = normalize_return(hash['return'])
    @needs_frame = hash['frame'] || false
    @args = (hash['args'] || []).map { |a| Arg.new(type: a['type'], name: a['name']) }
  end

  def constant
    "NG_#{name.upcase}"
  end

  def function
    "sys_#{name}"
  end

  def user_function
    "__ng_#{name}"
  end

  def needs_frame?
    @needs_frame
  end

  def pointer_mask
    return 0 if %w[trace ioctl].include?(name)

    args
      .map(&:pointer?)
      .each_with_index
      .map { |v, ix| v ? 1 << ix : nil }
      .compact
      .reduce(0, :+)
  end

  def kernel_header
    if needs_frame?
      all_args = ["interrupt_frame *frame", *args].join(', ')
      "sysret #{function}(#{all_args});"
    else
      "sysret #{function}(#{args.join(', ')});"
    end
  end

  def user_header
    "#{@return} #{user_function}(#{args.join(', ')});"
  end

  def debuginfo
    "[#{constant}] = \"#{name}(#{args.map(&:format).join(', ')})\","
  end

  def map
    "[#{constant}] = #{function},"
  end

  def mask
    "[#{constant}] = #{pointer_mask},"
  end

  def user_map
    "[#{constant}] = \"#{name}\","
  end

  def args_cast
    args.map(&:cast)
  end

  def noreturn?
    @return.include?('noreturn')
  end

  def pointer_return?
    @return.include?('*')
  end

  def void_return?
    @return == 'void'
  end

  def user_return
    return '__builtin_unreachable();' if noreturn?

    <<~EOF
      if (is_error(ret)) {
          errno = -ret;
          return (#{@return})-1;
      } else {
          return (#{@return})ret;
      }
    EOF
  end

  def syscall_call
    if args.empty?
      "__syscall0(#{constant})"
    else
      "__syscall#{args.length}(#{constant}, #{args_cast.join(', ')})"
    end
  end

  def user_stub
    <<~EOF
      #{@return} #{user_function}(#{args.join(', ')}) {
          intptr_t ret = #{syscall_call};
      #{user_return.gsub(/^/, '    ')}
      }
      #{@return} #{name}(#{args.join(', ')}) __attribute__ ((weak, alias("#{user_function}")));
    EOF
  end

  private

  def normalize_return(ret)
    return ret unless ret&.include?('noreturn')

    parts = ret.split
    parts.rotate!(-1)
    parts.join(' ').gsub('noreturn', '[[noreturn]]')
  end
end

# Represents an errno entry
class Error
  attr_reader :name, :number, :info

  def initialize(hash)
    @number = hash['number']
    @name = hash['name']
    @info = hash['description']
  end

  def constant
    name
  end

  def perror_info
    "(#{name}) #{@info}"
  end

  def enum
    "#{constant} = #{number},"
  end

  def name_map
    "[#{constant}] = \"#{name}\","
  end

  def info_map
    "[#{constant}] = \"#{perror_info}\","
  end
end

(output_dir = ARGV[0]) || raise('No output directory provided')

syscall_file = ENV.fetch('SYSCALLS_YAML', 'interface/syscalls.yml')
errno_file = ENV.fetch('ERRNOS_YAML', 'interface/errnos.yml')

syscalls = YAML.load_file(syscall_file).map { |h| Syscall.new(h) }
syscalls.sort_by! { |s| -s.number }
errnos = YAML.load_file(errno_file).map { |h| Error.new(h) }
errnos.sort_by! { |e| -e.number }

types = syscalls
          .flat_map { |s| s.args.map { |a| a.type } }
          .map { |s| s.gsub('const ', '') }
          .uniq
          .select { |s| s =~ /(struct|enum|union)/ }
          .sort

TEMPLATES = {
  consts: <<~'EOS',
    enum ng_syscall {
        NG_INVALID,
    <% syscalls.each do |s| %>
        <%= s.constant %> = <%= s.number %>,
    <% end %>
        SYSCALL_MAX,
    };
  EOS
  kernel_header: <<~'EOS',
    <% types.each do |t| %>
    <%= t %>;
    <% end %>
    <% syscalls.each do |s| %>
    <%= s.kernel_header %>
    <% end %>
  EOS
  kernel_source: <<~'EOS',
    void *syscall_table[SYSCALL_TABLE_SIZE] = {
    <% syscalls.each do |s| %>
        <%= s.map %>
    <% end %>
    };
    const char *syscall_debuginfos[SYSCALL_TABLE_SIZE] = {
    <% syscalls.each do |s| %>
        <%= s.debuginfo %>
    <% end %>
    };
    unsigned int syscall_ptr_mask[SYSCALL_TABLE_SIZE] = {
    <% syscalls.each do |s| %>
        <%= s.mask %>
    <% end %>
    };
    const char *syscall_names[SYSCALL_TABLE_SIZE] = {
    <% syscalls.each do |s| %>
        <%= s.user_map %>
    <% end %>
    };
  EOS
  user_header: <<~'EOS',
    <% types.each do |t| %>
    <%= t %>;
    <% end %>
    <% syscalls.each do |s| %>
    <%= s.user_header %>
    <% end %>
  EOS
  names_source: <<~'EOS',
    const char *syscall_names[] = {
    <% syscalls.each do |s| %>
        <%= s.user_map %>
    <% end %>
    };
  EOS
  user_source: <<~'EOS',
    <% syscalls.each do |s| %>
    <%= s.user_stub %>
    <% end %>
  EOS
  errnos_header: <<~'EOS',
    enum errno_value {
    <% errnos.each do |e| %>
        <%= e.enum %>
    <% end %>
        ERRNO_MAX,
    };
  EOS
  errnos_source: <<~'EOS',
    const char *errno_names[] = {
    <% errnos.each do |e| %>
        <%= e.name_map %>
    <% end %>
    };
    char *perror_strings[] = {
    <% errnos.each do |e| %>
        <%= e.info_map %>
    <% end %>
    };
  EOS
}.freeze

# Helpers for writing files

def generated(f)
  f.puts '// This file was AUTOGENERATED by generate_syscalls.rb'
  f.puts '// DO NOT EDIT IT'
  f.puts
end

def write(filename, content)
  File.open(filename, 'w') do |f|
    generated f
    f.puts content
  end
end

def header_guard(filename, content)
  name = filename.tr('/.-', '_')
  guard = "_AUTOGENERATED_#{name}_"
  wrapped = <<~EOF
    #ifndef #{guard}
    #define #{guard}

    #{content.rstrip}

    #endif
  EOF
  write(filename, wrapped)
end

# Generate files
header_guard("#{output_dir}/autogenerated_syscall_consts.h", ERB.new(TEMPLATES[:consts], trim_mode: '-').result(binding))
header_guard("#{output_dir}/autogenerated_syscalls_kernel.h", ERB.new(TEMPLATES[:kernel_header], trim_mode: '-').result(binding))
write("#{output_dir}/autogenerated_syscalls_kernel.c", ERB.new(TEMPLATES[:kernel_source], trim_mode: '-').result(binding))
header_guard("#{output_dir}/autogenerated_syscalls_user.h", ERB.new(TEMPLATES[:user_header], trim_mode: '-').result(binding))
write("#{output_dir}/autogenerated_syscall_names.c", ERB.new(TEMPLATES[:names_source], trim_mode: '-').result(binding))
write("#{output_dir}/autogenerated_syscalls_user.c", ERB.new(TEMPLATES[:user_source], trim_mode: '-').result(binding))
header_guard("#{output_dir}/autogenerated_errnos.h", ERB.new(TEMPLATES[:errnos_header], trim_mode: '-').result(binding))
write("#{output_dir}/autogenerated_errnos.c", ERB.new(TEMPLATES[:errnos_source], trim_mode: '-').result(binding))
