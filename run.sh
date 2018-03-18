#!/bin/sh

KERNEL=ngk
ISO=ngos.iso
VM=qemu-system-x86_64

DEFAULTS="-cdrom $ISO -vga std -no-reboot -m 128M"
VIDEO="-display none -serial stdio"
EXTRA=""

while getopts "dvsim" opt; do
    case $opt in
        d)
            # Debug
            EXTRA="$EXTRA -S -s"
            ;;
        v)
            # Video on
            VIDEO=""
            #EXTRA="$EXTRA -monitor stdio"
            ;;
        s)
            # so you can add serial back with video
            EXTRA="$EXTRA -serial stdio"
            ;;
        i)
            # Show interrupts
            EXTRA="$EXTRA -d int"
            ;;
        m)
            # Only monitor (no output)
            EXTRA="$EXTRA -monitor stdio"
            ;;
        /?)
            echo "Unknown option: -$OPTARG" >&2
            ;;
    esac
done

echo $VM $DEFAULTS $VIDEO $EXTRA
$VM $DEFAULTS $VIDEO $EXTRA

