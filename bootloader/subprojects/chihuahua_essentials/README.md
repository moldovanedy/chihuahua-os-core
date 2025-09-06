# `chihuahua_essentials` duplication

The `chihuahua_essentials` project is compiled with GCC and linked with LD, resulting in ELF binaries. 
In the bootloader we use Clang and lld-link for PE binaries, so the projects are not compatible. Because we use 
pkg-config, it can only get the *.a file, so we can't even get the sources. The only option is to copy the files 
from `chihuahua_essentials` here whenever the source changes.