#!/usr/bin/env ruby

require 'optparse'

options = {
  file: "ngos.iso",
  ram: "32M",
  serial: true,
  tee: true,
}
OptionParser.new do |opts|
  opts.on("-f", "--file FILE", "ISO to boot") { |f| options[:file] = f }
  opts.on("-r", "--ram RAM", "Set emulated machine RAM") { |r| options[:ram] = r }
  opts.on("-d", "--debug", "Wait for GDB connection") { options[:debug] = true }
  opts.on("-v", "--video", "Enable QEMU graphical video output") { options[:video] = true }
  opts.on("-i", "--interrupts", "Enable QEMU interrupt debugging") { options[:interrupts] = true }
  opts.on("-n", "--no-serial", "Do not use serial stdio") { options[:serial] = false }
  opts.on("-m", "--monitor", "Show the QEMU monitor on stdio (implies --no-serial, --no-tee)") { options[:monitor] = true }
  opts.on("-x", "--net", "Attach a network interface") { options[:net] = true }
  opts.on("-t", "--no-tee", "Do not tee output to ./last_output") { options[:tee] = false }
  opts.on("--test-mode", "Run in test mode (attach isa-debug-exit device)") { options[:test] = true }
  opts.on("--dry-run", "Just print the QEMU command, don't run it") { options[:dry_run] = true }
end.parse!

options[:serial] = false if options[:monitor]
options[:tee] = false if options[:monitor]

QEMU = "qemu-system-x86_64"

qemu_command = [
  QEMU,
  "-s",
  "-vga std",
  "-no-reboot",
  "-m #{options[:ram]}",
  "-cdrom #{options[:file]}",
]

qemu_command << "-S" if options[:debug]
qemu_command << "-monitor stdio" if options[:monitor]
qemu_command << "-serial stdio" if options[:serial]
qemu_command << "-d int" if options[:interrupts]
qemu_command << "-display none" unless options[:video]
qemu_command << "--device isa-debug-exit" if options[:test]
#qemu_command << "-serial unix:./serial2,nowait,server"
qemu_command << "-serial file:./serial_perf"

if options[:net]
  qemu_command << "-device rtl8139,netdev=net0"
  qemu_command << "-netdev tap,id=net0,script=no,downscript=no,ifname=tap0"
  qemu_command << "-object filter-dump,id=dump0,netdev=net0,file=tap0.pcap"
end

qemu_command += ARGV
qemu_command << "| tee last_output" if options[:tee]

cmd = qemu_command.join(" ")
puts cmd

exit if options[:dry_run]

trap "SIGINT" do
  exit 0
end

system cmd
