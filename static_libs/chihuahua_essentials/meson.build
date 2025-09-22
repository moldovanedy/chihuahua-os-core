project(
    'chihuahua_essentials',
    'cpp',
    version : '0.1.0',
    default_options : ['warning_level=3', 'cpp_std=c++20'])

include_dir = include_directories('include')
src = []
subdir('src')

chihuahua_essentials = static_library(
    'chihuahua_essentials', 
    src,
#    c_args: ['-ffreestanding'],
    include_directories: include_dir,
    install: true,
    install_dir: meson.project_source_root() / 'lib'
)

subdir('include')
install_headers(include_headers, subdir: 'chihuahua_essentials')

chihuahua_essentials_dep = declare_dependency(
    include_directories: include_dir,
    link_with: chihuahua_essentials
)
