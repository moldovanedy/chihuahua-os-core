project('bootloader', 'c', 
    version : '0.1.0',
        default_options : ['warning_level=3', 'c_std=c11', 'buildtype=release'])

include_dir = include_directories('include')
 
subdir('src')

bootloader_meson = executable(
    'BOOTX64.EFI', 
    src,
    include_directories: include_dir,
    c_args : ['-Wno-declaration-after-statement'],
    install: true,
    install_dir: meson.project_source_root() / '../bin/EFI/BOOT',
    dependencies: [
        declare_dependency(
            include_directories : include_directories('../static_libs/chihuahua_essentials'),
            version : meson.project_version()),
        declare_dependency(
            include_directories : include_directories('../efi_headers'),
            version: meson.project_version()),
    ])
