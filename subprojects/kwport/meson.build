project('kwport', ['c', 'cpp'], default_options: ['cpp_std=c++20'])

wasm_sources = [
    'wasm/m3_api_libc.c',
    'wasm/m3_api_tracer.c',
    'wasm/m3_bind.c',
    'wasm/m3_code.c',
    'wasm/m3_compile.c',
    'wasm/m3_core.c',
    'wasm/m3_emit.c',
    'wasm/m3_env.c',
    'wasm/m3_exec.c',
    'wasm/m3_function.c',
    'wasm/m3_info.c',
    'wasm/m3_module.c',
    'wasm/m3_optimize.c',
    'wasm/m3_parse.c'
]

wasm = static_library('wasm3', wasm_sources, include_directories: include_directories('wasm'), c_args: ['-DM3_NO_WASI'], link_language: 'c', native: true)

kwport_dep = declare_dependency(link_with: wasm, include_directories: include_directories('include', 'wasm'))