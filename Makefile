CC := gcc

CFLAGS := $(shell sdl2-config --cflags) -O3 --std=c99
LDFLAGS := $(shell sdl2-config --libs) -lm -lSDL2_ttf
EXTRA_FLAGS :=

HDRS := $(wildcard src/*.h)

SRCS := $(wildcard src/*.c)

OBJS := $(SRCS:.c=.o)

EXEC := game

# Emscripten
EMCC := emcc
EMSDK_ENV ?= $(HOME)/emsdk/emsdk_env.sh

EM_BUILD_DIR := dist

EMFLAGS := -sUSE_SDL=2 -sUSE_SDL_TTF=2 -sALLOW_MEMORY_GROWTH=1 -O3 --std=c99 --preload-file src/assets/font.ttf@font.ttf

# default recipe
all: compile_run

compile_run: compile
	./$(EXEC)

compile: $(OBJS) $(HDRS) Makefile
	$(CC) -o $(EXEC) $(OBJS) $(LDFLAGS) $(EXTRA_FLAGS)

debug:
	$(MAKE) EXTRA_FLAGS='-ggdb3 -O0 -fsanitize=address,undefined'

%.o: %.c $(HDRS) Makefile
	$(CC) -o $@ -c $< $(CFLAGS) $(EXTRA_FLAGS)

clean:
	rm -f $(EXEC) $(OBJS)
	rm -rf $(EM_BUILD_DIR)

wasm_build_dir:
	mkdir -p $(EM_BUILD_DIR)

wasm: wasm_build_dir
	cp index.html $(EM_BUILD_DIR)/index.html
	@if command -v $(EMCC) >/dev/null 2>&1; then \
		$(EMCC) $(SRCS) $(EMFLAGS) -o $(EM_BUILD_DIR)/index.js; \
	elif [ -f "$(EMSDK_ENV)" ]; then \
		EMSDK_QUIET=1 . "$(EMSDK_ENV)" && \
		$(EMCC) $(SRCS) $(EMFLAGS) -o $(EM_BUILD_DIR)/index.js; \
	else \
		echo "emcc not found. Install/source emsdk or set EMSDK_ENV=/path/to/emsdk_env.sh"; \
		exit 1; \
	fi

wasm_serve: wasm
	python3 -m http.server -d $(EM_BUILD_DIR) 8000

publish: wasm
	scripts/deploy-gh-pages.sh

.PHONY: all clean compile compile_run debug wasm wasm_serve wasm_build_dir deploy publish
