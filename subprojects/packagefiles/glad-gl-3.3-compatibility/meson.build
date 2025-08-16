project('glad-gl-3.3-compatibility', ['c'],
         version       : '3.3',  
         license       : ['PD'],
         meson_version : '>= 0.49.0')




name = 'glad-gl-3.3-compatibility'




system     = build_machine.system()
cpu        = build_machine.cpu()
cpu_family = build_machine.cpu_family()

cc    = meson.get_compiler('c')
cc_id = cc.get_id() 

lib_type    = get_option('default_library')

includes  = []
sources   = []
c_args    = []
link_args = []




include = include_directories('include')

sources = ['src/glad.c']




egl_registry_project = subproject('egl-registry')
egl_registry_dep = egl_registry_project.get_variable('egl_registry_dep')




if lib_type == 'static' or lib_type == 'both' 
    static_lib_name        = name
    static_lib_name_prefix = []
    static_lib_name_suffix = []

    if system == 'windows' and cc_id == 'msvc'  
        static_lib_name       += '_static'
        static_lib_name_prefix = ''
        static_lib_name_suffix = 'lib'
    endif

    glad_gl_static = static_library(static_lib_name,
                                    name_prefix         : static_lib_name_prefix, 
                                    name_suffix         : static_lib_name_suffix,
                                    include_directories : include,
                                    sources             : sources,
                                    c_args              : c_args, 
                                    dependencies        : [egl_registry_dep],
                                    link_args           : link_args,
                                    install             : true)

    glad_gl_static_dep = declare_dependency(include_directories : include,
                                            link_with           : glad_gl_static,
                                            dependencies        : [egl_registry_dep])
endif

if lib_type == 'shared' or lib_type == 'both'
    shared_lib_name = name 

    glad_gl_shared = shared_library(shared_lib_name, 
                                    include_directories : include,
                                    sources             : sources,
                                    c_args              : c_args + ['-DGLAD_GLAPI_EXPORT', '-DGLAD_GLAPI_EXPORT_BUILD'], 
                                    dependencies        : [egl_registry_dep],
                                    link_args           : link_args,   
                                    install             : true)

    glad_gl_shared_dep = declare_dependency(include_directories : include,
                                            compile_args        : ['-DGLAD_GLAPI_EXPORT'],
                                            link_with           : glad_gl_shared,
                                            dependencies        : [egl_registry_dep])
endif




if lib_type == 'static'
    glad_gl_dep = glad_gl_static_dep
elif lib_type == 'shared' or lib_type == 'both'
    glad_gl_dep = glad_gl_shared_dep
endif




install_headers('include/glad/glad.h',
                 subdir : 'glad/gl/3.3/compatibility')

install_data(['LICENSE.txt'],
               rename : ['LICENSE.PD'],
               install_dir : 'share/licenses/glad')