#!/usr/bin/env ruby

require 'optparse'

options = {
  serial: true,
  ram: "256M",
  iso: "ngos32.iso",
}

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
    options[:monitor] = v
    options[:serial] = false
  end

  opts.on("--dry-run", "Print the QEMU command and do not run it") do |v|
    options[:dry_run] = v
  end

  opts.on("-r", "--ram AMOUNT", String, "RAM size for QEMU") do |v|
    options[:ram] = v
  end

  opts.on("-i", "--disk FILENAME", String, "CD image to run in QEMU") do |v|
    options[:iso] = c
  end
end.parse!

VM = "qemu-system-x86_64"

command = "#{VM} -cdrom #{options[:iso]} -vga std -no-reboot -m #{options[:ram]} "
command += "-S -s " if options[:debug]
command += "-monitor stdio " if options[:monitor]
command += "-serial stdio " if options[:serial]
command += "-d int " if options[:interrupts]
command += "-display none " unless options[:video]

command += "-device rtl8139,netdev=net0 "
command += "-netdev user,id=net0,hostfwd=udp::1025-:1025 "
command += "-object filter-dump,id=dump0,netdev=net0,file=dump.pcap "

command += " | tee last_output"

# I don't want ruby to print an exception trace on C-c
trap "SIGINT" do
  exit
end

unless File.exist? options[:iso]
  puts "CD image '#{options[:iso]}' does not exist, do you need to 'make'?"
  exit
end

puts command
system(command) unless options[:dry_run]

