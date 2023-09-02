# Chess

## Overview
A chess GUI written in C++ using ImGui and OpenGL. It supports Windows and Linux and requires OpenGL 4.6.

![](/screenshot.png?raw=true)

## Compiling

### Requirements
- C++ 17
- CMake

Clone the repository:
``` bash
git clone --recursive https://github.com/cucumberbolts/Chess.git
cd Chess
```
Build using CMake:
``` bash
cmake -B build
cmake --build build
```
Or on MSVC (Visual Studio), to build release:
```
cmake --build build --config Release
```

Note: If you modify the resources in the resources/ directory,
run `python embed_resources.py` to regenerate the resource file.

## Future features
- Better UCI engine integration with evaluation bar and engine settings
- PGN
- Chessboard annotations with arrows, highlighted squares
- Board editor
- Opening explorer, game database
- Chess 960, and potentially other variants

## Credits

Much of the OpenGL rendering code was inspired by [The Cherno's Game Engine Series](https://youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT).
The bitboard logic for chess move generation is from the [Chess Programming wiki](https://www.chessprogramming.org/Kindergarten_Bitboards).

### Dependencies
- Windowing: [glfw](https://glfw.org/)
- OpenGL bindings: [glad](https://glad.dav1d.de/)
- Texture loading: [stb_image](https://github.com/nothings/stb/)
- Math: [glm](https://github.com/https://github.com/g-truc/glm)
- GUI: [imgui](https://github.com/ocornut/imgui)

### Assets

 - **Chess pieces**:
By jurgenwesterhof (adapted from work of Cburnett) - http://commons.wikimedia.org/wiki/Template:SVG_chess_pieces, CC BY-SA 3.0, https://commons.wikimedia.org/w/index.php?curid=35634436

 - **Font**:
Roboto-Regular.ttf and Roboto-Bold.ttf by Christian Robetson
Apache License 2.0
https://fonts.google.com/specimen/Roboto
Note: Changed file names to roboto_regular.ttf and roboto_bold.ttf
