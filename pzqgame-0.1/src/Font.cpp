#include "common.h"
#include "Font.h"
#include "GlobalObjects.h"

Font::Font(const char * filename){
  unsigned int w,h;
  unsigned char * imageData = ReadPNG(filename,&w,&h);
  charHeight = h;

  int beginCol = 0;
  int i,j;
  int charCount = 0;
  for(i = 1;i < w;++i){
    for(j = 0;j < h;++j){
      if(imageData[j * w * 4 + i * 4 + 3] != 0){break;}
    }
    if(j == h){
      if(i == beginCol){beginCol = i + 1;continue;}
      charWidth[charCount] = i - beginCol;
      beginPos[charCount] = ((GLfloat)beginCol) / w;
      endPos[charCount] = ((GLfloat)i) / w;
      //std::cout << "Char detected, width " << charWidth[charCount] << std::endl;
      beginCol = i + 1;
      ++charCount;
      if(charCount == 70){break;}
    }
  }

  glGenTextures(1,&texID);
  glBindTexture(GL_TEXTURE_2D,texID);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_BGRA,GL_UNSIGNED_INT_8_8_8_8_REV,imageData);

  delete [] imageData;
  glBindTexture(GL_TEXTURE_2D,0);
}

Font::~Font(){
  glDeleteTextures(1,&texID);
}

void Font::UseThisFont(){
  glBindTexture(GL_TEXTURE_2D,texID);
}

void Font::GetColorData(int ID,GLfloat dst[]){
  GLfloat p = beginPos[ID],q = endPos[ID],r = 0,s = 1;

  dst[0] = p;dst[1] = r;
  dst[2] = q;dst[3] = r;
  dst[4] = p;dst[5] = s;
  dst[6] = p;dst[7] = s;
  dst[8] = q;dst[9] = s;
  dst[10] = q;dst[11] = r;
}

int Font::GetCharID(char c){
  int id = -1;
  if(c >= 'A' && c <= 'Z'){
    id = c - 'A';
  } else if(c >= 'a' && c <= 'z'){
    id = c - 'a' + 26;
  } else if(c >= '0' && c <= '9'){
    id = c - '0' + 52;
  } else if(c == '.'){
    id = 62;
  } else if(c == ','){
    id = 63;
  } else if(c == ';'){
    id = 64;
  } else if(c == ':'){
    id = 65;
  } else if(c == '?'){
    id = 66;
  } else if(c == '!'){
    id = 67;
  } else if(c == '-'){
    id = 68;
  } else if(c == '_'){
    id = 69;
  }
  return id;
}
