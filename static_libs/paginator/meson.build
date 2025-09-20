project(
    'paginator', 
    'cpp',
    version : '0.1.0',
    default_options : ['warning_level=3', 'cpp_std=c++20'])

chihuahua_essentials_proj = subproject('chihuahua_essentials')
chihuahua_essentials_dep = chihuahua_essentials_proj.get_variable('chihuahua_essentials_dep')

include_dir = include_directories('include')
src = []
subdir('src')

static_library(
    'paginator',
    src,
    include_directories: include_dir,
    dependencies: [chihuahua_essentials_dep],
)