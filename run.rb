#!/usr/bin/env ruby

require 'optparse'

options = {
  serial: true,
  network: true,
  ram: "256M",
  iso: nil,
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
    options[:iso] = v

    if not File.exist? options[:iso]
      puts "CD image '#{options[:iso]}' does not exist, do you need to 'make'?"
      exit
    end
  end
  opts.on("--[no-]net", "Create a network interface (default: yes)") do |v|
    options[:network] = v
  end
end.parse!

if options[:iso] == nil
  if File.exist? "ngos64.iso"
    puts "using default file nightingale-64"
    options[:iso] = "ngos64.iso"
  elsif File.exist? "ngos32.iso" 
    puts "using default file nightingale-32"
    options[:iso] = "ngos32.iso"
  else
    puts "No default CD image found, do you need to 'make' or specify one with -i?"
    exit
  end
end

VM = "qemu-system-x86_64"

command = "#{VM} -cdrom #{options[:iso]} -vga std -no-reboot -m #{options[:ram]} "
command += "-S -s " if options[:debug]
command += "-monitor stdio " if options[:monitor]
command += "-serial stdio " if options[:serial]
command += "-d int " if options[:interrupts]
command += "-display none " unless options[:video]

if options[:network]
  command += "-device rtl8139,netdev=net0 "
  command += "-netdev user,id=net0,hostfwd=udp::1025-:1025 "
  command += "-object filter-dump,id=dump0,netdev=net0,file=dump.pcap "
end

command += " | tee last_output"

# I don't want ruby to print an exception trace on C-c
trap "SIGINT" do
  exit
end

puts command
system(command) unless options[:dry_run]

