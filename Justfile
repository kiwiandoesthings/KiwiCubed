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
