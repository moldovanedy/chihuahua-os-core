rm -rf ./buildDir
meson setup --cross-file ../host_config.ini --cross-file ../gcc_args.ini --cross-file ../x86_64-elf.ini buildDir
