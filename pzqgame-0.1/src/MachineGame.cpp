#include "common.h"
#include "Renderer.h"
#include "GlobalObjects.h"
#include "MachineGame.h"
#include "GameGlobalState.h"
#include "Quest.h"
#include "DisjointSet.h"
#include <stack>

void MachineGameStateBase::DestroyInjections(){
  auto iter = injections.begin();
  while(iter != injections.end()){
    delete (*iter);
    ++iter;
  }
  iter = backupInjections.begin();
  while(iter != backupInjections.end()){
    delete (*iter);
    ++iter;
  }
  auto iter2 = renderObjects.begin();
  while(iter2 != renderObjects.end()){
    delete (*iter2);
    ++iter2;
  }
  iter2 = backupRenderObjects.begin();
  while(iter2 != backupRenderObjects.end()){
    delete (*iter2);
    ++iter2;
  }
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
    auto dur = std::chrono::duration_cast< std::chrono::duration< int,std::ratio< 1,1000 > > >(currTime - prevTime);
    int tick = dur.count();if(tick > 100){tick = 100;}
    if(tick >= 8){
      int t = currState->OnLogic(tick);
      if(t != 0){
	currState->DestroyInjections();
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
	currState->OnLogic(tick);
      }
      currState->OnRender();
      prevTime = currTime;
    }
  }
}

void MachineGame::Initiate(){
  globalState.playerScore = 0;
  globalState.spriteAtlasID = renderer.RegisterSpriteAtlas("Sprites.png",3,7);
  globalState.lastState = 3;
  currState = new MachineGameStateMainMenu(&globalState);
  
  globalState.fposX = -33.0;
  globalState.fposY = -33.0;
  globalState.xyRot = 0;
  globalState.yawn = M_PI / 18;

  globalState.inQuest = false;
  globalState.currQuest = NULL;

  for(int i = 0;i < 12;++i){
    for(int j = 0;j < 12;++j){
      globalState.questList[i][j] = NULL;
    }
  }

  MazeGen();
}

