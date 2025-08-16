project('klogger', 'cpp', default_options: ['cpp_std=c++20'])

klogger_lib = static_library(
	'klogger',
	'klogger.cpp',
	include_directories: include_directories('.'),
)

klogger_dep = declare_dependency(link_with: klogger_lib, include_directories: include_directories('.'))