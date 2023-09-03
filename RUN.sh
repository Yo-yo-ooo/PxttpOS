#!/bin/bash

make

read -p "Press Enter to run..."

# ./cDisk.sh

# qemu-System-x86_64 -machine q35 -m 1G -cpu qemu64 -serial stdio -soundhw ac97 -drive file=disk.img -boot d -cdrom System.iso


qemu-System-x86_64 -machine q35 -m 1G -cpu qemu64 -smp 4 -serial stdio -boot d -cdrom System.iso -no-reboot --no-shutdown
# -d int 
