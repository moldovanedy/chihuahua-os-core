project(
    'bootloader',
    'cpp',
    version : '0.1.0',
    default_options : ['warning_level=3', 'cpp_std=c++20'])

include_dir = include_directories(['include', 'src/elf/include', 'src/tiny_libc'])
subdir('src')

bootloader_meson = executable(
    'BOOTX64.EFI', 
    src,
    include_directories: include_dir,
    install: true,
    install_dir: meson.project_source_root() / '../bin/EFI/BOOT',
    dependencies: [
        declare_dependency(
            include_directories : include_directories('../efi_headers'),
            version : '0.1.0'),
    ])
