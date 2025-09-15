# About symlinks

Some of the subdirectories are/contain symlinks to sources from some of the libraries in `static_libs`, namely 
`elf` and `paginator`. This is because I couldn't get the linker to accept the libraries as subprojects or even with 
pkg-config (most likely because they are produced with different cross-compilers, the binary with *-mingw32-g++, 
the libs with *-elf-g++, though this should be irrelevant when using subprojects). But again, "it just works", that's
all that matters.