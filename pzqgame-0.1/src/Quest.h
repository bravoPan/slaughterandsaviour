#ifndef PZQGAME_QUEST_H
#define PZQGAME_QUEST_H

#include "MachineInjection.h"

struct Quest{
  virtual int Control(MachineGameStateWorldmap *ptr,int tick){return 0;}
  virtual int Control(MachineGameStateMatchGame *ptr,int tick){return 0;}
  virtual int Control(MachineGameStateMainMenu *ptr,int tick){return 0;}
  int stage;
  char title[50];
  Quest() : stage(0),title{0}{}
};

struct QuestList{
  Quest * qst;
  QuestList *prev,*next;
};

/*
struct FirstQuest : Quest{
  int Control(MachineGameStateWorldmap *ptr,int tick);
  int Control(MachineGameStateMatchGame *ptr,int tick);

  int animFrame;
};
*/

struct DestroyGate : Quest{
  int Control(MachineGameStateWorldmap *ptr,int tick);
  int Control(MachineGameStateMatchGame *ptr,int tick);
  int Control(MachineGameStateMainMenu *ptr,int tick);

  int gate[3];
  float gateDir;
  int animFrame;
  int animPosX,animPosY;
  int goalScore;

  DestroyGate(int x,int y,int dir,int goal);
};

#endif
