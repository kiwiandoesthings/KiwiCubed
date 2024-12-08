project('kiwicubed', 'cpp', default_options : ['cpp_std=c++20'])


# Stops KiwiCubed\subprojects\glm-1.0.1\glm\gtx\../gtx/transform.hpp(23): fatal error C1189
add_project_arguments('-DGLM_ENABLE_EXPERIMENTAL', language: 'cpp')
add_project_arguments('-DSTB_IMAGE_IMPLEMENTATION', language: 'cpp')

inc = include_directories('src','includes')
sources = files('src/Block.cpp','src/Camera.cpp','src/Chunk.cpp','src/ChunkHandler.cpp','src/DebugRenderer.cpp','src/Entity.cpp','src/IndexBufferObject.cpp','src/Input.cpp','src/Main.cpp','src/Physics.cpp','src/Player.cpp','src/Renderer.cpp','src/Shader.cpp','src/SingleplayerHandler.cpp','src/Texture.cpp','src/VertexArrayObject.cpp','src/VertexBufferObject.cpp','src/Window.cpp','src/World.cpp')


executable('kiwicubed', sources, include_directories: inc, dependencies: [
	dependency('json', fallback: ['json', 'nlohmann_json_dep']),
	dependency('glfw', fallback: ['glfw', 'glfw_dep'], static: true),
	dependency('imgui', fallback: ['imgui', 'imgui_dep'], static: true),
	dependency('glm', fallback: ['glm', 'glm_dep'], static: true),
	dependency('glad-gl', fallback: ['glad-gl', 'glad_gl_dep'], static: true),
])

