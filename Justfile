set windows-shell := ['cmd', '/c']

[unix]
run: builddebug
	build/kiwicubed

[unix]
release: buildrelease
    BuildRelease/kiwicubed

[windows]
run: builddebug
	build\kiwicubed.exe

[windows]
release: buildrelease
    BuildRelease\kiwicubed.exe

buildrelease:
	meson setup BuildRelease --buildtype=release -Ddefault_library=static
	meson compile -C BuildRelease

builddebug:
	meson compile -C build

setup:
	meson setup build --buildtype=debug -Dcpp_args=-DKIWI_DEBUG_MODE -Ddefault_library=static

clean:
	meson setup --reconfigure BuildRelease
	meson setup --reconfigure build
