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

void MachineGameStateMatchGame::OnKeyDown(const SDL_KeyboardEvent &ev){
  if(ev.keysym.sym == SDLK_ESCAPE){
    escPressed = true;
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

void MachineGameStateMatchGame::OnKeyUp(const SDL_KeyboardEvent &ev){
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

int MachineGameStateMatchGame::OnLogic(){
  if(escPressed){return 1;}
  if(currInnerState == INNERSTATE_BEGIN){
    ++beginAnimFrame;
    GUImodified = true;
    if(beginAnimFrame == 180){currInnerState = INNERSTATE_NORMAL;}
    return 0;
  }
  
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
	currScore += 20;
	GUImodified = true;
      }
    }
  }

  for(int i = 6;i >= 0;--i){
    for(int j = 0;j < 8;++j){
      if(inAnim[i][j] || board[i][j] == -1){continue;}
      if(inAnim[i + 1][j] && repl[i + 1][j] != -1){continue;}
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

  if(upPressed && posX >= 1 && !inAnim[posX][posY] && !inAnim[posX - 1][posY] /*&& board[posX - 1][posY] != -1*/){
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
  if(downPressed && posX <= 6 && !inAnim[posX][posY] && !inAnim[posX + 1][posY] /*&& board[posX + 1][posY] != -1*/){
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
  if(leftPressed && posY >= 1 && !inAnim[posX][posY] && !inAnim[posX][posY - 1] /*&& board[posX][posY - 1] != -1*/){
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
  if(rightPressed && posY <= 6 && !inAnim[posX][posY] && !inAnim[posX][posY + 1] /*&& board[posX][posY + 1] != -1*/){
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

  return 0;
}

void MachineGameStateMatchGame::OnRender(){
  glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
  glEnable(GL_STENCIL_TEST);
  renderer.DrawStencil(723,723 + 9 * 106,964 - 9 * 106,964);
  renderer.UseSpriteAtlas(globalState -> spriteAtlasID);

  for(int i = 0;i < 8;++i){
    for(int j = 0;j < 8;++j){
      if(board[i][j] == -1){continue;}
      if(!inAnim[i][j]){
	renderer.DrawQuad(723 + 106 * j,723 + 106 * (j + 1),964 - 106 * (i + 1),964 - 106 * i,board[i][j]);
      } else {
	float p,q,r,s;
	float x,y;
	p = 723 + 106 * j;
	q = 964 - 106 * (i + 1);
	r = 723 + 106 * dest[i][j][1];
	s = 964 - 106 * (dest[i][j][0] + 1);
	x = (p * (totalFrame[i][j] - animFrame[i][j])) / totalFrame[i][j] + (r * animFrame[i][j]) / totalFrame[i][j];
	y = (q * (totalFrame[i][j] - animFrame[i][j])) / totalFrame[i][j] + (s * animFrame[i][j]) / totalFrame[i][j];
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
  glDisable(GL_STENCIL_TEST);
  renderer.BeginCairo();
  if(GUImodified){
    cairo_t *cr = renderer.GUIcr;
    PangoLayout *textLayout = renderer.pangoLayout;
    cairo_set_operator(cr,CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba(cr,0.0,0.0,0.0,0.0);
    cairo_rectangle(cr,0,0,renderer.winW,renderer.winH);
    cairo_fill(cr);
    if(currInnerState == INNERSTATE_BEGIN){
      cairo_move_to(cr,25,25);
      cairo_set_source_rgba(cr,0.0,1.0,0.0,1.0);
      char beginText[64];
      sprintf(beginText,"Ready? %d",3 - beginAnimFrame / 60);
      pango_layout_set_text(textLayout,beginText,-1);
      pango_cairo_show_layout(cr,textLayout);
    } else {
      char scoreText[64];
      sprintf(scoreText,"Current Score: %d",currScore);
      cairo_move_to(cr,25,25);
      cairo_set_source_rgba(cr,0.0,1.0,0.0,1.0);
      pango_layout_set_text(textLayout,scoreText,-1);
      pango_cairo_show_layout(cr,textLayout);
    }
  }
  renderer.EndCairo(GUImodified);
  GUImodified = false;
  SDL_GL_SwapWindow(mainWindow);
}

void MachineGameStateMatchGame::AddTempBlock(int blockID,int beginPosX,int beginPosY,int endPosX,int endPosY,int totalFrame){
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
  globalState.playerScore = 0;
  globalState.spriteAtlasID = renderer.RegisterSpriteAtlas("Sprites.png",1,7);
  currState = new MachineGameStateMainMenu(&globalState);
}

MachineGameStateMatchGame::MachineGameStateMatchGame(GameGlobalState * const globalState) : swapSpeed(9),elimSpeed(20){
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

  currInnerState = INNERSTATE_BEGIN;
  beginAnimFrame = 0;
  currScore = 0;
}

void MachineGameStateMainMenu::OnKeyDown(const SDL_KeyboardEvent &ev){
  if(ev.keysym.sym == SDLK_ESCAPE){
    quitGame = true;
  } else if(ev.keysym.sym == SDLK_RETURN){
    enterPressed = true;
  } else if(ev.keysym.sym == SDLK_DOWN){
    downPressed = true;
  } else if(ev.keysym.sym == SDLK_UP){
    upPressed = true;
  }
}

int MachineGameStateMainMenu::OnLogic(){
  ++titleAnimFrame;
  if(titleAnimFrame == 480){
    titleAnimFrame = 0;++titleLineNo;
    if(titleLineNo == 7){titleLineNo = 0;}
  }
  if(downPressed){downPressed = false;++currOption;if(currOption == 2){currOption = 0;}}
  if(upPressed){upPressed = false;--currOption;if(currOption == -1){currOption = 1;}}
  if(enterPressed){
    enterPressed = false;
    if(currOption == 0){return STATE_MATCHGAME;}
    if(currOption == 1){quitGame = true;}
  }
  return 0;
}

void MachineGameStateMainMenu::OnRender(){
  glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
  renderer.BeginCairo();
  cairo_t *cr = renderer.GUIcr;
  PangoLayout *textLayout = renderer.pangoLayout;
  PangoFontDescription *fontDesc = renderer.pangoDesc;
  cairo_set_operator(cr,CAIRO_OPERATOR_SOURCE);
  cairo_set_source_rgba(cr,0.0,0.0,0.0,0.0);
  cairo_rectangle(cr,0,0,renderer.winW,renderer.winH);
  cairo_fill(cr);
  if(currOption == 1){
    cairo_set_source_rgba(cr,0.0,1.0,0.0,1.0);
  } else {
    cairo_set_source_rgba(cr,1.0,0.0,0.0,1.0);
  }
  cairo_move_to(cr,151,505);
  pango_layout_set_text(textLayout,"Start Game",-1);
  pango_cairo_show_layout(cr,textLayout);
  if(currOption == 0){
    cairo_set_source_rgba(cr,0.0,1.0,0.0,1.0);
  } else {
    cairo_set_source_rgba(cr,1.0,0.0,0.0,1.0);
  }
  cairo_move_to(cr,151,637);
  pango_layout_set_text(textLayout,"Exit Game",-1);
  pango_cairo_show_layout(cr,textLayout);
  cairo_move_to(cr,879,740 - 0.833 * titleAnimFrame);
  cairo_set_source_rgba(cr,std::sin(((double)titleAnimFrame) / 960 * M_PI),std::cos(((double)titleAnimFrame) /  960 * M_PI),0.0,std::sin(((double)titleAnimFrame) / 480 * M_PI) * 0.8 + 0.2);
  pango_layout_set_width(textLayout,750 * PANGO_SCALE);
  pango_layout_set_markup(textLayout,titleLines[titleLineNo],-1);
  pango_cairo_show_layout(cr,textLayout);
  renderer.EndCairo(true);
  
  SDL_GL_SwapWindow(mainWindow);
}

MachineGameStateMainMenu::MachineGameStateMainMenu(GameGlobalState* globalState) : currOption(0),downPressed(false),upPressed(false),enterPressed(false),titleLineNo(0),titleAnimFrame(0),
			     titleLines{"<span font='24'>Slaughter and Savior: A Game by Charlie and Gilbert</span>",
					"<span font='24'>Once upon a time there were four kingdoms</span>",
					"<span font='24'>Each one held a part of a sacred sword that guarded the peace of their land</span>",
					"<span font='24'>Yet as the kings grew more and more greedy</span>",
					"<span font='24'>They were stuck in an endless war, vying for complete control over the sword</span>",
					"<span font='24'>The war exhausted all resources on the land, collapsing the kingdoms</span>",
					"<span font='24'>Can you find all the parts of the sacred sword, and bring peace and prosperity back to this desolate havoc?</span>"}
{return;}

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
      int t = currState->OnLogic();
      if(t != 0){
	delete currState;
	if(t == STATE_MAINMENU){
	  currState = new MachineGameStateMainMenu(&globalState);
	}
	if(t == STATE_MATCHGAME){
	  currState = new MachineGameStateMatchGame(&globalState);
	}
	currState->OnLogic();
      }
      currState->OnRender();
      prevTime = currTime;
    }
  }
}
