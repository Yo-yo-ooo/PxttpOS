#!/bin/bash
set -e

if [ ! -f ./disk.img ]; then
    echo "<Creating disk.img>"
    fallocate -l 50M disk.img
fi

