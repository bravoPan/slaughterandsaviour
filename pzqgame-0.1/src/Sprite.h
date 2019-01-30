#ifndef PZQGAME_SPRITE_H
#define PZQGAME_SPRITE_H

#include "common.h"

class Sprite
{
 public:
  GLuint texID;
  int rows,columns;

  Sprite(const char *filename,int rows,int columns);
  ~Sprite();

  void UseThisSprite();
  void GetColorData(int ID,GLfloat dst[]);
};

#endif
