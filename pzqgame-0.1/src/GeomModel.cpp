#include "GeomModel.h"
#include "common.h"
#include "GlobalObjects.h"

const float Cube::vertex[8][3] = {
				  {-0.5,0.5,0.5},
				  {0.5,0.5,0.5},
				  {0.5,0.5,-0.5},
				  {-0.5,0.5,-0.5},
				  {-0.5,-0.5,0.5},
				  {0.5,-0.5,0.5},
				  {0.5,-0.5,-0.5},
				  {-0.5,-0.5,-0.5}
};

const int Cube::triangle[36] = {
				0,1,4,4,5,1,
				3,2,0,0,1,2,
				1,2,5,5,6,2,
				7,6,4,4,5,6,
				3,0,7,7,4,0,
				3,2,7,7,6,2
};

Cube::Cube() : Cube(0.0,0.0,0.0) {}

Cube::Cube(float x,float y,float z){
  int i;
  for(i = 0;i < 36;++i){
    trigVert[i * 3] = vertex[triangle[i]][0] + x;
    trigVert[i * 3 + 1] = vertex[triangle[i]][1] + y;
    trigVert[i * 3 + 2] = vertex[triangle[i]][2] + z;
  }
  for(i = 0;i < 12;++i){trigColor[i] = 0;}
}

void Cube::SetPosition(float x,float y,float z){
  int i;
  for(i = 0;i < 36;++i){
    trigVert[i * 3] = vertex[triangle[i]][0] + x;
    trigVert[i * 3 + 1] = vertex[triangle[i]][1] + y;
    trigVert[i * 3 + 2] = vertex[triangle[i]][2] + z;
  }
  for(i = 0;i < 12;++i){trigColor[i] = 0;}
}

void Cube::SetColor(){
  int i;
  for(i = 0;i < 6;++i){
    trigColor[i * 2] = sideTexture[i] * 2;
    trigColor[i * 2 + 1] = sideTexture[i] * 2 + 1;
  }
}

void Cube::OnRender(){
  renderer.Draw3DTriangles(trigVert,trigColor,12);
}

const float Square::vertex[4][3] = {
				    {-0.5,0,0.5},
				    {0.5,0,0.5},
				    {0.5,0,-0.5},
				    {-0.5,0,-0.5}
};

const int Square::triangle[6] = {
			       3,2,0,0,1,2
};

Square::Square() : Square(0.0,0.0,0.0) {}

Square::Square(float x,float y,float z){
  int i;
  for(i = 0;i < 6;++i){
    trigVert[i * 3] = vertex[triangle[i]][0] + x;
    trigVert[i * 3 + 1] = vertex[triangle[i]][1] + y;
    trigVert[i * 3 + 2] = vertex[triangle[i]][2] + z;
  }
  for(i = 0;i < 2;++i){trigColor[i] = 0;}
  posX = x;posY = y;posZ = z;
}

void Square::SetPosition(float x,float y,float z){
  int i;
  for(i = 0;i < 6;++i){
    trigVert[i * 3] = vertex[triangle[i]][0] + x;
    trigVert[i * 3 + 1] = vertex[triangle[i]][1] + y;
    trigVert[i * 3 + 2] = vertex[triangle[i]][2] + z;
  }
  for(i = 0;i < 2;++i){trigColor[i] = 0;}
  posX = x;posY = y;posZ = z;
}

void Square::SetColor(){
  trigColor[0] = texture * 2;
  trigColor[1] = texture * 2 + 1;
}

void Square::SetRotation(float yaw,float pitch,float roll){
  int i;
  glm::mat4 rotMatrix = glm::eulerAngleYXZ(yaw,pitch,roll);
  for(i = 0;i < 6;++i){
    glm::vec4 tmpVertex = glm::vec4(
				    vertex[triangle[i]][0],
				    vertex[triangle[i]][1],
				    vertex[triangle[i]][2],
				    1.0
				    );
    tmpVertex = rotMatrix * tmpVertex;
    trigVert[i * 3] = tmpVertex[0] + posX;
    trigVert[i * 3 + 1] = tmpVertex[1] + posY;
    trigVert[i * 3 + 2] = tmpVertex[2] + posZ;
  }
}

void Square::OnRender(){
  renderer.Draw3DTriangles(trigVert,trigColor,2);
}
