set windows-shell := ['cmd', '/c']

[unix]
run: build
	build/kiwicubed

[windows]
run: build
	build\kiwicubed.exe

build:
	meson compile -C build
	
clean:
	meson setup --reconfigure build

fmt:
	fd -e h -e cpp -E subprojects -x clang-format -i

wincross_setup:
	rm -rf build-mingw
	meson setup --cross-file x86_64-w64-mingw32.txt build-mingw

wincross_run:
	wine64 build-mingw/kiwicubed.exe

wincross:
	meson compile -C build-mingw


