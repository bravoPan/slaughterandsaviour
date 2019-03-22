#ifndef PZQGAME_GAMEGLOBALSTATE_H
#define PZQGAME_GAMEGLOBALSTATE_H

#include "AI.h"

struct Quest;
struct QuestList;

struct GameGlobalState{
  char * playerName;
  int playerScore;
  int spriteAtlasID;
  int lastState;

  //For Worldmap
  int maze[73][73];
  bool isWall[73][73];
  float fposX,fposY;
  double xyRot,yawn;

  //For Matchgame
  NeuNetwork * neun;
  int goalScore;

  //For quest
  bool inQuest;
  Quest * currQuest;
  QuestList * questList[12][12];
};

#endif
