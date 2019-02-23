#ifndef PZQGAME_QUEST_H
#define PZQGAME_QUEST_H

#include "MachineInjection.h"

//struct MachineGameStateBase;
//struct MachineGameStateWorldmap;

struct Quest : MachineInjection{
  virtual int Control(MachineGameStateWorldmap *ptr,int tick){return 0;}
};

struct FirstQuest : Quest{
  int Control(MachineGameStateWorldmap *ptr,int tick);
  FirstQuest();

  int stage;
  int animFrame;
};

#endif
