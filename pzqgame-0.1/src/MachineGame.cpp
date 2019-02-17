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
    
    if(dur.count() >= 8){
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

void MachineGame::Initiate(){
  globalState.playerScore = 0;
  globalState.spriteAtlasID = renderer.RegisterSpriteAtlas("Sprites.png",2,7);
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
  } else if(ev.keysym.sym == SDLK_UP){
    leftPressed = rightPressed = downPressed = false;
    upPressed = true;
  } else if(ev.keysym.sym == SDLK_DOWN){
    leftPressed = rightPressed = upPressed = false;
    downPressed = true;
  } else if(ev.keysym.sym == SDLK_a){
    sPressed = dPressed = wPressed = false;
    aPressed = true;
  } else if(ev.keysym.sym == SDLK_s){
    aPressed = dPressed = wPressed = false;
    sPressed = true;
  } else if(ev.keysym.sym == SDLK_d){
    sPressed = aPressed = wPressed = false;
    dPressed = true;
  } else if(ev.keysym.sym == SDLK_w){
    sPressed = dPressed = aPressed = false;
    wPressed = true;
  }
}

void MachineGameStateWorldmap::OnKeyUp(const SDL_KeyboardEvent &ev){
  if(ev.keysym.sym == SDLK_LEFT){
    leftPressed = false;
  } else if(ev.keysym.sym == SDLK_RIGHT){
    rightPressed = false;
  } else if(ev.keysym.sym == SDLK_UP){
    upPressed = false;
  } else if(ev.keysym.sym == SDLK_DOWN){
    downPressed = false;
  } else if(ev.keysym.sym == SDLK_a){
    aPressed = false;
  } else if(ev.keysym.sym == SDLK_s){
    sPressed = false;
  } else if(ev.keysym.sym == SDLK_d){
    dPressed = false;
  } else if(ev.keysym.sym == SDLK_w){
    wPressed = false;
  }
}

bool MachineGameStateWorldmap::AttemptMove(float newPosX,float newPosY){
  int p = std::round(newPosX),q = std::round(newPosY);
  int u = p + 36,v = q + 36;
  if(maze[u][v] == 10){return false;}
  bool front = false,back = false,left = false,right = false;
  if(u > 0 && maze[u - 1][v] == 10){front = true;}
  if(u < 72 && maze[u + 1][v] == 10){back = true;}
  if(v > 0 && maze[u][v - 1] == 10){left = true;}
  if(v < 72 && maze[u][v + 1] == 10){right = true;}
  if(u > 0 && v > 0 && maze[u - 1][v - 1] == 10){front = true;left = true;}
  if(u > 0 && v < 72 && maze[u - 1][v + 1] == 10){front = true;right = true;}
  if(u < 72 && v > 0 && maze[u + 1][v - 1] == 10){back = true;left = true;}
  if(u < 72 && v < 72 && maze[u + 1][v + 1] == 10){back = true;right = true;}
  if(front && std::abs(p - (u - 37)) < 1.05){return false;}
  if(back && std::abs(p - (u - 35)) < 1.05){return false;}
  if(left && std::abs(q - (v - 37)) < 1.05){return false;}
  if(right && std::abs(q - (v - 35)) < 1.05){return false;}
  return true;
}

int MachineGameStateWorldmap::OnLogic(){
  if(escPressed){return STATE_MAINMENU;}
  if(leftPressed){xyRot += M_PI / 150;}
  if(rightPressed){xyRot -= M_PI / 150;}
  if(upPressed){yawn += M_PI / 150;}
  if(downPressed){yawn -= M_PI / 150;}
  float dirX = -cos(xyRot) / 20,dirY = -sin(xyRot) / 20;
  float newPosX = fposX,newPosY = fposY;
  if(aPressed){newPosX -= dirY;newPosY += dirX;}
  if(sPressed){newPosX -= dirX;newPosY -= dirY;}
  if(dPressed){newPosX += dirY;newPosY -= dirX;}
  if(wPressed){newPosX += dirX;newPosY += dirY;}
  int p = std::round(newPosX),q = std::round(newPosY);
  if(AttemptMove(newPosX,newPosY)){
    fposX = newPosX;fposY = newPosY;
    posX = p;posY = q;
  } else if(AttemptMove(newPosX,fposY)){
    fposX = newPosX;
    posX = p;
  } else if(AttemptMove(fposX,newPosY)){
    fposY = newPosY;
    posY = q;
  }
  if(xyRot >= M_PI){xyRot -= M_PI * 2;}
  if(xyRot <= -M_PI){xyRot += M_PI * 2;}
  if(yawn >= M_PI / 2){yawn = M_PI / 2 - 0.001;}
  if(yawn < M_PI / 18){yawn = M_PI / 18;}
  return 0;
}

void MachineGameStateWorldmap::OnRender(){
  glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  renderer.UseSpriteAtlas(globalState -> spriteAtlasID);
  renderer.Begin3D();
  renderer.cameraMat = glm::lookAt(glm::vec3(fposY,0.5 * sin(yawn) + 0.5,fposX),glm::vec3(fposY - 0.5 * std::sin(xyRot) * std::cos(yawn),0.5,fposX - 0.5 * std::cos(xyRot) * std::cos(yawn)),glm::vec3(0,1,0));
  /*
  for(int i = 0;i < 81;++i){
    worldObjects[i]->OnRender();
  }
  */
  for(int i = -10;i <= 10;++i){
    int ti = posX + i + 36;
    if(ti < 0){continue;}
    if(ti > 72){break;}
    for(int j = -10;j <= 10;++j){
      int tj = posY + j + 36;
      if(tj < 0){continue;}
      if(tj > 72){break;}
      worldObjects[ti * 73 + tj]->OnRender();
    }
  }
  renderer.End3D();
  SDL_GL_SwapWindow(mainWindow);
  return;
}

MachineGameStateWorldmap::MachineGameStateWorldmap(GameGlobalState * const globalState) : escPressed(false), leftPressed(false), rightPressed(false), aPressed(false), sPressed(false), dPressed(false), wPressed(false), posX(-33), posY(-33), fposX(-33.0), fposY(-33.0), xyRot(0), yawn(M_PI / 4){
  this -> globalState = globalState;
  renderer.projMat = glm::perspective(M_PI / 4,1.778,0.5,20.0);

  std::ifstream mazedata("maze.txt");
  char data[100];
  for(int i = 0;i < 73;++i){
    mazedata.getline(data,100);
    for(int j = 0;j < 73;++j){
      maze[i][j] = 0;
      if(data[j] == ' '){maze[i][j] = 7;}
      if(data[j] == '#'){maze[i][j] = 10;}
    }
  }
  for(int i = 0;i < 73;++i){
    for(int j = 0;j < 73;++j){
      //maze[i][j] = randEngine() % 6 + 7;
      if(maze[i][j] < 10){
	Square * sqr = new Square(-36 + j,0,-36 + i);
	sqr->texture = maze[i][j];
	sqr->SetColor();
	worldObjects[i * 73 + j] = sqr;
      } else {
	Cube * cbe = new Cube(-36 + j,0.5,-36 + i);
	for(int k = 0;k < 6;++k){
	  cbe->sideTexture[k] = maze[i][j];
	}
	cbe->SetColor();
	worldObjects[i * 73 + j] = cbe;
      }
    }
  }
  return;
}

MachineGameStateWorldmap::~MachineGameStateWorldmap(){
  for(int i = 0;i < 5329;++i){
    delete worldObjects[i];
  }
}
