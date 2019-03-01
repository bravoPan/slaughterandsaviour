#ifndef PZQGAME_COMMON_H
#define PZQGAME_COMMON_H

#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <ctime>
#include <vector>
#include <random>
#include <chrono>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#define GL3_PROTOTYPES 1
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <cairo.h>
#include <cairo-ft.h>
#include <fontconfig/fontconfig.h>
#include <pango/pangocairo.h>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

#endif
