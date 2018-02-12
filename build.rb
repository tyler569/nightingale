
# vim: ts=2 sts=2 sw=2

def compile_c(filename, output)

  if filename[-2:] != ".c" then
    puts "Is #{filename} really a C source file?"
  end

  if output == :obj then
    output = filename[-2:] + ".o"
  end
