#include "common.h"
#include "Renderer.h"
#include "GlobalObjects.h"
#include "MachineGame.h"
#include "GameGlobalState.h"

inline void exchangeInt(int * a,int * b){
  int t = *a;
  *a = *b;
  *b = t;
}

void MachineGameStateMainmenu::OnKeyDown(const SDL_KeyboardEvent &ev){
  if(ev.keysym.sym == SDLK_ESCAPE){
    quitGame = true;
  } else if(ev.keysym.sym == SDLK_UP){
    upPressed = downPressed = leftPressed = rightPressed = false;
    upPressed = true;
  } else if(ev.keysym.sym == SDLK_DOWN){
    upPressed = downPressed = leftPressed = rightPressed = false;
    downPressed = true;
  } else if(ev.keysym.sym == SDLK_LEFT){
    upPressed = downPressed = leftPressed = rightPressed = false;
    leftPressed = true;
  } else if(ev.keysym.sym == SDLK_RIGHT){
    upPressed = downPressed = leftPressed = rightPressed = false;
    rightPressed = true;
  } else if(ev.keysym.sym == SDLK_p){
    for(int i = 0;i < 8;++i){
      for(int j = 0;j < 8;++j){
	std::cout << totalFrame[i][j] << ' ';
      }
      std::cout << std::endl;
    }
    for(int i = 0;i < 8;++i){
      for(int j = 0;j < 8;++j){
	std::cout << animFrame[i][j] << ' ';
      }
      std::cout << std::endl;
    }
  }
}

void MachineGameStateMainmenu::OnKeyUp(const SDL_KeyboardEvent &ev){
  if(ev.keysym.sym == SDLK_UP){
    upPressed = false;
  } else if(ev.keysym.sym == SDLK_DOWN){
    downPressed = false;
  } else if(ev.keysym.sym == SDLK_LEFT){
    leftPressed = false;
  } else if(ev.keysym.sym == SDLK_RIGHT){
    rightPressed = false;
  }
}

