#include "common.h"
#include "GlobalObjects.h"
#include "Sprite.h"

Sprite::Sprite(const char *filename,int rows,int columns){
  unsigned int w,h;
  unsigned char * imageData = ReadPNG(filename,&w,&h);

  bleedWidth = 1.0f / w;
  bleedHeight = 1.0f / h;
  
  glGenTextures(1,&texID);
  glBindTexture(GL_TEXTURE_2D,texID);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_BGRA,GL_UNSIGNED_INT_8_8_8_8_REV,imageData);
  
  delete [] imageData;
  glBindTexture(GL_TEXTURE_2D,0);

  this->rows = rows;
  this->columns = columns;
}

Sprite::~Sprite(){
  glDeleteTextures(1,&texID);
}

void Sprite::UseThisSprite(){
  glBindTexture(GL_TEXTURE_2D,texID);
}

void Sprite::GetColorData(int ID,GLfloat dst[]){
  if(ID >= rows * columns){
    std::memset(dst,0,12 * sizeof(GLfloat));
    return;
  }
  int a = ID / columns;
  int b = ID % columns;
  GLfloat p = (1.0f / columns) * b + bleedWidth;
  GLfloat q = (1.0f / columns) * (b + 1) - bleedWidth;
  GLfloat r = (1.0f / rows) * a + bleedHeight;
  GLfloat s = (1.0f / rows) * (a + 1) - bleedHeight;

  dst[0] = p;dst[1] = r;
  dst[2] = q;dst[3] = r;
  dst[4] = p;dst[5] = s;
  dst[6] = p;dst[7] = s;
  dst[8] = q;dst[9] = s;
  dst[10] = q;dst[11] = r;
}

void Sprite::GetHalfColorData(int halfID,GLfloat dst[]){
  int ID = halfID / 2;
  if(ID >= rows * columns){
    std::memset(dst,0,12 * sizeof(GLfloat));
    return;
  }
  int a = ID / columns;
  int b = ID % columns;
  GLfloat p = (1.0f / columns) * b + bleedWidth;
  GLfloat q = (1.0f / columns) * (b + 1) - bleedWidth;
  GLfloat r = (1.0f / rows) * a + bleedHeight;
  GLfloat s = (1.0f / rows) * (a + 1) - bleedHeight;

  if((halfID & 1) == 0){
    dst[0] = p;dst[1] = r;
    dst[2] = q;dst[3] = r;
    dst[4] = p;dst[5] = s;
  } else {
    dst[0] = p;dst[1] = s;
    dst[2] = q;dst[3] = s;
    dst[4] = q;dst[5] = r;
  }
}
