#ifndef PZQGAME_GLOBAL_OBJECTS_H
#define PZQGAME_GLOBAL_OBJECTS_H

#include <SDL.h>
//#include <fontconfig/fontconfig.h>
#ifndef GL3_PROTOTYPES
#define GL3_PROTOTYPES 1
#endif
#include <GL/glew.h>
#include "Renderer.h"
#include "AudioEngine.h"
#include "MachineGame.h"

extern SDL_Window *mainWindow;
extern SDL_GLContext mainContext;
extern bool quitGame;
extern bool vsyncEnabled;

extern GLuint emptyTexture;

//extern FT_Library ftLibrary;

extern Renderer renderer;
extern AudioEngine audioEngine;

extern std::mt19937 randEngine;

extern MachineGame machine;

bool GlobalInitialize();
void GlobalFinalize();
char * ReadWholeFile(const char *filename,int *fileLen);
unsigned char * ReadPNG(const char * filename,unsigned int * width,unsigned int * height);

#endif
