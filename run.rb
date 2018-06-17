#!/usr/bin/env ruby

require 'optparse'

options = {serial: true, ram: "4M"}
OptionParser.new do |opts|
  opts.banner = "Usage: run.rb [options]"

  opts.on("-d", "--debug", "Wait for GDB debugging connection") do |v|
    options[:debug] = v
  end

  opts.on("-v", "--video", "Show QEMU VGA video window") do |v|
    options[:video] = v
  end

  opts.on("-s", "--[no-]serial", "Output the serial console to stdout (default: yes)") do |v|
    options[:serial] = v
  end

  opts.on("-i", "--interrupts", "Show interrupt information") do |v|
    options[:interrupts] = v
  end

  opts.on("-m", "--monitor", "Show the QEMU monitor (implies --no-serial)") do |v|
    options[:montior] = v
    options[:serial] = false
  end

  opts.on("--dry-run", "Print the QEMU command and do not run it") do |v|
    options[:dry_run] = v
  end

  opts.on("-r", "--ram AMOUNT", String, "RAM size for QEMU") do |v|
    options[:ram] = v
  end
end.parse!

VM = "qemu-system-x86_64"
ISO = "ngos.iso"

command = "#{VM} -cdrom #{ISO} -vga std -no-reboot -m #{options[:ram]} "
command += "-S -s " if options[:debug]
command += "-monitor stdio " if options[:monitor]
command += "-serial stdio " if options[:serial]
command += "-d int " if options[:interrupts]
command += "-display none " unless options[:video]

# I don't want ruby to print an exception trace on C-c
trap "SIGINT" do
  exit
end

puts command
system(command) unless options[:dry_run]

