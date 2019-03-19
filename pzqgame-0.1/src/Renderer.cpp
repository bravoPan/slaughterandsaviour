#include "common.h"
#include "GlobalObjects.h"
#include "Renderer.h"

void ReadShader(GLuint shaderID,const char * filename){
  int fileLen;
  char * shaderlog;
  
  char * source = ReadWholeFile(filename,&fileLen);
  glShaderSource(shaderID,1,(const GLchar**)&source,&fileLen);
  glCompileShader(shaderID);
  glGetShaderiv(shaderID,GL_INFO_LOG_LENGTH,&fileLen);
  shaderlog = new char[fileLen + 10];
  glGetShaderInfoLog(shaderID,fileLen,&fileLen,shaderlog);
  std::cout << shaderlog << std::endl;
  
  delete [] source;
  delete [] shaderlog;
}

void LinkProgram(GLuint shaderProgram,GLuint vertexShader,GLuint fragmentShader){
  int fileLen;
  char * shaderlog;
  
  glAttachShader(shaderProgram,vertexShader);
  glAttachShader(shaderProgram,fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram,GL_INFO_LOG_LENGTH,&fileLen);
  shaderlog = new char[fileLen + 10];
  glGetProgramInfoLog(shaderProgram,fileLen,&fileLen,shaderlog);
  std::cout << shaderlog << std::endl;

  delete [] shaderlog;
}

void Renderer::Initialize(){
  memset(vertex,0,sizeof(vertex));
  memset(color,0,sizeof(color));
  currentBufferPtr = 0;
  current3DBufferPtr = 0;
  mode3D = false;

  //Set window size 16:9
  winW = 1920;
  winH = 1080;

  //Create shaders
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  ReadShader(vertexShader,"PZQGame.vert");

  vertex3DShader = glCreateShader(GL_VERTEX_SHADER);
  ReadShader(vertex3DShader,"PZQGame3D.vert");

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  ReadShader(fragmentShader,"PZQGame.frag");

  fragmentTextShader = glCreateShader(GL_FRAGMENT_SHADER);
  ReadShader(fragmentTextShader,"PZQGameText.frag");

  shaderProgram = glCreateProgram();
  LinkProgram(shaderProgram,vertexShader,fragmentShader);

  shader3DProgram = glCreateProgram();
  LinkProgram(shader3DProgram,vertex3DShader,fragmentShader);
  cameraLoc = glGetUniformLocation(shader3DProgram,"camera");
  projLoc = glGetUniformLocation(shader3DProgram,"proj");

  textProgram = glCreateProgram();
  LinkProgram(textProgram,vertexShader,fragmentTextShader);
  textColorLoc = glGetUniformLocation(textProgram,"textColor");
  
  glUseProgram(shaderProgram);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertex3DShader);
  glDeleteShader(fragmentTextShader);

  glGenVertexArrays(8,vaoID);
  glGenBuffers(8,vboID);

  glBindVertexArray(vaoID[0]);
  glBindBuffer(GL_ARRAY_BUFFER,vboID[0]);
  glBufferData(GL_ARRAY_BUFFER,sizeof(vertex),NULL,GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER,vboID[1]);
  glBufferData(GL_ARRAY_BUFFER,sizeof(color),NULL,GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER,0);
  
  glBindVertexArray(vaoID[1]); //For Stencil Drawing
  glBindBuffer(GL_ARRAY_BUFFER,vboID[2]);
  glBufferData(GL_ARRAY_BUFFER,12 * sizeof(GLfloat),NULL,GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER,vboID[3]);
  glBufferData(GL_ARRAY_BUFFER,12 * sizeof(GLfloat),color,GL_STATIC_DRAW); //color is filled with NULL anyway
  glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);

  glGenTextures(1,&GUItex);
  glBindTexture(GL_TEXTURE_2D,GUItex);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,winW,winH,0,GL_BGRA,GL_UNSIGNED_BYTE,NULL);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  const GLfloat GUIvert[12] = {-1.0,1.0,1.0,1.0,-1.0,-1.0,-1.0,-1.0,1.0,-1.0,1.0,1.0};
  const GLfloat GUIcolor[12] = {0.0,0.0,1.0,0.0,0.0,1.0,0.0,1.0,1.0,1.0,1.0,0.0};

  glBindVertexArray(vaoID[3]); //For 3D Drawing
  glBindBuffer(GL_ARRAY_BUFFER,vboID[6]);
  glBufferData(GL_ARRAY_BUFFER,3600 * sizeof(GLfloat),NULL,GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER,vboID[7]);
  glBufferData(GL_ARRAY_BUFFER,2400 * sizeof(GLfloat),NULL,GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);

  glStencilMask(0x00);
  glStencilOp(GL_REPLACE,GL_KEEP,GL_KEEP);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_ALWAYS);

  fontTexture = new Font("Font.png");
}

