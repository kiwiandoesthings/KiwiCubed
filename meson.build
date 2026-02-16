project('kiwicubed', ['c', 'cpp'], default_options: ['cpp_std=c++20'])

# Stops KiwiCubed\subprojects\glm-1.0.1\glm\gtx\../gtx/transform.hpp(23): fatal error C1189
add_project_arguments('-DGLM_ENABLE_EXPERIMENTAL', language: 'cpp')
add_project_arguments('-GL_WITH_GLAD', language: 'c')
add_project_arguments('/wd4200', language: 'cpp')
add_project_arguments('/wd4002', language: 'cpp')
add_project_arguments('/wd4003', language: 'cpp')

if host_machine.system() == 'linux'
  add_project_arguments('-g', language: 'cpp')
endif

if meson.get_compiler('cpp').get_id() == 'msvc'
    add_project_link_arguments('/ignore:4099', language: 'cpp')
    add_project_arguments('/Zi', language: 'cpp')
    add_project_link_arguments('/DEBUG', language: 'cpp')
endif

rc_compiler = find_program('rc', required: true)

app_icon = custom_target(
  'app_icon',
  input: 'Source/AppIcon.rc',
  output: 'AppIcon.res',
  command: [rc_compiler, '/nologo', '/fo', '@OUTPUT@', '@INPUT@'],
  build_by_default: true
)

freetype = dependency('freetype2', static: true, default_options: ['png=disabled', 'zlib=disabled'])

executable(
  'kiwicubed', 
  [
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
    'Source/World/EntityManager.cpp',
    'Source/World/Inventory.cpp',
    'Source/World/Physics.cpp',
    'Source/World/Player.cpp',
    'Source/World/SingleplayerHandler.cpp',
    'Source/World/World.cpp',

    # Rendering
    'Source/Rendering/Camera.cpp',
    'Source/Rendering/DebugRenderer.cpp',
    'Source/Rendering/Renderer.cpp',
    'Source/Rendering/RenderThread.cpp',
    'Source/Rendering/TextRenderer.cpp',
    'Source/Rendering/Texture.cpp',
    'Source/Rendering/Shader.cpp',

    # Other
    'Source/AssetManager.cpp',
    'Source/Events.cpp',
	'Source/GenerateAngelScriptLanguageData.cpp',
    'Source/Globals.cpp',
    'Source/Input.cpp',
    'Source/KiwiCubed.cpp',
    'Source/Main.cpp',
    'Source/ModHandler.cpp',
    'Source/ThreadPool.cpp',
    'Source/UI.cpp',
    'Source/UIButton.cpp',
    'Source/UIImage.cpp',
    'Source/Window.cpp',

    # Windows
    app_icon,
  ],
  include_directories: include_directories('Source', 'Source/GPUObjects', 'Source/World', 'Source/Rendering'),
  dependencies: [
    dependency('kwport', static: true),
    dependency('klogger', static: true),
    dependency('json', static: true),
    dependency('glfw3', static: true),
    dependency('imgui', static: true),
    dependency('glm', static: true),
    dependency('glad-gl', static: true),
    dependency('fmt', static: true),
	dependency('angelscript', static: true),
    freetype
  ],
)