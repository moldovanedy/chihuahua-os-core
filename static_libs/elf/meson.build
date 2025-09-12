project(
    'elf', 'cpp',
    version : '0.1.0',
    default_options : ['warning_level=3', 'cpp_std=c++20'])

subdir('src')

static_library(
    'elf', 
    src,
    cpp_args: ['-ffreestanding']
)
