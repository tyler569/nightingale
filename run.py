#!/usr/bin/env python3

import optparse
import os
import subprocess

parser = optparse.OptionParser()
parser.add_option('-f', '--file', help='ISO to run (default ngos.iso)', default='ngos.iso')
parser.add_option('-r', '--ram', help="Set the VM's RAM size", default='32M')
parser.add_option('-d', '--debug', action='store_true', help='Wait for GDB debug connection')
parser.add_option('-v', '--video', action='store_true', help='Show video')
parser.add_option('-V', '--no-video', action='store_false', help="Don't show video (default)", dest='video')
parser.add_option('-i', '--interrupts', action='store_true', help='Show interrupt debug information')
parser.add_option('-s', '--serial', action='store_true', help='Use serial stdioa(default)', default=True)
parser.add_option('-n', '--no-serial', action='store_false', help='Do not use serial stdio', dest='serial')
parser.add_option('-m', '--monitor', action='store_true', help='Show the QEMU monitor on stdio (implies -T)')
parser.add_option('-M', '--no-monitor', action='store_false', help='Do not show the QEMU monitor on stdio (default)', dest='monitor')
parser.add_option('-x', '--net', action='store_true', help='Attach a network interface')
parser.add_option('-X', '--no-net', action='store_false', help='Do not attach a network interface (default)', dest='net')
parser.add_option('-t', '--tee', action='store_true', help='Tee output to ./last_output (default)', default=True, dest='tee')
parser.add_option('-T', '--no-tee', action='store_false', help='Do not tee output', dest='tee')
parser.add_option('-3', '--32bit', action='store_true', help='Run 32-bit QEMU (implied by ARCH=i686 environment)', dest='bits32')
parser.add_option('--test-mode', action='store_true', help='Run in test mode (attach isa-debug-exit device)')
parser.add_option('--dry-run', action='store_true', help="Just print the QEMU command, don't run it")

(options, args) = parser.parse_args()

# print(options)

bits = 64
if options.bits32:
    bits = 32
if os.environ.get('ARCH') == 'i686':
    bits = 32

if bits == 32:
    qemu = 'qemu-system-i386'
else:
    qemu = 'qemu-system-x86_64'

ram = options.ram
file = options.file

qemu_command = f'{qemu} -s -vga std -no-reboot -m {ram} -cdrom {file}'

if options.debug:
    qemu_command += ' -S'
if options.monitor:
    qemu_command += ' -monitor stdio'
if options.serial and not options.monitor:
    qemu_command += ' -serial stdio'
if options.interrupts:
    qemu_command += ' -d int'
if not options.video:
    qemu_command += ' -display none'
if options.test_mode:
    qemu_command += ' --device isa-debug-exit'

qemu_command += ' -serial unix:./serial2,nowait,server'

if options.net:
    qemu_command += ' -device rtl8139,netdev=net0'
    qemu_command += ' -netdev tap,id=net0,script=no,downscript=no,ifname=tap0'
    qemu_command += ' -object filter-dump,id=dump0,netdev=net0,file=tap0.pcap'

if args:
    qemu_command += ' '
    qemu_command += ' '.join(args)

if options.tee and not options.monitor:
    qemu_command += ' | tee last_output'

print(qemu_command)

if options.dry_run:
    exit()

try:
    subprocess.run(qemu_command, shell=True)
except KeyboardInterrupt:
    exit()

