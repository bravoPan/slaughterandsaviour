#include "common.h"
#include "Renderer.h"
#include "GlobalObjects.h"
#include "MachineGame.h"
#include "GameGlobalState.h"

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
	if(t == STATE_WORLDMAP){
	  currState = new MachineGameStateWorldmap(&globalState);
	}
	currState->OnLogic();
      }
      currState->OnRender();
      prevTime = currTime;
    }
  }
}

MachineGame::MachineGame(){
  globalState.playerScore = 0;
  globalState.spriteAtlasID = renderer.RegisterSpriteAtlas("Sprites.png",1,7);
  currState = new MachineGameStateMainMenu(&globalState);
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

  for(int i = 0;i < 128;++i){
    if(!tempBlockUsed[i]){continue;}
    ++tempBlocks[i].animFrame;
    if(tempBlocks[i].animFrame == tempBlocks[i].totalFrame){
      if(tempBlocks[i].replBlock){
	board[tempBlocks[i].destPosX][tempBlocks[i].destPosY] = tempBlocks[i].blockID;
      }
      tempBlockUsed[i] = false;
    }
  }

  //Detect match-3
  bool eliminable[8][8];
  for(int i = 0;i < 8;++i){for(int j = 0;j < 8;++j){eliminable[i][j] = false;}}
  for(int i = 0;i < 8;++i){
    for(int j = 0;j < 8;++j){
      if(board[i][j] == -1){continue;}
      if(j <= 5){
	if(board[i][j] == board[i][j + 1] && board[i][j] == board[i][j + 2]){
	  eliminable[i][j] = eliminable[i][j + 1] = eliminable[i][j + 2] = true;
	}
      }
      if(i <= 5){
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
	boardEmpty[i][j] = true;
	currScore += 20;
	GUImodified = true;
      }
    }
  }

  for(int i = 6;i >= 0;--i){
    for(int j = 0;j < 8;++j){
      if(board[i][j] == -1){continue;}
      if(boardEmpty[i + 1][j]){
	AddTempBlock(board[i][j],i,j,i + 1,j,swapSpeed);
	boardEmpty[i][j] = true;
	board[i][j] = -1;
	if(i == 0){
	  AddTempBlock(randEngine() % 6,-1,j,0,j,swapSpeed);
	  boardEmpty[i][j] = false;
	}
	if(i == posX && j == posY){++posX;}
      }
    }
  }

  for(int i = 0;i < 8;++i){
    if(boardEmpty[0][i]){
      AddTempBlock(randEngine() % 6,-1,i,0,i,swapSpeed);
    }
  }

  if(upPressed && posX >= 1 && board[posX][posY] != -1 && !(board[posX - 1][posY] == -1 && !boardEmpty[posX - 1][posY])){
    boardEmpty[posX][posY] = boardEmpty[posX - 1][posY] = true;
    AddTempBlock(board[posX][posY],posX,posY,posX - 1,posY,swapSpeed);
    AddTempBlock(board[posX - 1][posY],posX - 1,posY,posX,posY,swapSpeed);
    board[posX][posY] = board[posX - 1][posY] = -1;
    --posX;
  }
  if(downPressed && posX <= 6 && board[posX][posY] != -1 && !(board[posX + 1][posY] == -1 && !boardEmpty[posX + 1][posY])){
    
    boardEmpty[posX][posY] = boardEmpty[posX + 1][posY] = true;
    AddTempBlock(board[posX][posY],posX,posY,posX + 1,posY,swapSpeed);
    AddTempBlock(board[posX + 1][posY],posX + 1,posY,posX,posY,swapSpeed);
    board[posX][posY] = board[posX + 1][posY] = -1;
    ++posX;
  }
  if(leftPressed && posY >= 1 && board[posX][posY] != -1 && !(board[posX][posY - 1] == -1 && !boardEmpty[posX][posY - 1])){
    boardEmpty[posX][posY] = boardEmpty[posX][posY - 1] = true;
    AddTempBlock(board[posX][posY],posX,posY,posX,posY - 1,swapSpeed);
    AddTempBlock(board[posX][posY - 1],posX,posY - 1,posX,posY,swapSpeed);
    board[posX][posY] = board[posX][posY - 1] = -1;
    --posY;
  }
  if(rightPressed && posY <= 6 && board[posX][posY] != -1 && !(board[posX][posY + 1] == -1 && !boardEmpty[posX][posY + 1])){
    boardEmpty[posX][posY] = boardEmpty[posX][posY + 1] = true;
    AddTempBlock(board[posX][posY],posX,posY,posX,posY + 1,swapSpeed);
    AddTempBlock(board[posX][posY + 1],posX,posY + 1,posX,posY,swapSpeed);
    board[posX][posY] = board[posX][posY + 1] = -1;
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
      renderer.DrawQuad(723 + 106 * j,723 + 106 * (j + 1),964 - 106 * (i + 1),964 - 106 * i,board[i][j]);
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
  if(blockID == -1){return;}
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
      if(endPosX != 8){tempBlocks[i].replBlock = true;boardEmpty[endPosX][endPosY] = false;} else {tempBlocks[i].replBlock = false;}
      return;
    }
  }
}

