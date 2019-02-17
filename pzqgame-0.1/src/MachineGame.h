#ifndef PZQGAME_MACHINEGAME_H
#define PZQGAME_MACHINEGAME_H

#include "common.h"
#include "GameGlobalState.h"
#include "GeomModel.h"

struct MachineGameStateBase{
  virtual void OnKeyDown(const SDL_KeyboardEvent &ev){return;}
  virtual void OnKeyUp(const SDL_KeyboardEvent &ev){return;}
  virtual int OnLogic(){return 0;}
  virtual void OnRender(){return;}
};

enum StateType{
	       STATE_MAINMENU = 1,
	       STATE_MATCHGAME = 2,
	       STATE_WORLDMAP = 3
};

struct MachineGameStateMatchGame : MachineGameStateBase{
  enum MatchGameInnerState{
			  INNERSTATE_NORMAL,
			  INNERSTATE_BEGIN,
			  INNERSTATE_MENU
  };

  MatchGameInnerState currInnerState;
  //int animFrame;
  bool upPressed,downPressed,leftPressed,rightPressed,escPressed;
  
  void OnKeyDown(const SDL_KeyboardEvent &ev);
  void OnKeyUp(const SDL_KeyboardEvent &ev);
  int OnLogic();
  void OnRender();
  void AddTempBlock(int blockID,int beginPosX,int beginPosY,int endPosX,int endPosY,int totalFrame);
  
  GameGlobalState * globalState;
  bool GUImodified;
  int posX,posY;
  int newPosX,newPosY;
  
  MachineGameStateMatchGame(GameGlobalState * const globalState);

  int board[8][8];
  bool boardEmpty[8][8];

  struct TemporaryBlock{
    int blockID,posX,posY,destPosX,destPosY,animFrame,totalFrame;
    bool replBlock;
  } tempBlocks[128];
  bool tempBlockUsed[128];

  const int swapSpeed,elimSpeed;
  int currScore;

  int beginAnimFrame;
};

struct MachineGameStateWorldmap : MachineGameStateBase{
  void OnKeyDown(const SDL_KeyboardEvent &ev);
  void OnKeyUp(const SDL_KeyboardEvent &ev);
  int OnLogic();
  void OnRender();
  bool AttemptMove(float newPosX,float newPosY);
  MachineGameStateWorldmap(GameGlobalState* globalState);
  ~MachineGameStateWorldmap();

  bool escPressed,upPressed,downPressed,leftPressed,rightPressed;
  bool aPressed,sPressed,dPressed,wPressed;
  int maze[73][73];
  double xyRot,yawn;
  int posX,posY;
  float fposX,fposY;
  GeomModel * worldObjects[5329];
  GameGlobalState* globalState;
};

struct MachineGameStateMainMenu : MachineGameStateBase{
  void OnKeyDown(const SDL_KeyboardEvent &ev);
  int OnLogic();
  void OnRender();
  MachineGameStateMainMenu(GameGlobalState* globalState);

  bool downPressed,upPressed,enterPressed;
  int titleAnimFrame;
  int titleLineNo;
  int currOption;
  const char *titleLines[7];
};

struct MachineGame{
  GameGlobalState globalState;
  void RunGameLoop();

  MachineGameStateBase * currState;

  std::chrono::time_point<std::chrono::steady_clock> currTime,prevTime;

  void Initiate();
};

#endif
