#!/usr/bin/env ruby

class Arg
  attr_reader :pointer, :type, :name
  def initialize(desc)
    type, _, name = desc.rpartition(' ')
    
    @pointer = type.count '*'
    @type = type.gsub '*', ''
    @name = name

    @pointer = 2 if type == "args"
  end

  def to_s
    if type == "args"
      "char *const *#{name}"
    else
      "#{@type} #{'*' * @pointer}#{@name}"
    end
  end

  def format
    if @pointer == 1 and @type =~ /(const )?char/
      "\\\"%s\\\""
    elsif @pointer > 0
      "%p"
    else
      case @type
      when "char", "int", "pid_t", "off_t", "dev_t",
        "mode_t", "time_t", "ssize_t", "long", /^enum/
        "%zi"
      when "size_t", "nfds_t", "socklen_t"
        "%zu"
      when "sighandler_t"
        "%p"
      else
        raise "unknown type: #{type}"
      end
    end
  end

  def is_pointer?
    @pointer > 0
  end

  def cast(type = "intptr_t")
    "(#{type})#{name}"
  end
end

class Syscall
  attr_reader :number, :name, :return, :args

  def initialize(desc)
    m = desc.match /(\d+) +(\w+) +(frame +)?{ +([^}]*) *} +(.+)/
    puts "failed to parse: #{desc}" unless m

    @number = m[1].to_i
    @name = m[2]
    @arg_string = m[4]
    @return = m[5]
    @needs_frame = !m[3].nil?

    if is_noreturn?
      # SYSCALLS format is 'noreturn' on the end.
      # C gets mad unless its at the beginning
      @return = @return
        .split
        .rotate(-1)
        .join(" ")
        .gsub("noreturn", "_Noreturn")
    end

    @args = @arg_string.split(",").map { |s| Arg.new(s.strip) }
  end

  def to_s
    "#{return} #{name}(#{args.join(", ")}) => #{number}"
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
    if name == "trace" || name == "ioctl"
      0
    else
      args
        .map(&:is_pointer?)
        .each_with_index
        .map { |v, ix| 1 << ix if v }
        .compact
        .reduce(0, &:+)
    end
  end

  def enum
    "    #{constant} = #{number},"
  end

  def map
    "    [#{constant}] = (uintptr_t)#{function},"
  end

  def mask
    "    [#{constant}] = #{pointer_mask},"
  end

  def debuginfo
    "    [#{constant}] = \"#{name}(#{args.map(&:format).join(", ")})\","
  end

  def kernel_header
    if needs_frame? && args.empty?
      "sysret #{function}(interrupt_frame *frame);"
    elsif needs_frame?
      "sysret #{function}(interrupt_frame *frame, #{args.join(", ")});"
    else
      "sysret #{function}(#{args.join(", ")});"
    end
  end

  def user_header
    <<~EOF
      #{@return} #{user_function}(#{args.join(", ")});
    EOF
  end

  def user_map
    "    [#{constant}] = \"#{name}\","
  end

  def args_cast
    args.map(&:cast)
  end

  def is_noreturn?
    @return.include? "oreturn"
  end

  def is_pointer_return?
    @return.include? "*"
  end

  def is_void_return?
    @return == "void"
  end

  def user_return(arg)
    if is_noreturn?
      "__builtin_unreachable();"
    else
      <<~EOF
        if (is_error(ret)) {
            errno = -ret;
            return (#{@return})-1;
        } else {
            return (#{@return})ret;
        }
      EOF
    end
  end

  def syscall_call
    if args.empty?
      "__syscall0(#{constant})"
    else
      "__syscall#{args.length}(#{constant}, #{args_cast.join(", ")})"
    end
  end

  def user_stub
    <<~EOF
      #{@return} #{user_function}(#{args.join(", ")}) {
          intptr_t ret = #{syscall_call};
      #{user_return("ret").gsub(/^/, "    ")}
      }
      #{@return} #{name}(#{args.join(", ")}) __attribute__ ((weak, alias ("#{user_function}")));
    EOF
  end
end

class Error
  attr_reader :name, :number

  def initialize(desc)
    m = desc.match /(\d+) +(\w+) +(.*)/
    @number = m[1].to_i
    @name = m[2]
    @info = m[3]
  end

  def constant
    name
  end

  def perror_info
    "(#{name}) #{@info}"
  end

  def enum
    "    #{constant} = #{number},"
  end

  def name_map
    "    [#{constant}] = \"#{name}\","
  end

  def info_map
    "    [#{constant}] = \"#{perror_info}\","
  end
end

(output_dir = ARGV[0]) || (raise "No output directory provided")

def generated(f)
  f.puts "// This file was AUTOGENERATED by generate_syscalls.rb"
  f.puts "// DO NOT EDIT IT"
  f.puts
end

def write(filename, &block)
  File.open(filename, "w") do |f|
    generated f
    yield f
  end
end

def header_guard(filename, &block)
  name = filename.gsub("/", "_").gsub(".", "_").gsub("-", "_")
  guard = "_AUTOGENERATED_#{name}_"
  write(filename) do |f|
    f.puts "#ifndef #{guard}"
    f.puts "#define #{guard}"
    f.puts
    yield f
    f.puts
    f.puts "#endif"
  end
end


syscall_file = File.read("interface/SYSCALLS")
syscalls = syscall_file
             .split("\n")
             .map(&:strip)
             .select { |l| not l.empty? and l[0] != '#' }
             .map { |l| Syscall.new l }
syscalls.sort! { |s| -s.number }

types = syscalls.map { |s| s.args.map { |a| a.type }}
          .flatten!
          .map! { |s| s.gsub("const ", "") }
          .uniq!
          .filter! { |s| s =~ /struct|enum|union/ }
          .sort!

errno_file = File.read("interface/ERRNOS")
errnos = errno_file
           .split("\n")
           .map(&:strip)
           .select { |l| not l.empty? and l[0] != '#' }
           .map { |l| Error.new l }
errnos.sort! { |e| -e.number }

header_guard("#{output_dir}/autogenerated_syscall_consts.h") do |f|
  f.puts "enum ng_syscall {"
  f.puts "    NG_INVALID,"
  syscalls.map(&:enum).each { |s| f.puts s }
  f.puts "    SYSCALL_MAX,"
  f.puts "};"
end

header_guard("#{output_dir}/autogenerated_syscalls_kernel.h") do |f|
  f.puts "BEGIN_DECLS"
  types.each { |t| f.puts "#{t};" }
  syscalls.map(&:kernel_header).each { |s| f.puts s }
  f.puts "END_DECLS"
end

write("#{output_dir}/autogenerated_syscalls_kernel.c") do |f|
  f.puts "uintptr_t syscall_table[SYSCALL_TABLE_SIZE] = {"
  syscalls.map(&:map).each { |s| f.puts s }
  f.puts "};"
  f.puts "const char *syscall_debuginfos[SYSCALL_TABLE_SIZE] = {"
  syscalls.map(&:debuginfo).each { |s| f.puts s }
  f.puts "};"
  f.puts "unsigned int syscall_ptr_mask[SYSCALL_TABLE_SIZE] = {"
  syscalls.map(&:mask).each { |s| f.puts s }
  f.puts "};"
  f.puts "const char *syscall_names[SYSCALL_TABLE_SIZE] = {"
  syscalls.map(&:user_map).each { |s| f.puts s }
  f.puts "};"
end

header_guard("#{output_dir}/autogenerated_syscalls_user.h") do |f|
  types.each { |t| f.puts "#{t};" }
  syscalls.map(&:user_header).each { |s| f.puts s }
end

write("#{output_dir}/autogenerated_syscall_names.c") do |f|
  f.puts "const char *syscall_names[] = {"
  syscalls.map(&:user_map).each { |s| f.puts s }
  f.puts "};"
end

write("#{output_dir}/autogenerated_syscalls_user.c") do |f|
  syscalls.map(&:user_stub).each { |s| f.puts s }
end

header_guard("#{output_dir}/autogenerated_errnos.h") do |f|
  f.puts "enum errno_value {"
  errnos.map(&:enum).each { |s| f.puts s }
  f.puts "    ERRNO_MAX,"
  f.puts "};"
end

write("#{output_dir}/autogenerated_errnos.c") do |f|
  f.puts "const char *errno_names[] = {"
  errnos.map(&:name_map).each { |s| f.puts s }
  f.puts "};"
  f.puts "char *perror_strings[] = {"
  errnos.map(&:info_map).each { |s| f.puts s }
  f.puts "};"
end
