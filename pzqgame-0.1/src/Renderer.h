#ifndef PZQGAME_RENDERER_H
#define PZQGAME_RENDERER_H

#include <vector>
#ifndef GL3_PROTOTYPES
#define GL3_PROTOTYPES 1
#endif
#include <GL/glew.h>
#include "Sprite.h"
#include "Font.h"

class Renderer{
 public:
  void Initialize();
  int RegisterSpriteAtlas(const char *filename,int rows,int columns);
  void UseSpriteAtlas(int spriteAtlasID);
  void DrawQuad(int left,int right,int bottom,int top,int spriteID);
  void DrawQuads(int quadArray[],int num);
  void DrawStencil(int left,int right,int bottom,int top);
  void Draw3DTriangles(GLfloat trigVertArray[],GLfloat trigColorArray[],int num);
  void DrawPureColor(int left,int right,int bottom,int top,float r,float g,float b,float a);
  void DrawText(const char * text,int left,int top,int width,int point,float r,float g,float b,float a);
  int GetTextWidth(const char * text,int lineHeight);
  void Flush();
  void Begin3D();
  void End3D();

  int winW,winH;
  glm::mat4 cameraMat,projMat;

 private:
  GLuint vertexShader,vertex3DShader,fragmentShader,fragmentTextShader;
  GLuint shaderProgram,shader3DProgram,textProgram;
  //GLuint emptyTexture;
  GLuint cameraLoc,projLoc,textColorLoc;
  
  //We draw triangles in batches of 1000
  GLfloat vertex[6000];
  GLfloat color[6000];
  //We draw 3D triangles in batches of 400
  GLfloat vertex3D[3600];
  GLfloat color3D[2400];
  int currentBufferPtr;
  int current3DBufferPtr;
  std::vector < Sprite* > spriteAtlasList;
  Font * fontTexture;
  int currentSpriteAtlas;
  bool mode3D;

  GLuint vaoID[8],vboID[8];
  GLuint GUItex;
};
  
#endif