MachineGameStateMatchGame::MachineGameStateMatchGame(GameGlobalState * const globalState) : swapSpeed(9),elimSpeed(20){
  GUImodified = true;
  this -> globalState = globalState;
  posX = posY = 0;
  upPressed = downPressed = leftPressed = rightPressed = escPressed = false;

  for(int i = 0;i < 8;++i){
    for(int j = 0;j < 8;++j){
      board[i][j] = randEngine() % 6;
      boardEmpty[i][j] = false;
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
    if(currOption == 0){return STATE_WORLDMAP;}
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

void MachineGameStateWorldmap::OnKeyDown(const SDL_KeyboardEvent &ev){
  if(ev.keysym.sym == SDLK_ESCAPE){
    escPressed = true;
  } else if(ev.keysym.sym == SDLK_LEFT){
    rightPressed = upPressed = downPressed = false;
    leftPressed = true;
  } else if(ev.keysym.sym == SDLK_RIGHT){
    leftPressed = upPressed = downPressed = false;
    rightPressed = true;
  }
}

void MachineGameStateWorldmap::OnKeyUp(const SDL_KeyboardEvent &ev){
  if(ev.keysym.sym == SDLK_LEFT){
    leftPressed = false;
  } else if(ev.keysym.sym == SDLK_RIGHT){
    rightPressed = false;
  }
}

int MachineGameStateWorldmap::OnLogic(){
  if(escPressed){return STATE_MAINMENU;}
  if(leftPressed){xyRot += M_PI / 360;}
  if(rightPressed){xyRot -= M_PI / 360;}
  return 0;
}

void MachineGameStateWorldmap::OnRender(){
  glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  renderer.UseSpriteAtlas(globalState -> spriteAtlasID);
  renderer.Begin3D();
  renderer.cameraMat = glm::lookAt(glm::vec3(4 * std::sin(xyRot),6,4 * std::cos(xyRot)),glm::vec3(0,0,0),glm::vec3(0,1,0));
  GLfloat trigVert[18];
  GLfloat trigColor[2];
  for(int i = 0;i < 9;++i){
    for(int j = 0;j < 9;++j){
      trigVert[0] = -4.5 + j;
      trigVert[1] = 0;
      trigVert[2] = -4.5 + i;
      trigVert[3] = -3.5 + j;
      trigVert[4] = 0;
      trigVert[5] = -4.5 + i;
      trigVert[6] = -4.5 + j;
      trigVert[7] = 0;
      trigVert[8] = -3.5 + i;
      trigVert[9] = -4.5 + j;
      trigVert[10] = 0;
      trigVert[11] = -3.5 + i;
      trigVert[12] = -3.5 + j;
      trigVert[13] = 0;
      trigVert[14] = -3.5 + i;
      trigVert[15] = -3.5 + j;
      trigVert[16] = 0;
      trigVert[17] = -4.5 + i;
      trigColor[0] = maze[i][j] * 2;
      trigColor[1] = maze[i][j] * 2 + 1;
      renderer.Draw3DTriangles(trigVert,trigColor,2);
    }
  }
  renderer.End3D();
  SDL_GL_SwapWindow(mainWindow);
  return;
}

MachineGameStateWorldmap::MachineGameStateWorldmap(GameGlobalState * const globalState) : escPressed(false), leftPressed(false), rightPressed(false){
  this -> globalState = globalState;
  renderer.projMat = glm::perspective((M_PI * 45)/180,1.778,0.5,20.0);
  for(int i = 0;i < 9;++i){
    for(int j = 0;j < 9;++j){
      maze[i][j] = randEngine() % 6;
    }
  }
  return;
}
