#!/bin/bash

for dir in kernel bootloader static_libs
do
    pushd $dir
    source config_meson.sh
    popd
done

