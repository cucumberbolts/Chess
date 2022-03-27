# Chess
Chess GUI (in progress).

Download an engine and put it in the same directory as the executable.

### Requirements
- Windows (Unix support coming at some point)
- OpenGL 4.6
- C++ 17

### Compiling
Clone the repository:
``` bash
git clone --recursive https://github.com/cucumberbolts/Chess.git
cd Chess
```
Build using CMake:
``` bash
mkdir build
cd build
cmake ..
```

### Dependencies
- [glfw](https://glfw.org/)
- [glad](https://glad.dav1d.de/)
- [stb_image](https://github.com/nothings/stb/)
- [glm](https://github.com/https://github.com/g-truc/glm)
- [imgui](https://github.com/ocornut/imgui)