void MachineGameStateMainmenu::OnLogic(){
  for(int i = 0;i < 8;++i){
    for(int j = 0;j < 8;++j){
      if(inAnim[i][j]){
	++animFrame[i][j];
	if(animFrame[i][j] == totalFrame[i][j]){
	  inAnim[i][j] = false;
	  falling[i][j] = false;
	  animFrame[i][j] = 0;
	  totalFrame[i][j] = 0;
	  board[i][j] = repl[i][j];
	}
      }
    }
  }

  for(int i = 0;i < 128;++i){
    if(!tempBlockUsed[i]){continue;}
    ++tempBlocks[i].animFrame;
    if(tempBlocks[i].animFrame == tempBlocks[i].totalFrame){
      tempBlockUsed[i] = false;
    }
  }

  //Detect match-3
  bool eliminable[8][8];
  for(int i = 0;i < 8;++i){for(int j = 0;j < 8;++j){eliminable[i][j] = false;}}
  for(int i = 0;i < 8;++i){
    for(int j = 0;j < 8;++j){
      if(inAnim[i][j]){continue;}
      if(board[i][j] == -1){continue;}
      if(j <= 5 && !inAnim[i][j + 1] && !inAnim[i][j + 2]){
	if(board[i][j] == board[i][j + 1] && board[i][j] == board[i][j + 2]){
	  eliminable[i][j] = eliminable[i][j + 1] = eliminable[i][j + 2] = true;
	}
      }
      if(i <= 5 && !inAnim[i + 1][j] && !inAnim[i + 2][j]){
	if(board[i][j] == board[i + 1][j] && board[i][j] == board[i + 2][j]){
	  eliminable[i][j] = eliminable[i + 1][j] = eliminable[i + 2][j] = true;
	}
      }
    }
  }

  for(int i = 0;i < 8;++i){
    for(int j = 0;j < 8;++j){
      if(eliminable[i][j]){
	AddTempBlock(board[i][j],i,j,8,8,elimSpeed);
        board[i][j] = -1;
      }
    }
  }

  for(int i = 6;i >= 0;--i){
    for(int j = 0;j < 8;++j){
      if(inAnim[i][j] || board[i][j] == -1){continue;}
      if(board[i + 1][j] == -1 || falling[i + 1][j]){
	falling[i][j] = true;
	inAnim[i][j] = true;
	animFrame[i][j] = 0;
	totalFrame[i][j] = swapSpeed;
	dest[i][j][0] = i + 1;
	dest[i][j][1] = j;
	if(inAnim[i + 1][j]){repl[i + 1][j] = board[i][j];} else {
	  inAnim[i + 1][j] = true;
	  animFrame[i + 1][j] = 0;
	  totalFrame[i + 1][j] = swapSpeed;
	  dest[i + 1][j][0] = i + 1;
	  dest[i + 1][j][1] = j;
	  repl[i + 1][j] = board[i][j];
	}
	repl[i][j] = -1;
	if(i == 0){
	  repl[0][j] = randEngine() % 6;
	  AddTempBlock(repl[0][j],-1,j,0,j,swapSpeed);
	}
	if(i == posX && j == posY){++posX;}
      }
    }
  }

  for(int i = 0;i < 8;++i){
    if(repl[0][i] == -1){
      repl[0][i] = randEngine() % 6;
      AddTempBlock(repl[0][i],-1,i,0,i,swapSpeed);
    } else if(board[0][i] == -1 && !inAnim[0][i]){
      inAnim[0][i] = true;
      animFrame[0][i] = 0;
      totalFrame[0][i] = swapSpeed;
      dest[0][i][0] = 0;
      dest[0][i][1] = i;
      repl[0][i] = randEngine() % 6;
      AddTempBlock(repl[0][i],-1,i,0,i,swapSpeed);
    }
  }

  if(upPressed && posX >= 1 && !inAnim[posX][posY] && !inAnim[posX - 1][posY]){
    inAnim[posX][posY] = true;
    dest[posX][posY][0] = posX - 1;
    dest[posX][posY][1] = posY;
    animFrame[posX][posY] = 0;
    totalFrame[posX][posY] = swapSpeed;
    repl[posX][posY] = board[posX - 1][posY];
    inAnim[posX - 1][posY] = true;
    dest[posX - 1][posY][0] = posX;
    dest[posX - 1][posY][1] = posY;
    animFrame[posX - 1][posY] = 0;
    totalFrame[posX - 1][posY] = swapSpeed;
    repl[posX - 1][posY] = board[posX][posY];
    --posX;
  }
  if(downPressed && posX <= 6 && !inAnim[posX][posY] && !inAnim[posX + 1][posY]){
    inAnim[posX][posY] = true;
    dest[posX][posY][0] = posX + 1;
    dest[posX][posY][1] = posY;
    animFrame[posX][posY] = 0;
    totalFrame[posX][posY] = swapSpeed;
    repl[posX][posY] = board[posX + 1][posY];
    inAnim[posX + 1][posY] = true;
    dest[posX + 1][posY][0] = posX;
    dest[posX + 1][posY][1] = posY;
    animFrame[posX + 1][posY] = 0;
    totalFrame[posX + 1][posY] = swapSpeed;
    repl[posX + 1][posY] = board[posX][posY];
    ++posX;
  }
  if(leftPressed && posY >= 1 && !inAnim[posX][posY] && !inAnim[posX][posY - 1]){
    inAnim[posX][posY] = true;
    dest[posX][posY][0] = posX;
    dest[posX][posY][1] = posY - 1;
    animFrame[posX][posY] = 0;
    totalFrame[posX][posY] = swapSpeed;
    repl[posX][posY] = board[posX][posY - 1];
    inAnim[posX][posY - 1] = true;
    dest[posX][posY - 1][0] = posX;
    dest[posX][posY - 1][1] = posY;
    animFrame[posX][posY - 1] = 0;
    totalFrame[posX][posY - 1] = swapSpeed;
    repl[posX][posY - 1] = board[posX][posY];
    --posY;
  }
  if(rightPressed && posY <= 6 && !inAnim[posX][posY] && !inAnim[posX][posY + 1]){
    inAnim[posX][posY] = true;
    dest[posX][posY][0] = posX;
    dest[posX][posY][1] = posY + 1;
    animFrame[posX][posY] = 0;
    totalFrame[posX][posY] = swapSpeed;
    repl[posX][posY] = board[posX][posY + 1];
    inAnim[posX][posY + 1] = true;
    dest[posX][posY + 1][0] = posX;
    dest[posX][posY + 1][1] = posY;
    animFrame[posX][posY + 1] = 0;
    totalFrame[posX][posY + 1] = swapSpeed;
    repl[posX][posY + 1] = board[posX][posY];
    ++posY;
  }
}

