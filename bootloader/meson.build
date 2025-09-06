project('bootloader', 'c', 
    version : '0.1.0',
    default_options : ['warning_level=3', 'c_std=c11'])

include_dir = include_directories('include')

chihuahua_essentials_proj = subproject('chihuahua_essentials')
chihuahua_essentials_dep = chihuahua_essentials_proj.get_variable('chihuahua_essentials_dep')
 
subdir('src')

bootloader_meson = executable(
    'BOOTX64.EFI', 
    src,
    include_directories: include_dir,
    c_args: ['-Wno-declaration-after-statement', '-Wno-pre-c11-compat'],
    install: true,
    install_dir: meson.project_source_root() / '../bin/EFI/BOOT',
    dependencies: [
        chihuahua_essentials_dep,
        declare_dependency(
            include_directories: include_directories('c-efi'),
            version: meson.project_version()),
    ])
