project(
    'kernel', 'cpp',
    version : '0.1.0',
    default_options : ['warning_level=3', 'cpp_std=c++20', 'buildtype=release'])

subdir('src')

kernel = executable(
    'kernel.elf',
    src,
    install: true,
    install_dir: meson.project_source_root() / '../bin/boot'
)
