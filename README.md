# SDL Tetris

An implementation of the classic [Tetris game](https://en.wikipedia.org/wiki/Tetris) in C programming language using [SDL-2 library](https://www.libsdl.org/)

## Webassembly

The game has been ported to Webassembly using [Emscripten](https://emscripten.org/)

Check out the [Live Demo](https://olzhasar.github.io/sdl-tetris/)

## Desktop preview

![Preview](preview.gif)

## Installation

Clone this repository to your system:

```sh
git clone https://github.com/olzhasar/sdl-tetris.git
```

[Install](https://wiki.libsdl.org/SDL2/Installation) SDL2 and SDL2_ttf on your machine

On MacOS you can use Homebrew:

```sh
brew install sdl2 sdl2_ttf
```

Compile and run the project code with make

```sh
cd sdl-tetris
make
```

## Controls

- Rotate - Up arrow, W
- Left - Left arrow, A
- Right - Right arrow, D
- Soft Drop - Down arrow, S
- Hard drop - Space
- Quit - ESC

## TODO

- Levels
- Sounds
