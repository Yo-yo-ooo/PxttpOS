#!/bin/bash
set -e

./cDisk.sh
qemu-system-x86_64 -machine q35 -m 1G -cpu qemu64 -serial stdio -soundhw ac97 -drive file=disk.img -boot d -cdrom System.iso

