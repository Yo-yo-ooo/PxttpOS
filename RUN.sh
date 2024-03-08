#!/bin/bash
set -e

make

read -p "Press Enter to run..."

# ./cDisk.sh

# qemu-system-x86_64 -machine q35 -m 1G -cpu qemu64 -serial stdio -soundhw ac97 -drive file=disk.img -boot d -cdrom System.iso


qemu-system-x86_64 -machine q35 -m 2G -cpu qemu64 -smp 4 -serial stdio -boot d -drive file=disk.img -cdrom System.iso -soundhw ac97 -no-reboot --no-shutdown
# -d int 
