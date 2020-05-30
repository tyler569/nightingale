#!/usr/bin/env ruby

require 'optparse'

options = {
  serial: true,
  ram: "32M",
  serial2: true,
  iso: "ngos.iso",
  bits: 64,
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
  opts.on("-t", "--test", "Run in test mode (add the isa-debug-exit device)") do |v|
    options[:test] = true
  end
  opts.on("-T", "--notee", "Don't tee") do |v|
    options[:notee] = true
  end
  opts.on("--[no-]serial2", "Open a second serial console on a unix socket") do |v|
    options[:serial2] = v
  end
  opts.on("--extra ARGS", "Pass extra arguments to qemu") do |v|
    options[:extra] = v
  end
  opts.on("-3", "--32bit", "Force 32 bit mode") do |v|
    options[bits] = 32
  end
end.parse!

if options[:bits] == 64 && ENV['ARCH'] == 'i686'
  options[:bits] = 32
end

if options[:bits] == 64
  VM = "qemu-system-x86_64"
else
  VM = "qemu-system-i386"
end

command = "#{VM} -cdrom #{options[:iso]} -vga std -no-reboot -m #{options[:ram]} "
# command += "-d cpu_reset,guest_errors "
command += "-S -s " if options[:debug]
command += "-s " unless options[:debug] # test
command += "-monitor stdio " if options[:monitor]
command += "-serial stdio " if options[:serial]
command += "-d int " if options[:interrupts]
command += "-display none " unless options[:video]
command += "-device isa-debug-exit " if options[:test]
command += "-serial unix:./serial2,nowait,server " if options[:serial2]

if true
  command += "-device rtl8139,netdev=net0 "
  command += "-netdev tap,id=net0,script=no,downscript=no,ifname=tap0 "
  command += "-object filter-dump,id=dump0,netdev=net0,file=tap0.pcap "
end

# if options[:network]
#   command += "-device rtl8139,netdev=net1 "
#   command += "-netdev tap,id=net1,script=no,downscript=no,ifname=tap1 "
#   command += "-object filter-dump,id=dump1,netdev=net1,file=tap1.pcap "
# end

if options[:extra]
  command += options[:extra]
end

command += " | tee last_output" unless options[:notee]

# I don't want ruby to print an exception trace on C-c
trap "SIGINT" do
  exit
end

puts command
system(command) unless options[:dry_run]
# if options[:test]
#   puts $?
# end

