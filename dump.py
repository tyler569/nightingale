#!/usr/bin/env python3

import optparse
import os
import re
import subprocess

parser = optparse.OptionParser()
parser.add_option('-a', '--addr2line', action='store_true', help='Run addr2line on last_output')
parser.add_option('-s', '--source', action='store_true', help="Intersperce source (default)", default=True, dest='source')
parser.add_option('-S', '--no-source', action='store_false', help="Don't intersperce source", dest='source')
parser.add_option('-f', '--file', help='Program to dump', default='NGK')
parser.add_option('-3', '--32bits', help='Dump in 32-bit mode', dest='bits32')
parser.add_option('-i', '--intel', action='store_true', help='Dump in intel-format asm')
parser.add_option('-t', '--att', action='store_false', help='Dump in att-format asm', dest='intel')

(options, args) = parser.parse_args()

bits = 64
if options.bits32:
    bits = 32
if os.environ.get('ARCH') == 'i686':
    bits = 32

file = options.file
if file == 'NGK':
    if bits == 32:
        file = 'build-i686/ngk.elf'
    else:
        file = 'build-x86_64/ngk.elf'

if bits == 32:
    objdump = 'i686-nightingale-objdump'
else:
    objdump = 'x86_64-nightingale-objdump'

if options.addr2line:
    output = subprocess.check_output('tail -n50 last_output', shell=True)
    output = output.decode("UTF-8")
    if "backtrace" not in output:
        print("No backtrace found")
        exit(0)
    addresses = []
    for line in output.split("\n"):
        m = re.search("\((.*)\) <.*>", line)
        if m:
            addresses.append(m.group(1))
        m = re.search(".+bp:.+ ip: (\W+)", line)
        if m:
            addresses.append(m.group(1))
    command = f'addr2line -fips -e {file} {" ".join(addresses)}'
    subprocess.run(command, shell=True)
    exit()

command = objdump
if options.source:
    command += ' -dS'
else:
    command += ' -d'
if options.intel:
    command += ' -Mintel'
if options.bits32:
    command += ',i386'
command += ' -j.text -j.low.text'
command += f' {file}'
command += ' | less'
subprocess.run(command, shell=True)

