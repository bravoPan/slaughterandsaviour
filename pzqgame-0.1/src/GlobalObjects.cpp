#include "common.h"
#include "GlobalObjects.h"

SDL_Window *mainWindow = NULL;
SDL_GLContext mainContext = NULL;
bool quitGame = false;
bool vsyncEnabled = false;

GLuint emptyTexture;

Renderer renderer;
AudioEngine audioEngine;

std::mt19937 randEngine(std::time(0));

MachineGame machine;

bool GlobalInitialize(){
  //SDL Initialization
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
    SDL_Log("SDL Initialization failed, %s\n",SDL_GetError());
    return false;
  }

  if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1) != 0){
    SDL_Log("%s\n",SDL_GetError());
    return false;
  }
  if(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,8) != 0){
    SDL_Log("%s\n",SDL_GetError());
    return false;
  }
  if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3) != 0){
    SDL_Log("%s\n",SDL_GetError());
    return false;
  }
  if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,2) != 0){
    SDL_Log("%s\n",SDL_GetError());
    return false;
  }
  if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE) != 0){
    SDL_Log("%s\n",SDL_GetError());
    return false;
  }

  mainWindow = SDL_CreateWindow(
				"The Demo Game, by Charlie and Gilbert",
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				0,0,
				SDL_WINDOW_OPENGL|SDL_WINDOW_FULLSCREEN_DESKTOP
			       );
  if(mainWindow == NULL){
    SDL_Log("Create Window failed, %s\n",SDL_GetError());
    return false;
  }

  mainContext = SDL_GL_CreateContext(mainWindow);
  if(mainContext == NULL){
    SDL_Log("Create OpenGL Context failed, %s\n",SDL_GetError());
    return false;
  }

  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if(err != GLEW_OK){
    std::cout << "GLEW Initialization failed, " << glewGetErrorString(err) << std::endl;
    return false;
  }

  if(SDL_GL_SetSwapInterval(1) == 0){
    vsyncEnabled = true;
    std::cout << "VSync Enabled" << std::endl;
  } else {
    std::cout << "VSync Not Enabled" << std::endl;
  }

  SDL_DisplayMode dismode;
  SDL_GetCurrentDisplayMode(0,&dismode);
  int w = dismode.w,h = dismode.h;
  if((float)w / h > 1.7778){
    w = h * 1.7778;
  } else {
    h = w * 0.5625;
  }
  int borderWidth = (dismode.w - w) / 2;
  int borderHeight = (dismode.h - h) / 2;
  glViewport(borderWidth,borderHeight,w,h);

  renderer.Initialize();
  audioEngine.Initialize();

  glClearColor(0.0,0.0,0.0,1.0);
  glClearStencil(0);
  glStencilFunc(GL_LEQUAL,1,0xFF);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  glGenTextures(1,&emptyTexture);
  glBindTexture(GL_TEXTURE_2D,emptyTexture);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1,1,0,GL_BGRA,GL_UNSIGNED_BYTE,NULL);
  glBindTexture(GL_TEXTURE_2D,0);

  return true;
}

void GlobalFinalize(){
  glUseProgram(0);
  SDL_GL_DeleteContext(mainContext);
  SDL_DestroyWindow(mainWindow);
  SDL_Quit();
}

char* ReadWholeFile(const char *filename,int *length){
  std::FILE *f = fopen(filename,"rb");
  if(f){
    char *content;
    std::fseek(f,0,SEEK_END);
    int len = std::ftell(f);
    if(length) *length = len;
    content = new char[len + 1];
    std::rewind(f);
    std::fread(content,1,len,f);
    content[len] = 0;
    std::fclose(f);
    return content;
  } else {
    return NULL;
  }
}

unsigned char * ReadPNG(const char * filename,unsigned int * width,unsigned int * height){
  FILE * fin = fopen(filename,"rb");
  png_structp png_ptr;
  png_infop info_ptr;
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
  info_ptr = png_create_info_struct(png_ptr);
  png_init_io(png_ptr,fin);
  png_read_png(png_ptr,info_ptr,PNG_TRANSFORM_BGR,0);
  png_get_IHDR(png_ptr,info_ptr,width,height,NULL,NULL,NULL,NULL,NULL);
  png_bytepp data = png_get_rows(png_ptr,info_ptr);
  unsigned char * result = new unsigned char[(*width) * (*height) * 4];
  for(int i = 0;i < *height;++i){
    for(int j = 0;j < *width * 4;++j){
      result[i * (*width) * 4 + j] = data[i][j];
    }
  }
  png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
  return result;
}
