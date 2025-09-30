project(
    'interrupt_manager', 
    'cpp',
    'nasm',
    version : '0.1.0',
    default_options : ['warning_level=3', 'cpp_std=c++20'])

include_dir = include_directories('include')
src = []
subdir('src')

interrupt_manager = static_library(
    'interrupt_manager', 
    src,
    include_directories: include_dir,
)

interrupt_manager_dep = declare_dependency(
    include_directories : include_dir,
    link_with : interrupt_manager
)