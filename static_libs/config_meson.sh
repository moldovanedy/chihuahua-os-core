#!/bin/bash

for dir in ./chihuahua_essentials
do
    pushd $dir
    source config_meson.sh
    popd
done
