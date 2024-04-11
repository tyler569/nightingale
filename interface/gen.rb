#!/usr/bin/env ruby

require 'erb'

ERRNO_HEADER = <<~EOS
enum ng_errno {
<%- errnos.each do |e| -%>
	<%= e[:name] %> = <%= e[:number] %>,
<%- end -%>
};
extern const char *errno_names[];
extern const char *errno_descriptions[];
EOS

ERRNO_SOURCE = <<~EOS
const char *errno_names[] = {
<%- errnos.each do |e| -%>
	[<%= e[:name] %>] = "<%= e[:name] %>",
<%- end -%>
};

const char *errno_descriptions[] = {
<%- errnos.each do |e| -%>
	[<%= e[:name] %>] = "<%= e[:description] %>",
<%- end -%>
};
EOS

SYSCALL_HEADER = <<~EOS
<%- forward_declarations.each do |s| -%>
<%= s -%>;
<%- end -%>

enum ng_syscall {
<%- syscalls.each do |s| -%>
	<%= constant(s) %> = <%= s[:number] %>,
<%- end -%>
};

extern const char *syscall_names[];
extern const int syscall_ptr_masks[];
extern const char *syscall_debuginfos[];
EOS

SYSCALL_SOURCE = <<~EOS
const char *syscall_names[] = {
<%- syscalls.each do |s| -%>
	[<%= constant(s) %>] = "<%= s[:name] %>",
<%- end -%>
};

const int syscall_ptr_masks[] = {
<%- syscalls.each do |s| -%>
	[<%= constant(s) %>] = <%= pointer_mask(s) %>,
<%- end -%>
};

const char *syscall_debuginfos[] = {
<%- syscalls.each do |s| -%>
	[<%= constant(s) %>] = "<%= printf_format(s) %>",
<%- end -%>
};

EOS


KERNEL_HEADER = <<~EOS
#{SYSCALL_HEADER}

#{ERRNO_HEADER}

<%- syscalls.each do |s| -%>
<%= prototype(s, prefix: 'sys_', frame: true) %>;
<%- end -%>
EOS

KERNEL_SOURCE = <<~EOS
#{SYSCALL_SOURCE}

#{ERRNO_SOURCE}

long call_syscall(interrupt_frame *frame) {
	enum ng_syscall syscall_number = FRAME_SYSCALL(frame);
	long arg0 = FRAME_ARG0(frame);
	long arg1 = FRAME_ARG1(frame);
	long arg2 = FRAME_ARG2(frame);
	long arg3 = FRAME_ARG3(frame);
	long arg4 = FRAME_ARG4(frame);
	long arg5 = FRAME_ARG5(frame);
	long arg6 = FRAME_ARG6(frame);

	switch (syscall_number) {
	<%- syscalls.each do |s| -%>
		case <%= constant(s) %>:
			return sys_<%= s[:name] %>(<%= call_args(s, frame: true, generic_args: true, cast: true) %>);
	<%- end -%>
		default:
			return -ENOSYS;
	}
}
EOS

USER_HEADER = <<~EOS
#{SYSCALL_HEADER}

#{ERRNO_HEADER}

<%- syscalls.each do |s| -%>
<%= prototype(s, prefix: '__ng_') %>;
<%- end -%>
EOS

USER_SOURCE = <<~EOS
#{SYSCALL_SOURCE}

#{ERRNO_SOURCE}

<%- syscalls.each do |s| -%>
<%= prototype(s, prefix: '__ng_', frame: false) %> {
	intptr_t ret = syscall<%= s[:args].length %>(<%= constant(s) %><%= call_args(s, leading_comma: true) %>);
	<%- if is_pointer(s[:rettype]) -%>
	if (ret < 0 && ret > -4096) {
		errno = -ret;
		return nullptr;
	} else {
		errno = 0;
		return (void *)ret;
	}
	<%- elsif s[:rettype].include? 'noreturn' -%>
		UNREACHABLE();
	<%- elsif is_void(s[:rettype]) -%>
	if (ret < 0 && ret > -4096) {
		errno = -ret;
	} else {
		errno = 0;
	}
	<%- else -%>
	if (ret < 0 && ret > -4096) {
		errno = -ret;
		return -1;
	} else {
		errno = 0;
		return (<%= s[:rettype] %>)ret;
	}
	<%- end -%>
}

__attribute__((weak, alias("__ng_<%= s[:name] %>")))
<%= prototype(s, prefix: '', frame: false) %>;

<%- end -%>
EOS

# Each line contains a syscall number, syscall name, and syscall signature
# Example:
# 0 read ssize_t (unsigned int fd, char __user *buf, size_t count)