void MachineGame::MazeGen(){
  int i,j,k;
  char result[25][25];
  std::memset(result,1,25 * 25);

  for(i = 1;i < 25;i += 2){
    for(j = 1;j < 25;j += 2){
      result[i][j] = 0;
    }
  }
  DisjointSet thisRow(12),nextRow(12);
  for(i = 0;i < 12;++i){
    for(j = 0;j < 11;++j){
      if((thisRow.Find(j) != thisRow.Find(j + 1)) && (randEngine() % 2 == 0)){
	result[i * 2 + 1][j * 2 + 2] = 0;
	thisRow.Union(j,j + 1);
      }
    }
    if(i < 11){
      for(j = 0;j < 12;++j){
	if(randEngine() % 2 == 0){
	  result[i * 2 + 2][j * 2 + 1] = 0;
	  int t = thisRow.Find(j);
	  if(thisRow.nextData[t] == -1){
	    thisRow.nextData[t] = j;
	  } else {
	    nextRow.Union(thisRow.nextData[t],j);
	  }
	}
      }
      for(j = 0;j < 12;++j){
	int t = thisRow.Find(j);
	if(thisRow.nextData[t] == -1){
	  result[i * 2 + 2][j * 2 + 1] = 0;
	  thisRow.nextData[t] = j;
	}
      }
      thisRow.Copy(nextRow);
      nextRow.Init();
    }
  }
  for(int i = 0;i < 11;++i){
    if(thisRow.Find(i) != thisRow.Find(i + 1)){
      result[23][i * 2 + 2] = 0;
      thisRow.Union(i,i + 1);
    }
  }

  char visited[25][25];
  std::memset(visited,0,25 * 25);
  visited[1][1] = 1;
  int count = 1;
  int currX = 1,currY = 1;
  int newX,newY;
  static const int dir[4][2] = {{-2,0},{2,0},{0,-2},{0,2}};
  std::stack<int> xSt,ySt;
  xSt.push(-1);ySt.push(-1);
  do{
    for(i = 0;i < 4;++i){
      newX = currX + dir[i][0];
      newY = currY + dir[i][1];
      if(newX < 0 || newX > 24){continue;}
      if(newY < 0 || newY > 24){continue;}
      if(result[(currX + newX)/2][(currY + newY)/2]){continue;}
      if(visited[newX][newY]){continue;}
      visited[newX][newY] = 1;
      xSt.push(currX);
      ySt.push(currY);

      Quest * qst = new DestroyGate((currX + newX)/2,(currY + newY)/2,i,0);
      QuestList * wrp = new QuestList;
      wrp->qst = qst;
      wrp->prev = NULL;
      wrp->next = globalState.questList[(currX - 1)/2][(currY - 1)/2];
      if(globalState.questList[(currX - 1)/2][(currY - 1)/2] != NULL){
	globalState.questList[(currX - 1)/2][(currY - 1)/2] -> prev = wrp;
      }
      globalState.questList[(currX - 1)/2][(currY - 1)/2] = wrp;
      
      currX = newX;currY = newY;
      ++count;
      break;
    }
    if(i < 4){continue;}
    currX = xSt.top();xSt.pop();
    currY = ySt.top();ySt.pop();
  } while(!xSt.empty());

  for(i = 0;i < 73;++i){
    for(j = 0;j < 73;++j){
      globalState.maze[i][j] = 7;
      globalState.isWall[i][j] = false;
    }
  }
  for(i = 0;i < 73;++i){
    for(j = 0;j < 73;j += 6){
      globalState.maze[i][j] = 10;
      globalState.isWall[i][j] = true;
      globalState.maze[j][i] = 10;
      globalState.isWall[j][i] = true;
    }
  }
  for(i = 1;i < 24;i += 2){
    for(j = 2;j < 24;j += 2){
      if(result[i][j] == 0){
	for(k = 2;k <= 4;++k){
	  int p = (i - 1) * 3 + k,q = j * 3;
	  globalState.maze[p][q] = 11;
	  globalState.isWall[p][q] = true;
	}
      }
      if(result[j][i] == 0){
	for(k = 2;k <= 4;++k){
	  int p = j * 3,q = (i - 1) * 3 + k;
	  globalState.maze[p][q] = 11;
	  globalState.isWall[p][q] = true;
	}
      }
    }
  }
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
  } else if(ev.keysym.sym == SDLK_LCTRL || ev.keysym.sym == SDLK_RCTRL){
    ctrlPressed = true;
    shiftPressed = false;
  } else if(ev.keysym.sym == SDLK_LSHIFT || ev.keysym.sym == SDLK_RSHIFT){
    shiftPressed = true;
    ctrlPressed = false;
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
  } else if(ev.keysym.sym == SDLK_LCTRL || ev.keysym.sym == SDLK_RCTRL){
    ctrlPressed = false;
  } else if(ev.keysym.sym == SDLK_LSHIFT || ev.keysym.sym == SDLK_RSHIFT){
    shiftPressed = false;
  }
}

void MachineGameStateMatchGame::CyclicMove(int dir,int pos){
  //dir: 0 -> Counterclockwise, 1 -> Clockwise
  //pos: 0 -> begin from upperleft corner
  //     1 ->            upperright corner
  //     2 ->            lowerright corner
  //     3 ->            lowerleft corner

  const int x = posX;
  const int y = posY;
  const int disp[2][4][2] = {{{1,0},{0,-1},{-1,0},{0,1}},{{0,1},{1,0},{0,-1},{-1,0}}};
  int u = x,v = y,t = pos;
  do{
    if(u < 0 || u >= 8){return;}
    if(v < 0 || v >= 8){return;}
    if(board[u][v] == -1 && !boardEmpty[u][v]){return;}
    u += disp[dir][t][0];
    v += disp[dir][t][1];
    if(dir){t = (t + 1) % 4;} else {t = (t + 3) % 4;}
  }while(t != pos);
  u = x;v = y;t = pos;
  do{
    boardEmpty[u][v] = true;
    u += disp[dir][t][0];
    v += disp[dir][t][1];
    if(dir){t = (t + 1) % 4;} else {t = (t + 3) % 4;}
  }while(t != pos);
  u = x;v = y;t = pos;
  do{
    AddTempBlock(board[u][v],u,v,u + disp[dir][t][0],v + disp[dir][t][1],swapSpeed);
    u += disp[dir][t][0];
    v += disp[dir][t][1];
    if(dir){t = (t + 1) % 4;} else {t = (t + 3) % 4;}
  }while(t != pos);
  u = x;v = y;t = pos;
  do{
    board[u][v] = -1;
    u += disp[dir][t][0];
    v += disp[dir][t][1];
    if(dir){t = (t + 1) % 4;} else {t = (t + 3) % 4;}
  }while(t != pos);
  posX += disp[dir][pos][0];
  posY += disp[dir][pos][1];
}

