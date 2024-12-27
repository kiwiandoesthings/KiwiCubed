project('log4kwc', 'cpp', default_options: ['cpp_std=c++20'])

log4kwc_lib = static_library(
	'log4kwc',
	'log4kwc.cpp',
	include_directories: include_directories('.'),
)

log4kwc_dep = declare_dependency(link_with: log4kwc_lib, include_directories: include_directories('.'))