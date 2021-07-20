
require 'pathname'

def does_header_exist(filename, header_relative)
  path = Pathname(filename).dirname
  header = path + header_relative
  header.exist?
end

def does_include_dir_header_exist(filename, header_relative)
  path = Pathname(filename).dirname
  include_dir_header = Pathname("include") + path + header_relative
  include_dir_header.exist?
end

def new_include_directive(filename, header_relative)
  path = Pathname(filename).dirname
  include_dir_header = path + header_relative
  clean = include_dir_header.cleanpath
  "#include <" + clean.to_s + ">\n"
end

def replace_local_includes(file, filename)
  contents = ""
  file.each do |line|
    if line.start_with? "#include \""
      header_match = line.match /#include "(.*)"/
      header_relative = header_match[1]

      local = does_header_exist(filename, header_relative)
      include_dir = does_include_dir_header_exist(filename, header_relative)

      # puts header_relative + " " + local.to_s + " " + include_dir.to_s

      if !local and !include_dir
        STDERR.puts filename + " may need cleanup"
      end

      if !local and include_dir
        contents += new_include_directive(filename, header_relative)
      else
        contents += line
      end
    else
      contents += line
    end
  end
  contents
end

Dir.glob("**/*.c").each do |filename|
  f = File.open(filename)
  new_content = replace_local_includes(f, filename)
  f.close
  f = File.open(filename, "w")
  f.puts(new_content)
  f.close
end

