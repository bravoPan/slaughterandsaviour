#ifndef PZQGAME_MACHINEGAME_H
#define PZQGAME_MACHINEGAME_H

#include "common.h"
#include "GameGlobalState.h"

struct MachineGameStateBase{
  virtual void OnKeyDown(const SDL_KeyboardEvent &ev){return;}
  virtual void OnKeyUp(const SDL_KeyboardEvent &ev){return;}
  virtual void OnLogic(){return;}
  virtual void OnRender(){return;}
};

struct MachineGameStateMainmenu : MachineGameStateBase{
  enum MainmenuInnerState{
			  INNERSTATE_NORMAL,
			  INNERSTATE_ANIM
  };

  MainmenuInnerState currInnerState;
  //int animFrame;
  bool upPressed,downPressed,leftPressed,rightPressed;
  
  void OnKeyDown(const SDL_KeyboardEvent &ev);
  void OnKeyUp(const SDL_KeyboardEvent &ev);
  void OnLogic();
  void OnRender();
  void AddTempBlock(int blockID,int beginPosX,int beginPosY,int endPosX,int endPosY,int totalFrame);
  GameGlobalState * globalState;
  bool GUImodified;
  int posX,posY;
  int newPosX,newPosY;
  MachineGameStateMainmenu(GameGlobalState * const globalState);

  int board[8][8];
  bool inAnim[8][8];
  int animFrame[8][8];
  int totalFrame[8][8];
  int dest[8][8][2];
  int repl[8][8];
  bool falling[8][8];

  struct TemporaryBlock{
    int blockID,posX,posY,destPosX,destPosY,animFrame,totalFrame;
  } tempBlocks[128];
  bool tempBlockUsed[128];

  const int swapSpeed,elimSpeed;
};

struct MachineGame{
  GameGlobalState globalState;
  void RunGameLoop();

  MachineGameStateBase * currState;

  std::chrono::time_point<std::chrono::steady_clock> currTime,prevTime;

  MachineGame();
};

#endif
