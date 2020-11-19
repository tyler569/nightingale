#!/usr/bin/env ruby

require 'optparse'

options = {
  file: 'ngk.elf',
  mode: :objdump,
  source: true,
  format: :intel,
}
OptionParser.new do |opts|
  opts.on("-f", "--file FILE", "Program to dump") { |f| options[:file] = f }
  opts.on("-a", "--addr2line", "Run addr2line on last_output") { options[:mode] = :addr2line }
  opts.on("-s", "--no-source", "Don't intersperce source") { options[:source] = false }
  opts.on("-i", "--intel", "Dump in intel-format asm") { options[:format] = :intel }
  opts.on("-t", "--att", "Dump in att-format asm") { options[:format] = :att }
end.parse!

OBJDUMP = "objdump"

def backtrace
  output = `tail -n100 last_output`
  output.split("\n").map do |l|
    case l
    when /\((.*)\) <.*>/
      $1
    when /bp:.*ip: (.+)$/
      $1
    end
  end.compact
end

if options[:mode] == :addr2line
  addrs = backtrace
  return if addrs.length == 0
  cmd = "addr2line -fips -e #{options[:file]} #{addrs.join " "}"
  system cmd
end

if options[:mode] == :objdump
  objdump_command = [
    OBJDUMP,
    "-d"
  ]
  objdump_command << "-S" if options[:source]
  objdump_command << "-Mintel" if options[:format] == :intel
  objdump_command << "-j.text -j.low.text" unless options[:disasm_all] # TODO
  objdump_command << options[:file]
  objdump_command << "| less"

  cmd = objdump_command.join " "
  system cmd
end
