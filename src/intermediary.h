// Copyright (c) 2019 by Stan Fortoński

#ifndef INTERMEDIARY
#define INTERMEDIARY 1

#pragma comment(lib, "winmm")
#pragma comment(lib, "opengl32")
#pragma comment(lib, "external/GLEW/glew32s")
#pragma comment(lib, "external/GLFW/glfw3")

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

#include <external/GLEW/glew.h>
#include <external/GLEW/glext.h>
#include <external/GLFW/glfw3.h>

#include <external/GLM/glm.hpp>
#include <external/GLM/gtc/matrix_transform.hpp>
#include <external/GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <cmath>
#include <climits>

#endif