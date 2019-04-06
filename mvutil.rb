#!/usr/bin/env ruby

require 'optparse'

options = {
}

OptionParser.new do |opts|
  opts.banner = "Usage: mvutil.rb [options]"

  opts.on("--mv", "Move a file and fix dependancies") do |v|
    options[:op] = "mv"
  end
  opts.on("--dry-run", "Just print what this would do") do |v|
    options[:dry_run] = v
  end
end.parse!

options[:from], options[:to] = ARGV

puts "would #{options[:op]} with #{options[:from]} and #{options[:to]}"


INCLUDE_PATHS = [
  "include",
  # "kernel/include",
]

def find_in_include_paths(filename, pwd=nil)
  INCLUDE_PATHS.each do |ip|
    if Dir.children(ip).include? filename
      return File.expand_path("./" + ip + "/" + filename)
    end
  end
  if pwd
    if Dir.children(pwd).include? filename
      return File.expand_path("./" + pwd + "/" + filename)
    end
  end
end

p find_in_include_paths("ng_syscall.h")
p find_in_include_paths("cpu.h", pwd="kernel/arch/x86")

def is_same_file(dir1, file_str1, dir2, file_str2)
  return File.expand_path(dir1 + '/' + file_str1) ==
         File.expand_path(dir2 + '/' + file_str2)
end

p is_same_file('kernel/arch', 'x86/cpu.h', 'kernel/fs', '../arch/x86/cpu.h')


# Basically, this is supposed to be a refactoring utility that resolves
# dependancies when moving/renaming files.

