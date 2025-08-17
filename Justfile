set windows-shell := ['cmd', '/c']

[unix]
run: build
	Build/kiwicubed

[windows]
run: build
	Build\kiwicubed.exe

build:
	meson compile -C Build

setup:
	meson setup Build
	
clean:
	meson setup --reconfigure Build
