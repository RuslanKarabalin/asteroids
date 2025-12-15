project = asteroids

# Emscripten compiler
CC = em++

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build_web
RAYLIB_INCLUDE = external/raylib/include
RAYLIB_LIB = external/raylib/lib/libraylib.wasm.a
ENTT_INCLUDE = external/entt/include

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Output
OUTPUT = $(BUILD_DIR)/$(project).js

# Compiler flags
CFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -Werror -Wno-error=missing-field-initializers
CFLAGS += -I$(INCLUDE_DIR) -I$(RAYLIB_INCLUDE) -I$(ENTT_INCLUDE)

# Emscripten flags
EMFLAGS = -s USE_GLFW=3
EMFLAGS += -s ASYNCIFY
EMFLAGS += -s TOTAL_MEMORY=67108864
EMFLAGS += -s FORCE_FILESYSTEM=1
EMFLAGS += -s ALLOW_MEMORY_GROWTH=1
EMFLAGS += -s ERROR_ON_UNDEFINED_SYMBOLS=0

# Debug or Release
EMFLAGS += -O2

all: mkdir-build build

mkdir-build:
	mkdir -p $(BUILD_DIR)

build: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) $(RAYLIB_LIB) $(EMFLAGS) -o $(OUTPUT)
	@echo '<!DOCTYPE html>' > $(BUILD_DIR)/index.html
	@echo '<html lang="en">' >> $(BUILD_DIR)/index.html
	@echo '<head>' >> $(BUILD_DIR)/index.html
	@echo '    <meta charset="UTF-8">' >> $(BUILD_DIR)/index.html
	@echo '    <meta name="viewport" content="width=device-width, initial-scale=1.0">' >> $(BUILD_DIR)/index.html
	@echo '    <title>Asteroids</title>' >> $(BUILD_DIR)/index.html
	@echo '    <style>' >> $(BUILD_DIR)/index.html
	@echo '        html, body { margin: 0; padding: 0; overflow: hidden; background-color: #1a1a2e; display: flex; justify-content: center; align-items: center; min-height: 100vh; font-family: monospace; }' >> $(BUILD_DIR)/index.html
	@echo '        #container { text-align: center; max-width: 100%; max-height: 100vh; }' >> $(BUILD_DIR)/index.html
	@echo '        canvas { border: 2px solid #16213e; border-radius: 4px; background-color: #0f3460; max-width: 100%; height: auto; }' >> $(BUILD_DIR)/index.html
	@echo '        h1 { color: #e94560; margin-bottom: 20px; }' >> $(BUILD_DIR)/index.html
	@echo '    </style>' >> $(BUILD_DIR)/index.html
	@echo '</head>' >> $(BUILD_DIR)/index.html
	@echo '<body>' >> $(BUILD_DIR)/index.html
	@echo '    <div id="container">' >> $(BUILD_DIR)/index.html
	@echo '        <canvas id="canvas"></canvas>' >> $(BUILD_DIR)/index.html
	@echo '    </div>' >> $(BUILD_DIR)/index.html
	@echo '    <script>' >> $(BUILD_DIR)/index.html
	@echo '        var Module = {' >> $(BUILD_DIR)/index.html
	@echo '            canvas: document.getElementById("canvas"),' >> $(BUILD_DIR)/index.html
	@echo '            print: function(text) { console.log(text); },' >> $(BUILD_DIR)/index.html
	@echo '            printErr: function(text) { console.error(text); }' >> $(BUILD_DIR)/index.html
	@echo '        };' >> $(BUILD_DIR)/index.html
	@echo '    </script>' >> $(BUILD_DIR)/index.html
	@echo '    <script src="$(project).js"></script>' >> $(BUILD_DIR)/index.html
	@echo '</body>' >> $(BUILD_DIR)/index.html
	@echo '</html>' >> $(BUILD_DIR)/index.html

run:
	emrun --browser=chrome $(BUILD_DIR)/index.html

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all mkdir-build build run clean
