project('bootloader', 'cpp',
        version : '0.1.0',
        default_options : ['warning_level=3', 'cpp_std=c++20', 'buildtype=release'])

include_dir = include_directories('include')
 
subdir('src')

bootloader_meson = executable(
    'BOOTX64.EFI', 
    src,
    # disable the string warnings, as we don't have another way of writing "safe C++" strings (we don't have std::string)
    cpp_args : ['-Wno-write-strings'],
    include_directories: include_dir,
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