int Renderer::RegisterSpriteAtlas(const char *filename,int rows,int columns){
  spriteAtlasList.push_back(new Sprite(filename,rows,columns));
  return spriteAtlasList.size() - 1;
}

void Renderer::DrawQuad(int left,int right,int bottom,int top,int spriteID){
  if(mode3D){return;}
  if(currentBufferPtr == 6000){Flush();}
  
  GLfloat p = 2.0f / winW * left - 1.0f;
  GLfloat q = 2.0f / winW * right - 1.0f;
  GLfloat r = 2.0f / winH * bottom - 1.0f;
  GLfloat s = 2.0f / winH * top - 1.0f;

  int c = currentBufferPtr;

  vertex[c + 0] = p;vertex[c + 1] = s;
  vertex[c + 2] = q;vertex[c + 3] = s;
  vertex[c + 4] = p;vertex[c + 5] = r;
  vertex[c + 6] = p;vertex[c + 7] = r;
  vertex[c + 8] = q;vertex[c + 9] = r;
  vertex[c + 10] = q;vertex[c + 11] = s;

  spriteAtlasList[currentSpriteAtlas]->GetColorData(spriteID,color + c);

  currentBufferPtr += 12;
}

void Renderer::DrawQuads(int quadArray[],int num){
  if(mode3D){return;}
  for(int i = 0;i < num;++i){
    DrawQuad(quadArray[5 * i],quadArray[5 * i + 1],quadArray[5 * i + 2],quadArray[5 * i + 3],quadArray[5 * i + 4]);
  }
}

void Renderer::Flush(){
  if(!mode3D){
    if(currentBufferPtr == 0){return;}
    glBindVertexArray(vaoID[0]);
    glBindBuffer(GL_ARRAY_BUFFER,vboID[0]);
    glBufferSubData(GL_ARRAY_BUFFER,0,currentBufferPtr * sizeof(GLfloat),vertex);
    glBindBuffer(GL_ARRAY_BUFFER,vboID[1]);
    glBufferSubData(GL_ARRAY_BUFFER,0,currentBufferPtr * sizeof(GLfloat),color);
    glDrawArrays(GL_TRIANGLES,0,currentBufferPtr / 2);
    currentBufferPtr = 0;
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
  } else {
    if(current3DBufferPtr == 0){return;}
    glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, &cameraMat[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projMat[0][0]);
    glBindVertexArray(vaoID[3]);
    glBindBuffer(GL_ARRAY_BUFFER,vboID[6]);
    glBufferSubData(GL_ARRAY_BUFFER,0,current3DBufferPtr * 3 * sizeof(GLfloat),vertex3D);
    glBindBuffer(GL_ARRAY_BUFFER,vboID[7]);
    glBufferSubData(GL_ARRAY_BUFFER,0,current3DBufferPtr * 2 * sizeof(GLfloat),color3D);
    glDrawArrays(GL_TRIANGLES,0,current3DBufferPtr);
    current3DBufferPtr = 0;
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
  }
}

void Renderer::DrawStencil(int left,int right,int bottom,int top){
  if(mode3D){return;}
  glBindVertexArray(vaoID[1]);

  GLfloat p = 2.0f / winW * left - 1.0f;
  GLfloat q = 2.0f / winW * right - 1.0f;
  GLfloat r = 2.0f / winH * bottom - 1.0f;
  GLfloat s = 2.0f / winH * top - 1.0f;

  GLfloat vertData[12] = {p,s,q,s,p,r,p,r,q,r,q,s};
  glBindBuffer(GL_ARRAY_BUFFER,vboID[2]);
  glBufferSubData(GL_ARRAY_BUFFER,0,12 * sizeof(GLfloat),vertData);

  glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
  glStencilMask(0xFF);
  
  glDrawArrays(GL_TRIANGLES,0,6);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);

  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  glStencilMask(0x00);
}

void Renderer::UseSpriteAtlas(int spriteAtlasID){
  spriteAtlasList[spriteAtlasID]->UseThisSprite();
  currentSpriteAtlas = spriteAtlasID;
}

void Renderer::Begin3D(){
  if(mode3D){return;}
  glUseProgram(shader3DProgram);
  glDepthFunc(GL_LESS);
  mode3D = true;
}

void Renderer::End3D(){
  if(!mode3D){return;}
  Flush();
  glDepthFunc(GL_ALWAYS);
  glUseProgram(shaderProgram);
  mode3D = false;
}

void Renderer::Draw3DTriangles(GLfloat trigVertArray[],GLfloat trigColorArray[],int num){
  if(!mode3D){return;}
  for(int i = 0;i < num;++i){
    if(current3DBufferPtr == 1200){Flush();}
    for(int j = 0;j < 9;++j){
      vertex3D[current3DBufferPtr * 3 + j] = trigVertArray[i * 9 + j];
    }
    for(int j = 0;j < 6;++j){
      spriteAtlasList[currentSpriteAtlas]->GetHalfColorData(trigColorArray[i],color3D + current3DBufferPtr * 2);
    }
    current3DBufferPtr += 3;
  }
}

