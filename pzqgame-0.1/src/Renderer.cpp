#include "common.h"
#include "GlobalObjects.h"
#include "Renderer.h"

void Renderer::Initialize(){
  memset(vertex,0,sizeof(vertex));
  memset(color,0,sizeof(color));
  currentBufferPtr = 0;

  //SDL_GetWindowSize(mainWindow,&winW,&winH);
  winW = 1920;
  winH = 1080;

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
  GUIsurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,winW,winH);
  GUIcr = NULL;

  const GLfloat GUIvert[12] = {-1.0,1.0,1.0,1.0,-1.0,-1.0,-1.0,-1.0,1.0,-1.0,1.0,1.0};
  const GLfloat GUIcolor[12] = {0.0,0.0,1.0,0.0,0.0,1.0,0.0,1.0,1.0,1.0,1.0,0.0};
  
  glBindVertexArray(vaoID[2]);
  glBindBuffer(GL_ARRAY_BUFFER,vboID[4]);
  glBufferData(GL_ARRAY_BUFFER,12 * sizeof(GLfloat),GUIvert,GL_STATIC_DRAW);
  glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER,vboID[5]);
  glBufferData(GL_ARRAY_BUFFER,12 * sizeof(GLfloat),GUIcolor,GL_STATIC_DRAW);
  glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);

  glStencilMask(0x00);
  glStencilOp(GL_REPLACE,GL_KEEP,GL_KEEP);

  //FcConfigAppFontAddFile(FcConfigGetCurrent(),(const FcChar8*)"Hack-Regular.ttf");
  //pangoDesc = pango_font_description_from_string("Hack Regular 16");
}

int Renderer::RegisterSpriteAtlas(const char *filename,int rows,int columns){
  spriteAtlasList.push_back(std::shared_ptr < Sprite > (new Sprite(filename,rows,columns)));
  return spriteAtlasList.size() - 1;
}

void Renderer::DrawQuad(int left,int right,int bottom,int top,int spriteID){
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
  for(int i = 0;i < num;++i){
    DrawQuad(quadArray[5 * i],quadArray[5 * i + 1],quadArray[5 * i + 2],quadArray[5 * i + 3],quadArray[5 * i + 4]);
  }
}

void Renderer::Flush(){
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
}

void Renderer::DrawStencil(int left,int right,int bottom,int top){
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

void Renderer::BeginCairo(){
  //glBindTexture(GL_TEXTURE_2D,emptyTexture);
  GUIcr = cairo_create(GUIsurface);
  //pangoLayout = pango_cairo_create_layout(GUIcr);
  //pango_layout_set_font_description(pangoLayout,pangoDesc);
}

void Renderer::EndCairo(bool modified){
  //g_object_unref(pangoLayout);
  //pangoLayout = NULL;
  cairo_destroy(GUIcr);
  GUIcr = NULL;
  //cairo_gl_surface_swapbuffers(GUIsurface);
  //Render GUI to screen
  glBindVertexArray(vaoID[2]);
  glBindTexture(GL_TEXTURE_2D,GUItex);
  if(modified){
    unsigned char *GUIdata = cairo_image_surface_get_data(GUIsurface);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,winW,winH,0,GL_BGRA,GL_UNSIGNED_BYTE,GUIdata);
  }
  glDrawArrays(GL_TRIANGLES,0,6);
}
