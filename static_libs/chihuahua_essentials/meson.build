project('chihuahua_essentials', 
        'c',
        version : '0.1.0',
        default_options : ['warning_level=3', 'c_std=c11'])

include_dir = include_directories('include')
subdir('src')

chihuahua_essentials = static_library(
    'chihuahua_essentials', 
    src,
    c_args: ['-ffreestanding'],
    include_directories: include_dir,
    install: true,
    install_dir: meson.project_source_root() / 'bin'
)

install_headers('include/string.h', subdir: 'chihuahua_essentials')

pkg_mod = import('pkgconfig')
pkg_mod.generate(
    name: 'chihuahua_essentials',
    description: 'Utils',
    install_dir: 'bin',
    libraries: chihuahua_essentials,
    subdirs: 'chihuahua_essentials'
)