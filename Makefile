CC := gcc

CFLAGS := `sdl2-config --cflags` -ggdb3 -O0 --std=c99 -Wall
LDFLAGS := `sdl2-config --libs` -lm -lSDL2_ttf
EXTRA_FLAGS :=

HDRS :=

SRCS := $(wildcard src/*.c)

OBJS := $(SRCS:.c=.o)

EXEC := game

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

.PHONY: all clean compile
