project = asteroids

all: build exec

run: reload exec

build: mkdir-build cmake-load cmake-build

reload: cmake-load cmake-build

mkdir-build:
	[ -d ./build ] | mkdir -p build

format:
	clang-format -i $$(find src include \
		-name '*.cpp' -o -name '*.hpp' \
		-o -name '*.c' -o -name '*.h')

cmake-load:
	cd build;cmake ..
	ln -sf build/compile_commands.json compile_commands.json

cmake-build:
	cd build;cmake --build . --target $(project)

clean:
	rm -rf .cache build compile_commands.json

exec:
	./build/$(project)
