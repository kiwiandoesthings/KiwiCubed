project('kiwicubed', 'cpp', default_options : ['cpp_std=c++20'])


# Stops KiwiCubed\subprojects\glm-1.0.1\glm\gtx\../gtx/transform.hpp(23): fatal error C1189
add_project_arguments('-DGLM_ENABLE_EXPERIMENTAL', language: 'cpp')


inc = include_directories('src')
sources = files('src/Block.cpp','src/Camera.cpp','src/Chunk.cpp','src/ChunkHandler.cpp','src/DebugRenderer.cpp','src/Entity.cpp','src/IndexBufferObject.cpp','src/Input.cpp','src/Main.cpp','src/Physics.cpp','src/Player.cpp','src/Renderer.cpp','src/Shader.cpp','src/SingleplayerHandler.cpp','src/Texture.cpp','src/VertexArrayObject.cpp','src/VertexBufferObject.cpp','src/Window.cpp','src/World.cpp')

executable('kiwicubed', sources, include_directories: inc, dependencies: [
	dependency('json', fallback: ['json', 'nlohmann_json_dep']),
	dependency('glfw', fallback: ['glfw', 'glfw_dep']),
	dependency('imgui'),
	dependency('glm'),
	dependency('FastNoise', fallback: ['FastNoise', 'FastNoise_dep']),
	dependency('GLAD', fallback: ['GLAD', 'GLAD_dep']),
	dependency('stb', fallback: ['stb', 'stb_dep']),
])

