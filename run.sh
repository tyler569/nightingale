#!/usr/bin/env bash

iso="ngos.iso"
mem="128M"
smp=2
debugopt="-debugcon stdio"
gdbserver=""
video="-display none"
tee="|& tee last_output"

while getopts "dmstv" opt; do
  case $opt in
    d)
      debugopt="-d int,cpu_reset"
      ;;
    m)
      debugopt="-monitor stdio"
      ;;
    s)
      gdbserver="-S"
      ;;
    t)
      tee=""
      ;;
    v)
      video=""
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done

exec qemu-system-x86_64 -s -vga std \
  -no-reboot \
  -m $mem \
  -smp $smp \
  -cdrom $iso \
  -M smm=off \
  $video \
  $debugopt \
  $gdbserver |& tee last_output
