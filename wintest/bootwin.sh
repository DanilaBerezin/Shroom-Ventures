#!/usr/bin/bash

# You can configure this stuff, add to it etc.
IMAGE_NAME=test.im
MEM_SIZE=8G
BOOT_ORDER=c
CPU=host
GRAPHICS=std
NIC_MODEL=e1000
NET_TYPE=user

# Makes this shell script executable anywhere, it just changes the current directory to the directory which contains the script
MY_PATH="$(dirname "${BASH_SOURCE[0]}")"
cd "$MY_PATH" 

qemu-system-x86_64  -hda $IMAGE_NAME \
                    -boot $BOOT_ORDER \
                    -enable-kvm \
                    -cpu $CPU \
                    -smp $(nproc) \
                    -m $MEM_SIZE \
                    -vga $GRAPHICS \
                    -nic $NET_TYPE,model=$NIC_MODEL \
                    -usb