int MachineGameStateMatchGame::OnLogic(int tick){
  globalState->playerScore = currScore;
  
  if(escPressed){return 1;}
  
  for(int i = 0;i < 128;++i){
    if(!tempBlockUsed[i]){continue;}
    tempBlocks[i].animFrame += tick;
    if(tempBlocks[i].animFrame >= tempBlocks[i].totalFrame){
      if(tempBlocks[i].replBlock){
	board[tempBlocks[i].destPosX][tempBlocks[i].destPosY] = tempBlocks[i].blockID;
      }
      tempBlockUsed[i] = false;
    }
  }

  if(canElim){
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
	}
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

  if(currScore > globalState->goalScore && !lockControl){
    lockControl = true;
    MatchGameVictoryAnim *ptr = new MatchGameVictoryAnim;
    injections.push_back(ptr);
    renderObjects.push_back(ptr->render);
  }

  if(!lockControl && board[posX][posY] != -1){
    if(!ctrlPressed && !shiftPressed){
      if(upPressed && posX >= 1 && !(board[posX - 1][posY] == -1 && !boardEmpty[posX - 1][posY])){
	boardEmpty[posX][posY] = boardEmpty[posX - 1][posY] = true;
	AddTempBlock(board[posX][posY],posX,posY,posX - 1,posY,swapSpeed);
	AddTempBlock(board[posX - 1][posY],posX - 1,posY,posX,posY,swapSpeed);
	board[posX][posY] = board[posX - 1][posY] = -1;
	--posX;
      }
      if(downPressed && posX <= 6 && !(board[posX + 1][posY] == -1 && !boardEmpty[posX + 1][posY])){
	boardEmpty[posX][posY] = boardEmpty[posX + 1][posY] = true;
	AddTempBlock(board[posX][posY],posX,posY,posX + 1,posY,swapSpeed);
	AddTempBlock(board[posX + 1][posY],posX + 1,posY,posX,posY,swapSpeed);
	board[posX][posY] = board[posX + 1][posY] = -1;
	++posX;
      }
      if(leftPressed && posY >= 1 && !(board[posX][posY - 1] == -1 && !boardEmpty[posX][posY - 1])){
	boardEmpty[posX][posY] = boardEmpty[posX][posY - 1] = true;
	AddTempBlock(board[posX][posY],posX,posY,posX,posY - 1,swapSpeed);
	AddTempBlock(board[posX][posY - 1],posX,posY - 1,posX,posY,swapSpeed);
	board[posX][posY] = board[posX][posY - 1] = -1;
	--posY;
      }
      if(rightPressed && posY <= 6 && !(board[posX][posY + 1] == -1 && !boardEmpty[posX][posY + 1])){
	boardEmpty[posX][posY] = boardEmpty[posX][posY + 1] = true;
	AddTempBlock(board[posX][posY],posX,posY,posX,posY + 1,swapSpeed);
	AddTempBlock(board[posX][posY + 1],posX,posY + 1,posX,posY,swapSpeed);
	board[posX][posY] = board[posX][posY + 1] = -1;
	++posY;
      }
    } else if(ctrlPressed){
      if(upPressed){
	CyclicMove(1,3);
	upPressed = false;
      } else if(downPressed){
	CyclicMove(1,1);
	downPressed = false;
      } else if(leftPressed){
	CyclicMove(1,2);
	leftPressed = false;
      } else if(rightPressed){
	CyclicMove(1,0);
	rightPressed = false;
      }
    } else if(shiftPressed){
      if(upPressed){
	CyclicMove(0,2);
	upPressed = false;
      } else if(downPressed){
	CyclicMove(0,0);
	downPressed = false;
      } else if(leftPressed){
	CyclicMove(0,1);
	leftPressed = false;
      } else if(rightPressed){
	CyclicMove(0,3);
	rightPressed = false;
      }
    }
  }

  if(globalState->inQuest){globalState->currQuest->Control(this,tick);}
  return HandleInjections(this,tick);
}

