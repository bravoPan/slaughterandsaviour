#ifndef PZQGAME_RENDERER_H
#define PZQGAME_RENDERER_H

#include <vector>
#ifndef GL3_PROTOTYPES
#define GL3_PROTOTYPES 1
#endif
#include <GL/glew.h>
#include <cairo/cairo.h>
#include <pango/pangocairo.h>
#include "Sprite.h"

class Renderer{
 public:
  void Initialize();
  int RegisterSpriteAtlas(const char *filename,int rows,int columns);
  void UseSpriteAtlas(int spriteAtlasID);
  void DrawQuad(int left,int right,int bottom,int top,int spriteID);
  void DrawQuads(int quadArray[],int num);
  void DrawStencil(int left,int right,int bottom,int top);
  void Draw3DTriangles(GLfloat trigVertArray[],GLfloat trigColorArray[],int num);
  void Flush();
  void Begin3D();
  void End3D();
  void BeginCairo();
  void EndCairo();

  int winW,winH;
  cairo_t *GUIcr;
  PangoLayout *pangoLayout;
  PangoFontDescription *pangoDesc;
  cairo_surface_t *GUIsurface;
  glm::mat4 cameraMat,projMat;

 private:
  GLuint vertexShader,vertex3DShader,fragmentShader;
  GLuint shaderProgram,shader3DProgram;
  //GLuint emptyTexture;
  GLuint cameraLoc,projLoc;
  
  //We draw triangles in batches of 1000
  GLfloat vertex[6000];
  GLfloat color[6000];
  //We draw 3D triangles in batches of 400
  GLfloat vertex3D[3600];
  GLfloat color3D[2400];
  int currentBufferPtr;
  int current3DBufferPtr;
  std::vector < Sprite* > spriteAtlasList;
  int currentSpriteAtlas;
  bool mode3D;

  GLuint vaoID[8],vboID[8];
  GLuint GUItex;
};
  
#endif
