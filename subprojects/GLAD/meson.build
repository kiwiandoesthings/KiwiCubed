project('GLAD', 'c')

GLAD = library('GLAD', 'GLAD.c')
GLAD_inc = include_directories('.')

GLAD_dep = declare_dependency(include_directories: GLAD_inc, link_with: GLAD)
