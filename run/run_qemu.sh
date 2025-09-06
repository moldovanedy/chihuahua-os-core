#!/bin/bash

# Mouts the given .img file, writes the entire OS binaries inside the .img file, unmounts it,
# then runs the OS using QEMU.

if [[ $# -ne 2 ]]; then
    echo 'Too many/few arguments, expecting only the path to the .img file to mount and the temporary mount path.' >&2
    exit 1
fi

LOOPBACK_DEV="$(losetup --find --show $1)"
mount "$LOOPBACK_DEV" $2

cp -r ../bin/* $2

umount -l "$LOOPBACK_DEV"
losetup --detach "$LOOPBACK_DEV"

qemu-system-x86_64 \
  -enable-kvm \
  -m 4096 \
  -drive file=$1,format=raw \
  -bios /usr/share/edk2/ovmf/OVMF_CODE.fd \
  -boot order=d \
  -serial file:debug.log \
  -d int \
  -no-reboot \
  -no-shutdown
