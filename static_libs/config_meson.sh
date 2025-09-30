#!/bin/bash

for dir in chihuahua_essentials elf paginator c_husky interrupt_manager
do
    pushd $dir
    source config_meson.sh
    popd
done
