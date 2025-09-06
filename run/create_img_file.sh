#!/bin/bash

# This creates an empty .img file and creates a VFAT FS on it. This is very useful for running
# the OS inside a virtual machine like QEMU. Uses losetup, so it might need escaladed priviledges.

if [[ $# -ne 1 ]]; then
    echo 'Too many/few arguments, expecting only the path where to create the .img file.' >&2
    exit 1
fi

dd if=/dev/zero of="$1" bs=1M count=512
loopback_dev="$(losetup --find --show $1)"
mkfs.vfat "$loopback_dev"
losetup --detach "$loopback_dev"
