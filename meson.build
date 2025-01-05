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
    'Source/World/Block.cpp',
    'Source/World/BlockManager.cpp',
    'Source/World/Chunk.cpp',
    'Source/World/ChunkHandler.cpp',
    'Source/World/Entity.cpp',
    'Source/World/Physics.cpp',
    'Source/World/World.cpp',

    # Rendering
    'Source/Rendering/Camera.cpp',
    'Source/Rendering/DebugRenderer.cpp',
    'Source/Rendering/Renderer.cpp',
    'Source/Rendering/Texture.cpp',
    'Source/Rendering/Shader.cpp',

    'Source/Input.cpp',
    'Source/Player.cpp',
    'Source/SingleplayerHandler.cpp',
    'Source/Window.cpp',
    'Source/Main.cpp',
  ),
  include_directories: include_directories('Source', 'Source/GPUObjects', 'Source/World', 'Source/Rendering'),
  dependencies: [
    dependency('kwport'),
    dependency('klogger', static: true),
    dependency('json'),
    dependency('glfw3', static: true),
    dependency('imgui', static: true),
    dependency('glm', static: true),
    dependency('glad-gl', static: true),
  ],
)
