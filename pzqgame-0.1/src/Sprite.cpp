#include "Sprite.h"

Sprite::Sprite(const char *filename,int rows,int columns){
  SDL_Surface *sprite_image = IMG_Load(filename);
  SDL_Surface *converted_image = SDL_ConvertSurfaceFormat(sprite_image,SDL_PIXELFORMAT_RGBA32,0);
  
  glGenTextures(1,&texID);
  glBindTexture(GL_TEXTURE_2D,texID);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,converted_image->w,converted_image->h,0,GL_BGRA,GL_UNSIGNED_INT_8_8_8_8_REV,converted_image->pixels);

  static const GLint swizzle[] = {GL_BLUE,GL_GREEN,GL_RED,GL_ALPHA};
  glTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_SWIZZLE_RGBA,swizzle);

  SDL_FreeSurface(sprite_image);
  SDL_FreeSurface(converted_image);
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
  GLfloat p = (1.0f / columns) * b;
  GLfloat q = (1.0f / columns) * (b + 1);
  GLfloat r = (1.0f / rows) * a;
  GLfloat s = (1.0f / rows) * (a + 1);

  dst[0] = p;dst[1] = r;
  dst[2] = q;dst[3] = r;
  dst[4] = p;dst[5] = s;
  dst[6] = p;dst[7] = s;
  dst[8] = q;dst[9] = s;
  dst[10] = q;dst[11] = r;
}