void Renderer::DrawPureColor(int left,int right,int bottom,int top,float rf,float gf,float bf,float af){
  if(mode3D){return;}
  Flush();
  glBindTexture(GL_TEXTURE_2D,emptyTexture);
  if(rf > 1){rf = 1;}
  if(gf > 1){gf = 1;}
  if(bf > 1){bf = 1;}
  if(af > 1){af = 1;}
  unsigned char data[4] = {bf * 255,gf * 255,rf * 255,af * 255};
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1,1,0,GL_BGRA,GL_UNSIGNED_INT_8_8_8_8_REV,data);

  GLfloat p = 2.0f / winW * left - 1.0f;
  GLfloat q = 2.0f / winW * right - 1.0f;
  GLfloat r = 2.0f / winH * bottom - 1.0f;
  GLfloat s = 2.0f / winH * top - 1.0f;

  vertex[0] = p;vertex[1] = s;
  vertex[2] = q;vertex[3] = s;
  vertex[4] = p;vertex[5] = r;
  vertex[6] = p;vertex[7] = r;
  vertex[8] = q;vertex[9] = r;
  vertex[10] = q;vertex[11] = s;

  for(int i = 0;i < 12;++i){color[i] = 0.5;}
  currentBufferPtr = 12;
  Flush();
  spriteAtlasList[currentSpriteAtlas]->UseThisSprite();
}

void Renderer::DrawText(const char * text,int left,int top,int width,int lineHeight,float r,float g,float b,float a){
  if(mode3D){return;}
  Flush();
  fontTexture->UseThisFont();
  glUseProgram(textProgram);
  glUniform4f(textColorLoc,r,g,b,a);
  
  int i = 0,j,t;
  int pointerX = top - lineHeight,pointerY = left;
  GLfloat coeff = ((GLfloat)lineHeight) / fontTexture->charHeight;
  
  while(text[i] != 0){
    if(text[i] == ' '){
      ++i;
      pointerY += lineHeight / 2;
      if(pointerY - left >= width){
	pointerY = left;
	pointerX -= lineHeight + 5;
      }
      continue;
    }

    j = i;t = pointerY;
    while(text[j] != 0 && text[j] != ' '){
      int id = fontTexture->GetCharID(text[j]);
      if(id == -1){++j;continue;}
      t += fontTexture->charWidth[id] * coeff + 5;
      ++j;
    }
    if(t - pointerY > 5){t -= 5;}
    if(t - left > width){
      pointerX -= lineHeight + 5;
      pointerY = left;
    }

    while(i < j){
      int id = fontTexture->GetCharID(text[i]);
      if(id == -1){++i;continue;}
      
      GLfloat p = 2.0f / winW * pointerY - 1.0f;
      GLfloat q = 2.0f / winW * (pointerY + fontTexture->charWidth[id] * coeff)  - 1.0f;
      GLfloat r = 2.0f / winH * pointerX - 1.0f;
      GLfloat s = 2.0f / winH * (pointerX + lineHeight) - 1.0f;
      
      int c = currentBufferPtr;
      
      vertex[c + 0] = p;vertex[c + 1] = s;
      vertex[c + 2] = q;vertex[c + 3] = s;
      vertex[c + 4] = p;vertex[c + 5] = r;
      vertex[c + 6] = p;vertex[c + 7] = r;
      vertex[c + 8] = q;vertex[c + 9] = r;
      vertex[c + 10] = q;vertex[c + 11] = s;
      fontTexture->GetColorData(id,color + c);
      
      currentBufferPtr += 12;
      if(currentBufferPtr == 6000){Flush();}
      
      ++i;
      pointerY += fontTexture->charWidth[id] * coeff + 5;
      /*
	if(pointerY - left >= width){
          pointerY = left;
          pointerX -= lineHeight + 5;
	}
      */
    }
  }
  Flush();
  spriteAtlasList[currentSpriteAtlas]->UseThisSprite();
  glUseProgram(shaderProgram);
}

int Renderer::GetTextWidth(const char * text,int lineHeight){
  int i = 0;
  int result = 0,lastSpace = 0;
  GLfloat coeff = ((GLfloat)lineHeight) / fontTexture->charHeight;
  while(text[i] != 0){
    if(text[i] == ' '){result += lineHeight / 2;lastSpace += lineHeight / 2;}
    int id = fontTexture->GetCharID(text[i]);
    if(id == -1){++i;continue;}
    result += fontTexture->charWidth[id] * coeff + 5;
    lastSpace = 5;
    ++i;
  }
  return result - lastSpace;
}
