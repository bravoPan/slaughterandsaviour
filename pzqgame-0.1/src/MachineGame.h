#ifndef PZQGAME_MACHINEGAME_H
#define PZQGAME_MACHINEGAME_H

#include <vector>
#include "GameGlobalState.h"
#include "GeomModel.h"
#include "MachineInjection.h"
#include "Dialogue.h"

struct Quest;

struct MachineGameStateBase{
  std::vector<MachineInjection *> injections;
  std::vector<Renderable *> renderObjects;
  virtual int HandleInjections(int tick){}
  virtual void HandleRenderObjects(){}
  virtual void HandleRender2DObjects(){}
  virtual void OnKeyDown(const SDL_KeyboardEvent &ev){}
  virtual void OnKeyUp(const SDL_KeyboardEvent &ev){}
  virtual int OnLogic(int tick){return 0;}
  virtual void OnRender(){}
};

enum StateType{
	       STATE_MAINMENU = 1,
	       STATE_MATCHGAME = 2,
	       STATE_WORLDMAP = 3
};

struct MachineGameStateMatchGame : MachineGameStateBase{
  MachineGameStateMatchGame(GameGlobalState * const globalState);
  
  void OnKeyDown(const SDL_KeyboardEvent &ev);
  void OnKeyUp(const SDL_KeyboardEvent &ev);
  int OnLogic(int tick);
  void OnRender();
  int HandleInjections(int tick);
  void HandleRenderObjects();
  void HandleRender2DObjects();
  void AddTempBlock(int blockID,int beginPosX,int beginPosY,int endPosX,int endPosY,int totalFrame);
  
  GameGlobalState * globalState;
  
  bool upPressed,downPressed,leftPressed,rightPressed,escPressed;
  bool lockControl,canElim;
  int posX,posY;
  int board[8][8];
  bool boardEmpty[8][8];

  struct TemporaryBlock{
    int blockID,posX,posY,destPosX,destPosY,animFrame,totalFrame;
    bool replBlock;
  } tempBlocks[128];
  bool tempBlockUsed[128];

  const int swapSpeed,elimSpeed;
  int currScore;
};

struct MachineGameStateWorldmap : MachineGameStateBase{
  MachineGameStateWorldmap(GameGlobalState* globalState);
  ~MachineGameStateWorldmap();
  
  void OnKeyDown(const SDL_KeyboardEvent &ev);
  void OnKeyUp(const SDL_KeyboardEvent &ev);
  int OnLogic(int tick);
  void OnRender();
  int HandleInjections(int tick);
  void HandleRenderObjects();
  void HandleRender2DObjects();
  bool AttemptMove(float newPosX,float newPosY);

  GameGlobalState* globalState;
  
  enum WorldmapInnerState{
			  INNERSTATE_NORMAL,
			  INNERSTATE_DIALOGUE,
			  INNERSTATE_TRANS_TO_MATCH_ANIM
  } currInnerState;
  bool escPressed,upPressed,downPressed,leftPressed,rightPressed,enterPressed,tPressed;
  bool aPressed,sPressed,dPressed,wPressed;
  double xyRot,yawn;
  int posX,posY;
  float fposX,fposY;
  GeomModel * worldObjects[5329];

  //For INNERSTATE_DIALOGUE
  Dialogue * currDialogue;
  bool GUIModified;

  //For INNERSTATE_TRANS_TO_MATCH_ANIM
  int animFrame;

  //For quests
  bool inQuest;
  Quest * currQuest;
};

struct MachineGameStateMainMenu : MachineGameStateBase{
  MachineGameStateMainMenu(GameGlobalState* globalState);
  void OnKeyDown(const SDL_KeyboardEvent &ev);
  int OnLogic(int tick);
  void OnRender();
  int HandleInjections(int tick);
  void HandleRenderObjects();
  void HandleRender2DObjects();

  GameGlobalState * globalState;
  bool downPressed,upPressed,enterPressed;
  int titleAnimFrame;
  int titleLineNo;
  int currOption;
  const char *titleLines[7];
};

struct MachineGame{
  void Initiate();
  void RunGameLoop();

  MachineGameStateBase * currState;
  GameGlobalState globalState;

  std::chrono::time_point<std::chrono::steady_clock> currTime,prevTime;
};

#endif
