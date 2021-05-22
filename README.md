# OpenGL Ocean Waves

![Ocean surface](assets/misc/ocean.gif)

### How it works
No physics calculations are performed. Instead, we use simple interpolations between normalmaps and heightmaps. Additionally, Tessellation is used to apply the heightmaps.
This project uses 14 normalmaps, 14 heightmaps, and one texture.

### Controls
```
R - start camera rotation
T - stop camera rotation
ESC - exit
```
### Libraries Used
- GLEW - http://glew.sourceforge.net
- GLFW - https://www.glfw.org
- GLM  - https://glm.g-truc.net
- STB_IMAGE - https://github.com/nothings/stb

### Requirements
- Graphics card that supports OpenGL version 4.1

### Credits
- This project is based on https://github.com/stanfortonski/Ocean-OpenGL by stanfortonski