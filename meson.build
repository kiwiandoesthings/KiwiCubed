project('kiwicubed', 'cpp', default_options : ['cpp_std=c++20'])


# Stops KiwiCubed\subprojects\glm-1.0.1\glm\gtx\../gtx/transform.hpp(23): fatal error C1189
add_project_arguments('-DGLM_ENABLE_EXPERIMENTAL', language: 'cpp')
add_project_arguments('-DSTB_IMAGE_IMPLEMENTATION', language: 'cpp')

inc = include_directories('Source', 'Vendor')
sources = files('Source/Block.cpp', 'Source/Camera.cpp', 'Source/Chunk.cpp', 'Source/ChunkHandler.cpp', 'Source/DebugRenderer.cpp', 'Source/Entity.cpp', 'Source/IndexBufferObject.cpp', 'Source/Input.cpp', 'Source/Main.cpp', 'Source/Physics.cpp', 'Source/Player.cpp', 'Source/Renderer.cpp', 'Source/Shader.cpp', 'Source/SingleplayerHandler.cpp', 'Source/Texture.cpp', 'Source/VertexArrayObject.cpp', 'Source/VertexBufferObject.cpp', 'Source/Window.cpp', 'Source/World.cpp')


executable('kiwicubed', sources, include_directories: inc, dependencies: [
	dependency('json', fallback: ['json', 'nlohmann_json_dep']),
	dependency('glfw', fallback: ['glfw', 'glfw_dep'], static: true),
	dependency('imgui', fallback: ['imgui', 'imgui_dep'], static: true),
	dependency('glm', fallback: ['glm', 'glm_dep'], static: true),
	dependency('glad-gl', fallback: ['glad-gl', 'glad_gl_dep'], static: true),
])

