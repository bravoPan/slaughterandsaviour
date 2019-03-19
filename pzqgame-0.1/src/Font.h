#ifndef PZQGAME_FONT_H
#define PZQGAME_FONT_H

#include "Sprite.h"

struct Font{
  Font(const char * filename);
  ~Font();
  void UseThisFont();
  void GetColorData(int ID,GLfloat dst[]);
  int GetCharID(char c);

  GLuint texID;
  int charHeight;
  int charWidth[70];
  GLfloat beginPos[70],endPos[70];
};

#endif
