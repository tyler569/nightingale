def gcc_compiler(basename = "gcc", default_flags: [])
  Proc.new do |input:, output:, dep: nil, flags: [], **args|
    <<~EOF.gsub("\n", " ")
    #{basename} -c -o #{output} #{input}
    #{"-MD -MF #{dep}" if dep}
    #{(default_flags + flags).join " "}
    EOF
  end
end

def gcc_linker(basename = "gcc", default_flags: [])
  Proc.new do |input:, output:, flags: [], libs: [], **args|
    <<~EOF.gsub("\n", " ")
    #{basename}
    #{(default_flags + flags).join " "}
    -o #{output}
    #{input.join " "}
    #{libs.map { |l| "-l{l}" }.join " " }
    EOF
  end
end

def gas_assembler(basename = "gcc", default_flags: [])
  gcc_compiler(basename, default_flags: default_flags)
end

def nasm_assembler(basename = "nasm", default_flags: [])
  Proc.new do |input:, output:, dep: nil, flags: [], **args|
    # raise "nasm-type assemblers do not supprt dependancy tracking" if dep
    "#{basename} #{(default_flags + flags).join " "} -o #{output} #{input}"
  end
end

def ar_linker(basename = "ar", default_flags: [])
  Proc.new do |input:, output:, flags: [], libs: [], **args|
    raise "ar-type linkers cannot take flags" unless (default_flags + flags).empty?
    raise "ar-type linkers cannot link libraries" unless libs.empty?
    "#{basename} rcs #{output} #{input.join " "}"
  end
end

def ld_r_linker(basename = "ld", default_flags: [])
  Proc.new do |input:, output:, flags: [], libs: [], **args|
    raise "ld-r-type linkers cannot link libraries" unless libs.empty?
    "#{basename} -r #{(default_flags + flags).join " "} -o #{output} #{input.join " "}"
  end
end

