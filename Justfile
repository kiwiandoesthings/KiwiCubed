
run: build_debug
	BuildDebug/kiwicubed

run_release: build_release
	BuildRelease/kiwicubed

setup:
	cmake -S . -B BuildDebug -DCMAKE_BUILD_TYPE=Debug
	cmake -S . -B BuildRelease -DCMAKE_BUILD_TYPE=Release

build_debug:
	cmake --build BuildDebug
build_release:
	cmake --build BuildRelease