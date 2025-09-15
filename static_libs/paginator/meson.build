project(
    'paginator', 
    'cpp',
    version : '0.1.0',
    default_options : ['warning_level=3', 'cpp_std=c++20'])

include_dir = include_directories('include')
subdir('src')

static_library(
    'paginator',
    src,
    include_directories: include_dir,
    cpp_args: ['-ffreestanding']
)