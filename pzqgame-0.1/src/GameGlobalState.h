#ifndef PZQGAME_GAMEGLOBALSTATE_H
#define PZQGAME_GAMEGLOBALSTATE_H

struct Quest;

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
  int goalScore;

  //For quest
  bool inQuest;
  Quest * currQuest;
};

#endif
