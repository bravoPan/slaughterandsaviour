#ifndef PZQGAME_GEOMMODEL_H
#define PZQGAME_GEOMMODEL_H

#ifndef GL3_PROTOTYPES
#define GL3_PROTOTYPES 1
#endif
#include <GL/glew.h>

struct GeomModel{
  virtual void OnRender(){}
  virtual void SetPosition(float x,float y,float z){}
  virtual void SetRotation(float yaw,float pitch,float roll){}
  virtual void SetColor(){};
};

struct Cube : GeomModel{
  int sideTexture[6];//front, top, right, bottom, left, back
  void OnRender();
  void SetPosition(float x,float y,float z);
  void SetColor();
  Cube();
  Cube(float x,float y,float z);

 private:
  static const float vertex[8][3];
  static const int triangle[36];
  GLfloat trigVert[108];
  GLfloat trigColor[12];
};

struct Square : GeomModel{
  int texture;
  void OnRender();
  void SetPosition(float x,float y,float z);
  void SetRotation(float yaw,float pitch,float roll);
  void SetColor();
  Square();
  Square(float x,float y,float z);

 private:
  float posX,posY,posZ;
  static const float vertex[4][3];
  static const int triangle[6];
  GLfloat trigVert[18];
  GLfloat trigColor[2];
};

#endif