syscalls = File.open('interface/syscalls', 'r') do |f|
  f.readlines.map do |line|
    next if line.start_with?('#') || line.strip.empty?

    parts = line.split(' ')
    number = parts[0].to_i
    name = parts[1]
    signature = parts[2..-1].join(' ')

    rettype = signature.split('(')[0].strip
    args_string = signature.split('(')[1].split(')').first || ""
    args = args_string.split(',').map do |arg|
      f, m, l = arg.rpartition(/\*| /)
      { type: (f + m).strip, name: l }
    end

    if rettype.include?('[[ng::frame]]')
      rettype = rettype.gsub('[[ng::frame]]', '').strip
      frame = true
    else
      frame = false
    end

    { number: number, name: name, rettype: rettype, args: args, frame: frame }
  end
end.compact

errnos = File.open('interface/errnos', 'r') do |f|
  f.readlines.map do |line|
    next if line.start_with?('#') || line.strip.empty?

    parts = line.split(' ')
    number = parts[0].to_i
    name = parts[1]
    description = parts[2..-1].join(' ').strip

    { number: number, name: name, description: description }
  end
end

def prototype(syscall, prefix: '', frame: false)
  args = syscall[:args].map { |a| "#{a[:type]} #{a[:name]}" }
  if frame && syscall[:frame]
    args.unshift('interrupt_frame *frame')
  end

  "#{syscall[:rettype]} #{prefix}#{syscall[:name]}(#{args.join(', ')})"
end

def user_function_type(syscall)
  args = syscall[:args].map { |a| a[:type] }
  "#{syscall[:rettype]} (*)(#{args.join(', ')})"
end

def kernel_function_type(syscall)
  args = syscall[:args].map { |a| a[:type] }
  args.unshift('interrupt_frame *') if syscall[:frame]
  "long (*)(#{args.join(', ')})"
end

def remove_attributes(type)
  type.gsub(/\[\[.*?\]\]/, '').strip
end

def call_args(syscall, frame: false, generic_args: false, leading_comma: false, cast: false)
  args = if generic_args
           (0...syscall[:args].size).map { |i| "arg#{i}" }
         else
           syscall[:args].map { |a| a[:name] }
         end

  if cast
    args = args.zip(syscall[:args]).map do |arg, a|
      "(#{remove_attributes(a[:type])})#{arg}"
    end
  end

  if frame && syscall[:frame]
    args.unshift('frame')
  end
  args.unshift('') if leading_comma
  args.join(', ')
end

def call(syscall, prefix: '', frame: false)
  "#{prefix}#{syscall[:name]}(#{call_args(syscall, frame: frame)})"
end

def constant(syscall)
  "NG_#{syscall[:name]}"
end

POINTER_TYPES = %w[sighandler_t clone_fn]

def is_pointer(type)
  type.end_with?('*') || POINTER_TYPES.any? { |p| type.end_with?(p) }
end

def is_void(type)
  type.end_with?('void')
end

def pointer_mask(syscall)
  (0...syscall[:args].size).map do |i|
    if is_pointer(syscall[:args][i][:type])
      1 << i
    else
      0
    end
  end.reduce(:|) || 0
end

# printf format specifiers for syscall_trace
# note that all arguments are promoted to 64 bit integers when passed
# so we always use %l for all integral types
def printf_arg(type)
  if is_pointer(type)
    '%p'
  elsif type.start_with?('enum')
    '%ld'
  else
    case type
    when 'size_t', 'off_t'
      '%lu'
    when 'int', 'long', 'time_t', 'pid_t', 'dev_t'
      '%ld'
    when 'mode_t'
      '%lo'
    else
      raise "unknown type #{type} for syscall_trace format - update generate"
    end
  end
end

def printf_format(syscall)
  args = syscall[:args].map { |a| printf_arg(a[:type]) }
  args = args.join(', ')
  "#{syscall[:name]}(#{args})"
end

types = syscalls.flat_map { |s| [s[:rettype]] + s[:args].map { |a| a[:type] } }.uniq
structs = types.select { |t| t.start_with?('struct') || t.start_with?('union') || t.start_with?('enum') }
forward_declarations = structs.map { |s| s.chomp('*').strip }

File.open('build/ng_kernel.h', 'w') do |f|
  template = ERB.new(KERNEL_HEADER, trim_mode: '-')
  f.puts template.result(binding)
end
File.open('build/ng_kernel.c', 'w') do |f|
  template = ERB.new(KERNEL_SOURCE, trim_mode: '-')
  f.puts template.result(binding)
end
File.open('build/ng_user.h', 'w') do |f|
  template = ERB.new(USER_HEADER, trim_mode: '-')
  f.puts template.result(binding)
end
File.open('build/ng_user.c', 'w') do |f|
  template = ERB.new(USER_SOURCE, trim_mode: '-')
  f.puts template.result(binding)
end
