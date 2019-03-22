#ifndef PZQGAME_AI_H
#define PZQGAME_AI_H

struct NeuNetwork{
  float dense1[50][128];
  float dense2[128][128];
  float dense3[128][12];
  float dense1bias[128];
  float dense2bias[128];
  float dense3bias[12];

  void ReadData();
  int Choose(int board[8][8]);
};

#endif
