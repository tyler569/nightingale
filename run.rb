#!/usr/bin/env ruby

require 'optparse'

options = {
  iso_file: "build/ngos.iso",
  ram: "128M",
  stdio: "serial",
  smp: 2,
  network: nil,
  debug_wait: false,
  disk_image: "none",
  video: false,
  show_interrupts: false,
  command: nil,
  delay: 5
}

OptionParser.new do |opts|
  opts.banner = "Usage: run.rb [options]"

  opts.on("-f", "--file FILE", "ISO file to boot from (default: build/ngos.iso)") do |file|
    options[:iso_file] = file
  end

  opts.on("-r", "--ram RAM", "Amount of RAM to allocate (default: 128M)") do |ram|
    options[:ram] = ram
  end

  opts.on("-d", "--debug", "Wait for GDB to connect before starting") do
    options[:debug_wait] = true
  end

  opts.on("-v", "--video", "Enable video output") do
    options[:video] = true
  end

  opts.on("-i", "--interrupts", "Show interrupts") do
    options[:show_interrupts] = true
  end

  opts.on("-n", "--no-stdio", "Disable stdio") do
    options[:stdio] = "none"
  end

  opts.on("-m", "--monitor", "Enable monitor stdio") do
    options[:stdio] = "monitor"
  end

  opts.on("-s", "--smp SMP", "Number of CPUs to emulate (default: 2)") do |smp|
    options[:smp] = smp
  end

  opts.on("-e", "--disk-image FILE", "Disk image to attach (default: none)") do |file|
    options[:disk_image] = file
  end

  opts.on("-c", "--command CMD", "Command to run in the system via serial input") do |cmd|
    options[:command] = cmd
  end

  opts.on("--delay SECONDS", Integer, "Delay before running command (default: 5)") do |delay|
    options[:delay] = delay
  end

  opts.on("--dry-run", "Print the command without executing it") do
    options[:dry_run] = true
  end
end.parse!

extra_qemu_args = ARGV

qemu_args = [
  "qemu-system-x86_64",
  "-s",
  "-vga", "std",
  "-no-reboot",
  "-m", options[:ram],
  "-cdrom", options[:iso_file]
]

qemu_args << "-S" if options[:debug_wait]
qemu_args.concat(["-display", "none"]) unless options[:video]
qemu_args.concat(["-d", "int,cpu_reset"]) if options[:show_interrupts]

case options[:network]
when "rtl"
  qemu_args.concat(["-device", "rtl8139,netdev=net0"])
when "e1000"
  qemu_args.concat(["-device", "e1000,netdev=net0"])
when "igb"
  qemu_args.concat(["-device", "igb,netdev=net0"])
end

qemu_args.concat(["-netdev", "user,id=net0,hostfwd=udp::10000-:10000"])
qemu_args.concat(["-object", "filter-dump,id=dump0,netdev=net0,file=net.pcap"])

if options[:disk_image] != "none"
  qemu_args.concat(["-drive", "file=#{options[:disk_image]},format=raw"])
end

qemu_args.concat(["-smp", options[:smp].to_s]) if options[:smp] != 1

if options[:stdio] == "serial" && !options[:show_interrupts]
  qemu_args.concat(["-chardev", "stdio,id=char0,logfile=last_output"])
  qemu_args.concat(["-serial", "chardev:char0"])
end

qemu_args.concat(["-monitor", "stdio"]) if options[:stdio] == "monitor"

qemu_args.concat(extra_qemu_args) unless extra_qemu_args.empty?

qemu_command = qemu_args.join(" ")

if options[:command]
  escaped_cmd = options[:command].gsub("'", "'\\\\''")
  input_cmd = "(sleep #{options[:delay]}; echo '#{escaped_cmd}')"
  qemu_command = "#{input_cmd} | #{qemu_command}"
end

puts qemu_command
exec qemu_command unless options[:dry_run]
