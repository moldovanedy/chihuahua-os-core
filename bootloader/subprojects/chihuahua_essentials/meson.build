project('chihuahua_essentials', 
        'c',
        version : '0.1.0',
        default_options : ['warning_level=3', 'c_std=c11'])

include_dir = include_directories('include')
subdir('src')

chihuahua_essentials = static_library(
    'chihuahua_essentials', 
    src,
    include_directories: include_dir,
)

chihuahua_essentials_dep = declare_dependency(
    link_with: chihuahua_essentials,
    include_directories: include_dir
)