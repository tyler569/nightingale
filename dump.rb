#!/usr/bin/env ruby

require 'optparse'

# In the future, I could make more of these configurable
options = {
  pager: "less",
  program: "kernel/ngk",
  objdump: "x86_64-elf-objdump",
  sections: %w(.text .low.text),
  source: true,
}

OptionParser.new do |opts|
  opts.banner = "Usage: dump.rb [options]"

  opts.on("-3", "--32-bit", "Dump in 32 bit mode") do |v|
    options[:'32bit'] = v
  end

  opts.on("-s", "--[no-]source", "Dump with interspersed source code (default yes)") do |v|
    options[:source] = v
  end

  opts.on("-p", "--[no-]pager [PAGER]", "Dump into a pager (default yes, less)") do |v|
    options[:pager] = v unless v.nil?
  end

  opts.on("-f", "--file [FILE]", "Program to be dumped (default kernel/ngk)") do |v|
    options[:program] = v
  end

  opts.on("--dry-run", "Dry run to see command") do |v|
    options[:dry_run] = v
  end
end.parse!

command = "#{options[:objdump]} "
command += options[:source] ? "-dS " : "-d "
command += "-Mintel"
command += ",i386" if options[:'32bit']
command += options[:sections].map { |section| " -j#{section}" }.join
command += " #{options[:program]}"
command += " | #{options[:pager]}" if options[:pager]

puts command
system(command) unless options[:dry_run]
