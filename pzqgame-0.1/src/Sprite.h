#ifndef PZQGAME_SPRITE_H
#define PZQGAME_SPRITE_H

#ifndef GL3_PROTOTYPES
#define GL3_PROTOTYPES
#endif
#include <GL/glew.h>

struct Sprite
{
  GLuint texID;
  int rows,columns;
  GLfloat bleedWidth,bleedHeight;

  Sprite(const char *filename,int rows,int columns);
  ~Sprite();

  void UseThisSprite();
  void GetColorData(int ID,GLfloat dst[]);
  void GetHalfColorData(int halfID,GLfloat dst[]);
};

#endif