void MachineGameStateMainmenu::OnRender(){
  glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
  renderer.UseSpriteAtlas(globalState -> spriteAtlasID);

  for(int i = 0;i < 8;++i){
    for(int j = 0;j < 8;++j){
      if(board[i][j] == -1){continue;}
      if(!inAnim[i][j]){
	renderer.DrawQuad(723 + 106 * j,723 + 106 * (j + 1),964 - 106 * (i + 1),964 - 106 * i,board[i][j]);
      } else {
	float p,q,r,s;
	float x,y,z,w;
	p = 723 + 106 * j;
	q = 964 - 106 * (i + 1);
	r = 723 + 106 * dest[i][j][1];
	s = 964 - 106 * (dest[i][j][0] + 1);
	x = (p * (totalFrame[i][j] - animFrame[i][j])) / totalFrame[i][j] + (r * animFrame[i][j]) / totalFrame[i][j];
	y = (q * (totalFrame[i][j] - animFrame[i][j])) / totalFrame[i][j] + (s * animFrame[i][j]) / totalFrame[i][j];
	//z = (p * animFrame) / 6 + (r * (6 - animFrame)) / 6;
	//w = (q * animFrame) / 6 + (s * (6 - animFrame)) / 6;
	renderer.DrawQuad(x,x + 106,y,y + 106,board[i][j]);
      }
    }
  }

  for(int i = 0;i < 128;++i){
    if(!tempBlockUsed[i]){continue;}
    float p,q,r,s;
    float x,y;
    p = 723 + 106 * tempBlocks[i].posY;
    q = 964 - 106 * (tempBlocks[i].posX + 1);
    r = 723 + 106 * tempBlocks[i].destPosY;
    s = 964 - 106 * (tempBlocks[i].destPosX + 1);
    x = (p * (tempBlocks[i].totalFrame - tempBlocks[i].animFrame)) / tempBlocks[i].totalFrame + (r * tempBlocks[i].animFrame) / tempBlocks[i].totalFrame;
    y = (q * (tempBlocks[i].totalFrame - tempBlocks[i].animFrame)) / tempBlocks[i].totalFrame + (s * tempBlocks[i].animFrame) / tempBlocks[i].totalFrame;
    renderer.DrawQuad(x,x + 106,y,y + 106,tempBlocks[i].blockID);
  }
  
  renderer.Flush();
  renderer.BeginCairo();
  if(GUImodified){
    cairo_t *cr = renderer.GUIcr;
    cairo_set_operator(cr,CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba(cr,0.0,0.0,0.0,0.0);
    cairo_rectangle(cr,0,0,renderer.winW,renderer.winH);
    cairo_fill(cr);
  }
  renderer.EndCairo(GUImodified);
  GUImodified = false;
  SDL_GL_SwapWindow(mainWindow);
}

void MachineGameStateMainmenu::AddTempBlock(int blockID,int beginPosX,int beginPosY,int endPosX,int endPosY,int totalFrame){
  for(int i = 0;i < 128;++i){
    if(!tempBlockUsed[i]){
      tempBlockUsed[i] = true;
      tempBlocks[i].blockID = blockID;
      tempBlocks[i].posX = beginPosX;
      tempBlocks[i].posY = beginPosY;
      tempBlocks[i].destPosX = endPosX;
      tempBlocks[i].destPosY = endPosY;
      tempBlocks[i].animFrame = 0;
      tempBlocks[i].totalFrame = totalFrame;
      return;
    }
  }
}

MachineGame::MachineGame(){
  globalState.spriteAtlasID = renderer.RegisterSpriteAtlas("Sprites.png",1,7);
  currState = new MachineGameStateMainmenu(&globalState);
}

MachineGameStateMainmenu::MachineGameStateMainmenu(GameGlobalState * const globalState) : swapSpeed(10),elimSpeed(25){
  GUImodified = true;
  this -> globalState = globalState;
  posX = posY = 0;
  upPressed = downPressed = leftPressed = rightPressed = false;

  for(int i = 0;i < 8;++i){
    for(int j = 0;j < 8;++j){
      board[i][j] = randEngine() % 6;
      inAnim[i][j] = false;
      animFrame[i][j] = 0;
      totalFrame[i][j] = 0;
    }
  }
  board[0][0] = 6;

  for(int i = 0;i < 128;++i){tempBlockUsed[i] = false;}

  currInnerState = INNERSTATE_NORMAL;
}

void MachineGame::RunGameLoop(){
  prevTime = std::chrono::steady_clock::now();
  
  while(!quitGame){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
      if(event.type == SDL_QUIT){
	quitGame = true;
      } else if(event.type == SDL_KEYDOWN){
	currState->OnKeyDown(event.key);
      } else if(event.type == SDL_KEYUP){
	currState->OnKeyUp(event.key);
      }
    }

    currTime = std::chrono::steady_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::duration<int,std::ratio<1,1000>>>(currTime - prevTime);
    
    if(dur.count() >= 15){
      currState->OnLogic();
      currState->OnRender();
      prevTime = currTime;
    }
  }
}
