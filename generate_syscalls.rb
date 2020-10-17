class Arg
  attr_reader :pointer, :type, :name
  def initialize(desc)
    type, _, name = desc.rpartition(' ')
    
    @pointer = type.count '*'
    @type = type.gsub '*', ''
    @name = name
  end

  def to_s
    "#{@type} #{'*' * @pointer}#{@name}"
  end

  def format
    if @pointer == 1 and @type =~ /(const )?char/
      "\\\"%s\\\""
    elsif @pointer > 0
      "%p"
    else
      case @type
      when "char", "int", "pid_t", "off_t", "ssize_t", "long", /^enum/
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
    m = desc.match /(\d+) (\w+) (frame )?{ ([^}]*) ?} (.+)/
    puts "failed to parse: #{desc}" unless m

    @number = m[1].to_i
    @name = m[2]
    @arg_string = m[4]
    @return = m[5]
    @needs_frame = !m[3].nil?

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

  def deprecated?
    false
  end

  def needs_frame?
    @needs_frame
  end


  def enum
    "    #{constant} = #{number},"
  end

  def map
    "    [#{constant}] = #{function},"
  end

  def mask
    "    [#{constant}] = #{args.map(&:is_pointer?).each_with_index.map { |v, ix| 1 << ix if v }.compact.reduce(0, &:+)},"
  end

  def debuginfo
    "    [#{constant}] = \"#{name}(#{args.map(&:format).join(", ")})\","
  end

  def kernel_header
    if needs_frame?
      "sysret #{function}(interupt_frame *frame, #{args.join(", ")});"
    else
      "sysret #{function}(#{args.join(", ")});"
    end
  end

  def user_header
    "#{@return} #{name}(#{args.join(", ")});"
  end

  def user_map
    "    [#{constant}] = \"#{name}\","
  end

  def args_cast
    args.map(&:cast)
  end

  def is_noreturn?
    @return.include? "noreturn"
  end

  def user_return(arg)
    if is_noreturn?
      "__builtin_unreachable()"
    else
      "RETURN_OR_SET_ERRNO(#{arg})"
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
      #{@return} #{name}(#{args.join(", ")}) {
          intptr_t ret = #{syscall_call};
          #{user_return("ret")}
      }
    EOF
  end
end


syscall_file = File.read("SYSCALLS")
syscalls = syscall_file
             .split("\n")
             .map(&:strip)
             .select { |l| not l.empty? and l[0] != '#' }
             .map { |l| Syscall.new l }


syscalls.sort! { |s| -s.number }
File.open("kernel_syscalls.h", "w") do |f|
  f.puts "enum ng_syscall {"
  syscalls.map(&:enum).each { |s| f.puts s }
  f.puts "};"
  syscalls.map(&:kernel_header).each { |s| f.puts s }
end

File.open("kernel_syscalls.c", "w") do |f|
  f.puts "sysret (*syscall_table[128])() {"
  syscalls.map(&:map).each { |s| f.puts s }
  f.puts "};"
  f.puts "const char *const syscall_debuginfos[128] = {"
  syscalls.map(&:debuginfo).each { |s| f.puts s }
  f.puts "};"
  f.puts "const unsigned int syscall_ptr_mask[128] = {"
  syscalls.map(&:mask).each { |s| f.puts s }
  f.puts "};"
end

File.open("user_syscalls.h", "w") do |f|
  f.puts "const char *syscall_names[] = {"
  syscalls.map(&:user_map).each { |s| f.puts s }
  f.puts "};"
  syscalls.map(&:user_header).each { |s| f.puts s }
end

File.open("user_syscalls.c", "w") do |f|
  syscalls.map(&:user_stub).each { |s| f.puts s }
end
