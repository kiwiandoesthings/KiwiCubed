project(
  'kiwicubed',
  'cpp',
  default_options: ['cpp_std=c++20', 'default_library=static'],
)

# Stops KiwiCubed\subprojects\glm-1.0.1\glm\gtx\../gtx/transform.hpp(23): fatal error C1189
add_project_arguments('-DGLM_ENABLE_EXPERIMENTAL', language: 'cpp')
add_project_arguments('-DSTB_IMAGE_IMPLEMENTATION', language: 'cpp')

comp_id = meson.get_compiler('cpp').get_id()

if comp_id == 'msvc'
  add_project_link_arguments('/ignore:4099', language: 'cpp') # 4099 is for PDB not found, which is fine and just logspam because we don't have PDBs anyways
endif

if build_machine.system() != 'windows' and meson.is_cross_build()
  add_project_link_arguments(['-static', '-static-libgcc', '-static-libstdc++'], language: 'cpp') # we're cross compiling and wine no likey
endif

dependencies = [
  dependency('kwport'),
  dependency('klogger', static: true),
  dependency('json'),
  dependency('glm', static: true),
  dependency('glad-gl', static: true),
  subproject('sdl2').get_variable('sdl2_dep'), # Force subproject-based SDL2
  dependency('imgui', static: true),
]

if get_option('tracy_enable')
  dependencies += [dependency('tracy', static: true)]
  add_project_arguments('-DPROFBUILD', language: 'cpp')
  if get_option('buildtype') != 'debugoptimized'
  warning('Profiling builds should set --buildtype=debugoptimized')
  endif
endif

executable(
  'kiwicubed',
  files(
    'Source/Input.cpp',
    'Source/Main.cpp',
    'Source/Player.cpp',

    # Rendering
    'Source/Rendering/Camera.cpp',
    'Source/Rendering/DebugRenderer.cpp',
    'Source/Rendering/GLError.cpp',
    'Source/Rendering/Renderer.cpp',
    'Source/Rendering/Shader.cpp',
    'Source/Rendering/Texture.cpp',
    'Source/SingleplayerHandler.cpp',
    'Source/Window.cpp',

    # World types
    'Source/World/Block.cpp',
    'Source/World/BlockManager.cpp',
    'Source/World/Chunk.cpp',
    'Source/World/ChunkHandler.cpp',
    'Source/World/Entity.cpp',
    'Source/World/Physics.cpp',
    'Source/World/World.cpp',
  ),
  include_directories: include_directories('Source', 'Source/World', 'Source/Rendering'),
  dependencies: dependencies,
)
