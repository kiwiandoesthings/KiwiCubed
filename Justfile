set windows-shell := ['cmd', '/c']

[unix]
run: builddebug
	BuildDebug/kiwicubed

[unix]
release: buildrelease
    BuildRelease/kiwicubed

[windows]
run: builddebug
	BuildDebug\kiwicubed.exe

[windows]
release: buildrelease
    BuildRelease\kiwicubed.exe

buildrelease:
	meson setup BuildRelease --buildtype=release -Ddefault_library=static --native-file native_64.ini
	meson compile -C BuildRelease

builddebug:
	meson compile -C BuildDebug
	@echo Current directory: %CD%
	copy BuildDebug\\compile_commands.json compile_commands.json

setup:
	meson setup BuildDebug --buildtype=debug -Dcpp_args=-DKIWI_DEBUG_MODE -Ddefault_library=static --native-file native_64.ini

clean:
	meson setup --reconfigure BuildRelease
	meson setup --reconfigure BuildDebug
