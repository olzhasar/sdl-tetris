CC := gcc

CFLAGS := `sdl2-config --cflags` -ggdb3 -O0 --std=c99 -Wall
LDFLAGS := `sdl2-config --libs` -lm -lSDL2_ttf
EXTRA_FLAGS :=

HDRS :=

SRCS := $(wildcard src/*.c)

OBJS := $(SRCS:.c=.o)

EXEC := game

# Emscripten
EMCC := emcc

EM_BUILD_DIR := dist

EMFLAGS = -sUSE_SDL=2 -sUSE_SDL_TTF=2 -sALLOW_MEMORY_GROWTH -s -ggdb3 -s -O0 -s --std=c99 --preload-file src/assets/font.ttf

# default recipe
all: compile_run

compile_run: compile
	./$(EXEC)

compile: $(OBJS) $(HDRS) Makefile
	$(CC) -o $(EXEC) $(OBJS) $(LDFLAGS) $(EXTRA_FLAGS)

debug:
	$(MAKE) EXTRA_FLAGS=-fsanitize=address,undefined

%.o: %.c $(HDRS) Makefile
	$(CC) -o $@ -c $< $(CFLAGS) $(EXTRA_FLAGS)

clean:
	rm -f $(EXEC) $(OBJS)
	rm -rf $(EM_BUILD_DIR)

wasm_build_dir:
	mkdir -p $(EM_BUILD_DIR)

wasm: wasm_build_dir
	cp index.html $(EM_BUILD_DIR)
	$(EMCC) $(SRCS) $(EMFLAGS) -o $(EM_BUILD_DIR)/index.js

wasm_serve: clean wasm
	open http://localhost:8000
	python -m http.server -d $(EM_BUILD_DIR) 8000

publish:
	git checkout gh-pages
	git reset --hard master
	$(MAKE) clean
	$(MAKE) wasm
	mv -f dist/* .
	rm -rf dist/
	git rm -rf src/ README.md preview.gif
	git add --all
	git commit -m "Deploy"
	git push --force origin gh-pages
	git checkout master

.PHONY: all clean compile compile_run debug wasm wasm_serve wasm_build_dir publish
