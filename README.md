# SDL Tetris

An implementation of the classic [Tetris game](https://en.wikipedia.org/wiki/Tetris) in C programming language using [SDL-2 library](https://www.libsdl.org/)

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

Compile the project code

```sh
cd sdl-tetris
make
```

Run the executable

```sh
./game
```

## Controls

- Rotate - Up arrow, W
- Left - Left arrow, A
- Right - Right arrow, D
- Drop - Down arrow, S, Space
- Quit - ESC

## TODO

- Different block colors
- Port to WASM