void MachineGameStateMatchGame::OnRender(){
  glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
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
  
  char scoreText[64];
  sprintf(scoreText,"Current Score: %d",currScore);
  renderer.DrawText(scoreText,25,1055,800,50,0,1,0,1);
  HandleRender2DObjects(this);
  renderer.Flush();
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

MachineGameStateMatchGame::MachineGameStateMatchGame(GameGlobalState * const globalState) : swapSpeed(250),elimSpeed(600),upPressed(false),downPressed(false),leftPressed(false),rightPressed(false),escPressed(false),ctrlPressed(false),shiftPressed(false),lockControl(true),canElim(false){
  this -> globalState = globalState;
  globalState->lastState = 2;
  currScore = globalState->playerScore;
  posX = posY = 0;

  for(int i = 0;i < 8;++i){
    for(int j = 0;j < 8;++j){
      board[i][j] = randEngine() % 6;
      boardEmpty[i][j] = false;
    }
  }
  board[0][0] = 6;

  for(int i = 0;i < 128;++i){tempBlockUsed[i] = false;}

  MatchGameBeginAnim * ptr = new MatchGameBeginAnim;
  injections.push_back(ptr);
  renderObjects.push_back(ptr->render);
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

int MachineGameStateMainMenu::OnLogic(int tick){
  titleAnimFrame += tick;
  if(titleAnimFrame >= 5000){
    titleAnimFrame = 0;++titleLineNo;
    if(titleLineNo == 14){titleLineNo = 0;}
  }
  if(downPressed){downPressed = false;++currOption;if(currOption == 2){currOption = 0;}}
  if(upPressed){upPressed = false;--currOption;if(currOption == -1){currOption = 1;}}
  if(enterPressed){
    enterPressed = false;
    if(currOption == 0){return globalState->lastState;}
    if(currOption == 1){quitGame = true;}
  }
  return 0;
}

void MachineGameStateMainMenu::OnRender(){
  glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
  if(currOption == 1){
    renderer.DrawText("Start Game",151,575,800,50,0,1,0,1);
    renderer.DrawText("Exit Game",151,443,800,50,1,0,0,1);
  } else {
    renderer.DrawText("Start Game",151,575,800,50,1,0,0,1);
    renderer.DrawText("Exit Game",151,443,800,50,0,1,0,1);
  }
  renderer.DrawText(titleLines[titleLineNo],879,340 + 0.05 * titleAnimFrame,750,40,std::sin(((double)titleAnimFrame) / 10000 * M_PI),std::cos(((double)titleAnimFrame) /  10000 * M_PI),0.0,std::sin(((double)titleAnimFrame) / 5000 * M_PI) * 0.8 + 0.2);
  
  SDL_GL_SwapWindow(mainWindow);
}

MachineGameStateMainMenu::MachineGameStateMainMenu(GameGlobalState* globalState) : currOption(0),downPressed(false),upPressed(false),enterPressed(false),titleLineNo(0),titleAnimFrame(0),
			     titleLines{"Before you start the game, please read this documentation carefully.",
					"When you enter the game, you are placed inside a large maze. You can press WASD to move around the maze, and press arrow keys to adjust the camera.",
					"Press t to see the list of actions available in a room.",
					"Between each two adjacent rooms there is a locked gate, which can only be opened by solving a puzzle.",
					"We intended to create something similar to Puzzle Quest.",
					"However we decided to add another twist to the match 3 mechanism.",
					"At the beginning of each puzzle level there is a gray ball at the top left corner of the board.",
					"You can only use arrow keys to swap this gray ball with its neighbors.",
				        "You will soon discover, this is often a very inefficient way to create matches.",
					"To compensate this we introduced another kind of moves, the cyclic swaps.",
					"To do a clockwise cyclic swap, press Ctrl + arrow key. To do a counterclockwise cyclic swap, press Shift + arrow key.",
			     		"Explore the patterns of cyclic swap. They will save you a lot of time.",
					"This game is written by Charlie and Gilbert in 3 days. We are inspired by Puzzle Quest (for its match-3 puzzle), NetHack (for its procedural level generation), and Daedalus 3.3 (for its swiss army knife of maze algorithms).",
					"This program is licensed under GNU GPL3. All assets come from free-licensed sources. All dependencies are open source libraries."
}
{this->globalState = globalState;}

void MachineGameStateWorldmap::OnKeyDown(const SDL_KeyboardEvent &ev){
  if(ev.keysym.sym == SDLK_ESCAPE){
    escPressed = true;
  }
  if(ev.keysym.sym == SDLK_RETURN){
    enterPressed = true;
  }
  if(ev.keysym.sym == SDLK_LEFT){
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
  } else if(ev.keysym.sym == SDLK_t){
    tPressed = true;
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
  } else if(ev.keysym.sym == SDLK_RETURN){
    enterPressed = false;
  } else if(ev.keysym.sym == SDLK_t){
    tPressed = false;
  }
}

bool MachineGameStateWorldmap::AttemptMove(float newPosX,float newPosY){
  const float testPoints[8][2] = {
			    {0.7,0},
			    {-0.7,0},
			    {0,0.7},
			    {0,-0.7},
			    {0.7,0.7},
			    {0.7,-0.7},
			    {-0.7,0.7},
			    {-0.7,-0.7}
  };

  for(int i = 0;i < 8;++i){
    int x = std::round(newPosX + testPoints[i][0]) + 36;
    int y = std::round(newPosY + testPoints[i][1]) + 36;
    if(x < 0 || x >= 72){return false;}
    if(y < 0 || y >= 72){return false;}
    if(globalState->isWall[x][y]){return false;}
  }
  return true;
}

int MachineGameStateWorldmap::OnLogic(int tick){
  globalState->fposX = fposX;
  globalState->fposY = fposY;
  globalState->xyRot = xyRot;
  globalState->yawn = yawn;
  
  if(escPressed){return STATE_MAINMENU;}

  if(!lockControl){
    if(leftPressed){xyRot += tick * M_PI / 1000;}
    if(rightPressed){xyRot -= tick * M_PI / 1000;}
    if(upPressed){yawn += tick * M_PI / 1200;}
    if(downPressed){yawn -= tick * M_PI / 1200;}
    float dirX = -cos(xyRot) * tick / 150,dirY = -sin(xyRot) * tick / 150;
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
  }
  
  if(xyRot >= M_PI){xyRot -= M_PI * 2;}
  if(xyRot <= -M_PI){xyRot += M_PI * 2;}
  if(yawn >= M_PI / 2){yawn = M_PI / 2 - 0.001;}
  if(yawn < M_PI / 18){yawn = M_PI / 18;}
  
  return HandleInjections(this,tick);
}

void MachineGameStateWorldmap::OnRender(){
  glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  renderer.UseSpriteAtlas(globalState -> spriteAtlasID);
  renderer.Begin3D();
  renderer.cameraMat = glm::lookAt(glm::vec3(fposY,0.5 * sin(yawn) + 0.5,fposX),glm::vec3(fposY - 0.5 * std::sin(xyRot) * std::cos(yawn),0.5,fposX - 0.5 * std::cos(xyRot) * std::cos(yawn)),glm::vec3(0,1,0));
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
  
  HandleRender2DObjects(this);
  renderer.Flush();
  
  SDL_GL_SwapWindow(mainWindow);
  return;
}

MachineGameStateWorldmap::MachineGameStateWorldmap(GameGlobalState * const globalState) : escPressed(false), leftPressed(false), rightPressed(false), upPressed(false), downPressed(false), aPressed(false), sPressed(false), dPressed(false), wPressed(false), enterPressed(false), tPressed(false), lockControl(false)
{
  this -> globalState = globalState;
  globalState->lastState = 3;
  fposX = globalState->fposX;
  fposY = globalState->fposY;
  posX = std::round(fposX);
  posY = std::round(fposY);
  xyRot = globalState->xyRot;
  yawn = globalState->yawn;
  renderer.projMat = glm::perspective(M_PI * 0.278,1.778,0.5,20.0);
  
  for(int i = 0;i < 73;++i){
    for(int j = 0;j < 73;++j){
      if(!globalState->isWall[i][j]){
	Square * sqr = new Square(-36 + j,0,-36 + i);
	sqr->texture = globalState->maze[i][j];
	sqr->SetColor();
	worldObjects[i * 73 + j] = sqr;
      } else {
	Cube * cbe = new Cube(-36 + j,0.5,-36 + i);
	for(int k = 0;k < 6;++k){
	  cbe->sideTexture[k] = globalState->maze[i][j];
	}
	cbe->SetColor();
	worldObjects[i * 73 + j] = cbe;
      }
    }
  }

  auto inj = new WorldmapQuestBox;
  injections.push_back(inj);
  renderObjects.push_back(inj->render);
  return;
}

MachineGameStateWorldmap::~MachineGameStateWorldmap(){
  for(int i = 0;i < 5329;++i){
    delete worldObjects[i];
  }
}
