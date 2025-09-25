#!/bin/bash

# Mouts the given .img file, writes the entire OS binaries inside the .img file, unmounts it,
# then runs the OS using QEMU.

if [[ $# -ne 2 ]]; then
    echo 'Too many/few arguments, expecting only the path to the .img file to mount and the temporary mount path.' >&2
    exit 1
fi

LOOPBACK_OUTPUT=$(kpartx -av $1)
LOOPBACK_DEV=$(echo "$LOOPBACK_OUTPUT" | awk '{print $3}')
mount "/dev/mapper/${LOOPBACK_DEV}" $2

cp -r ../bin/* $2

umount -l "/dev/mapper/${LOOPBACK_DEV}"
kpartx -d $1

qemu-system-x86_64 \
  -enable-kvm \
  -m 4096 \
  -drive file=$1,format=raw \
  -bios /usr/share/edk2/ovmf/OVMF_CODE.fd \
  -boot order=d \
  -serial file:debug.log \
  -monitor stdio \
  -d int \
  -no-reboot \
  -no-shutdown
