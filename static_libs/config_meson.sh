#!/bin/bash

for dir in chihuahua_essentials elf paginator
do
    pushd $dir
    source config_meson.sh
    popd
done
