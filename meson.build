project('kiwicubed', 'cpp', default_options: ['cpp_std=c++20'])

# Stops KiwiCubed\subprojects\glm-1.0.1\glm\gtx\../gtx/transform.hpp(23): fatal error C1189
add_project_arguments('-DGLM_ENABLE_EXPERIMENTAL', language: 'cpp')
add_project_arguments('-DSTB_IMAGE_IMPLEMENTATION', language: 'cpp')

if meson.get_compiler('cpp').get_id() == 'msvc'
    add_project_link_arguments('/ignore:4099', language: 'cpp') # 4099 is for PDB not found, which is fine and just logspam because we don't have PDBs anyways
endif

executable(
  'kiwicubed',
  files(
    # GPU objects
    'Source/GPUObjects/IndexBufferObject.cpp',
    'Source/GPUObjects/VertexArrayObject.cpp',
    'Source/GPUObjects/VertexBufferObject.cpp',

    # World types
    'Source/world/Block.cpp',
    'Source/world/Chunk.cpp',
    'Source/world/ChunkHandler.cpp',
    'Source/world/Entity.cpp',
    'Source/world/Physics.cpp',
    'Source/world/World.cpp',

    # Rendering
    'Source/rendering/Camera.cpp',
    'Source/rendering/DebugRenderer.cpp',
    'Source/rendering/Renderer.cpp',
    'Source/rendering/Texture.cpp',
    'Source/rendering/Shader.cpp',

    'Source/Input.cpp',
    'Source/Player.cpp',
    'Source/SingleplayerHandler.cpp',
    'Source/Window.cpp',
    'Source/Main.cpp',
  ),
  include_directories: include_directories('Source', 'Source/GPUObjects', 'Source/world', 'Source/rendering'),
  dependencies: [
    dependency('kwport'),
    dependency('log4kwc', static: true),
    dependency('json'),
    dependency('glfw3', static: true),
    dependency('imgui', static: true),
    dependency('glm', static: true),
    dependency('glad-gl', static: true),
  ],
  cpp_pch: 'Source/pch/pch.h',
)
