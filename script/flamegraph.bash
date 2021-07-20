#!/usr/bin/env bash

cat serial_perf | \
    stackcollapse.pl | \
    grep -v -e 'kernel_main ' -e '_kernel_phy_top' | \
    flamegraph.pl  > graph.svg
