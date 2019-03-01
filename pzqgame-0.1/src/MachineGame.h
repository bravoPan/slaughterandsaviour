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
  void DestroyInjections();
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

  GameGlobalState * globalState;
  bool downPressed,upPressed,enterPressed;
  int titleAnimFrame;
  int titleLineNo;
  int currOption;
  const char *titleLines[7];
};

struct MachineGame{
  void Initiate();
  void MazeGen();
  void RunGameLoop();

  MachineGameStateBase * currState;
  GameGlobalState globalState;

  std::chrono::time_point<std::chrono::steady_clock> currTime,prevTime;
};

template<class T> int HandleInjections(T * state,int tick){
  auto iter = state->injections.begin();
  while(iter != state->injections.end()){
    if((*iter)->finished){
      delete (*iter);
      iter = state->injections.erase(iter);
      continue;
    }
    if((*iter)->active){
      int t = (*iter)->Control(state,tick);
      if(t != 0){return t;}
    }
    ++iter;
  }
  return 0;
}

template<class T> void HandleRenderObjects(T * state){
  auto iter = state->renderObjects.begin();
  while(iter != state->renderObjects.end()){
    if((*iter)->finished){
      delete (*iter);
      iter = state->renderObjects.erase(iter);
      continue;
    }
    if((*iter)->active){
      (*iter)->Render3D();
    }
    ++iter;
  }
}

template<class T> void HandleRender2DObjects(T * state,cairo_t *cr,PangoLayout *textLayout){
  auto iter = state->renderObjects.begin();
  while(iter != state->renderObjects.end()){
    if((*iter)->finished){
      delete (*iter);
      iter = state->renderObjects.erase(iter);
      continue;
    }
    if((*iter)->active){
      (*iter)->Render2D(cr,textLayout);
    }
    ++iter;
  }
}

#endif
