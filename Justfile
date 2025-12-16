set windows-shell := ['cmd', '/c']

[unix]
run: builddebug
	BuildDebug/kiwicubed

[unix]
release: buildrelease
	BuildRelease/kiwicubed

[windows]
run: builddebug
	just buildmods
	BuildDebug\kiwicubed.exe

[windows]
release: buildrelease 
	just buildmods
	BuildRelease\kiwicubed.exe

buildrelease:
	meson setup BuildRelease --buildtype=release -Ddefault_library=static --vsenv
	meson compile -C BuildRelease

builddebug:
	meson compile -C BuildDebug
	copy BuildDebug\\compile_commands.json compile_commands.json

buildmods:
    powershell.exe -ExecutionPolicy Bypass -File BuildMods.ps1

setup:
	meson setup BuildDebug --buildtype=debug -Dcpp_args=-DKIWI_DEBUG_MODE -Ddefault_library=static --vsenv

clean:
	meson setup --reconfigure BuildRelease
	meson setup --reconfigure BuildDebug
