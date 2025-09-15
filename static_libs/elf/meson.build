project(
    'elf',
    'cpp',
    version : '0.1.0',
    default_options : ['warning_level=3', 'cpp_std=c++20'])

chihuahua_essentials_proj = subproject('chihuahua_essentials')
chihuahua_essentials_dep = chihuahua_essentials_proj.get_variable('chihuahua_essentials_dep')

include_dir = include_directories('include')
src = []
subdir('src')

elf = static_library(
    'elf', 
    src,
    include_directories: include_dir,
    cpp_args: ['-ffreestanding'],
    dependencies: [
        chihuahua_essentials_dep
    ],
    install: true,
    install_dir: meson.project_source_root() / 'lib'
)

elf_dep = declare_dependency(
    include_directories: include_dir,
    link_with: elf,
    dependencies: [chihuahua_essentials_dep]
)
