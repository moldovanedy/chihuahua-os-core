project(
    'c_husky',
    'cpp',
    version : '0.1.0',
    default_options : ['warning_level=3', 'cpp_std=c++20'])

include_dir = include_directories('include')
src = []
subdir('src')

c_husky = static_library(
    'c_husky',
    src,
    include_directories: include_dir,
)

c_huskyc_dep = declare_dependency(
    include_directories: include_dir,
    link_with: c_husky
)
