#ifndef PZQGAME_RENDERER_H
#define PZQGAME_RENDERER_H

#include "common.h"
#include "Sprite.h"

class Renderer{
 public:
  void Initialize();
  int RegisterSpriteAtlas(const char *filename,int rows,int columns);
  void UseSpriteAtlas(int spriteAtlasID);
  void DrawQuad(int left,int right,int bottom,int top,int spriteID);
  void DrawQuads(int quadArray[],int num);
  void DrawStencil(int left,int right,int bottom,int top);
  void Flush();
  void BeginCairo();
  void EndCairo(bool modified);

  int winW,winH;
  cairo_t *GUIcr;
  PangoLayout *pangoLayout;
  PangoFontDescription *pangoDesc;
  cairo_surface_t *GUIsurface;
  //FT_Face GUIfont;
  //cairo_font_face_t *GUIfontcr;

 private:
  //We draw triangles in batches of 1000
  GLfloat vertex[6000];
  GLfloat color[6000];
  int currentBufferPtr;
  std::vector < std::shared_ptr < Sprite > > spriteAtlasList;
  int currentSpriteAtlas;

  GLuint vaoID[8],vboID[8];
  GLuint GUItex;
};
  
#endif
