#!/bin/bash

for dir in ./chihuahua_essentials ./elf
do
    pushd $dir
    source config_meson.sh
    popd
done
