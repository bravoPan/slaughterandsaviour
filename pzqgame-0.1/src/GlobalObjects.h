#ifndef PZQGAME_GLOBAL_OBJECTS_H
#define PZQGAME_GLOBAL_OBJECTS_H

#include "common.h"
#include "Renderer.h"

extern SDL_Window *mainWindow;
extern SDL_GLContext mainContext;
extern bool quitGame;
extern bool vsyncEnabled;

//extern GLuint vertexShader,fragmentShader;
//extern GLuint shaderProgram;
extern GLuint emptyTexture;

extern FT_Library ftLibrary;

extern Renderer renderer;

extern std::mt19937 randEngine;

bool GlobalInitialize();
void GlobalFinalize();
char* ReadWholeFile(const char *filename,int *fileLen);

#endif
