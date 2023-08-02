# Chess
Chess GUI.

![](/screenshot.png?raw=true)

### Requirements
- Windows
- OpenGL 4.6
- C++ 17
- CMake

### Compiling
Clone the repository:
``` bash
git clone --recursive https://github.com/cucumberbolts/Chess.git
cd Chess
```
Build using CMake:
``` bash
mkdir build
cmake -B build
cmake --build build
```
Or on MSVC (Visual Studio), to build release:
```
cmake --build build --config Release
```

Note: If you modified the resources in the resources/ directory,
run `python embed_resources.py` to regenerate the resource file.

### Future features
- Linux support
- PGN
- Chess 960
- Unit tests for chess?

### Dependencies
- [glfw](https://glfw.org/)
- [glad](https://glad.dav1d.de/)
- [stb_image](https://github.com/nothings/stb/)
- [glm](https://github.com/https://github.com/g-truc/glm)
- [imgui](https://github.com/ocornut/imgui)

### Chess pieces
By jurgenwesterhof (adapted from work of Cburnett) - http://commons.wikimedia.org/wiki/Template:SVG_chess_pieces, CC BY-SA 3.0, https://commons.wikimedia.org/w/index.php?curid=35634436
