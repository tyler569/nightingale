#!/bin/sh

ip tuntap add tap0 mode tap
ip link set dev tap0 up
ip addr add 10.50.1.1/24 dev tap0

