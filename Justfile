set windows-shell := ['cmd', '/c']

run: build
	./build/kiwicubed

build:
	meson compile -C build

clean:
	rm -rf build
	meson setup build
