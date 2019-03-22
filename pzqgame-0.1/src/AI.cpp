#include "common.h"
#include "AI.h"
#include "GlobalObjects.h"

void NeuNetwork::ReadData(){
  return;
}

void make_move(int simboard[5][5],int move,int *x,int *y){
  int t;
  if(move == 0){
    if(*x > 0){
      t = simboard[*x][*y];
      simboard[*x][*y] = simboard[*x - 1][*y];
      simboard[*x - 1][*y] = t;
      --(*x);
    }
    return;
  }
  if(move == 1){
    if(*x < 4){
      t = simboard[*x][*y];
      simboard[*x][*y] = simboard[*x + 1][*y];
      simboard[*x + 1][*y] = t;
      ++(*x);
    }
    return;
  }
  if(move == 2){
    if(*y > 0){
      t = simboard[*x][*y];
      simboard[*x][*y] = simboard[*x][*y - 1];
      simboard[*x][*y - 1] = t;
      --(*y);
    }
    return;
  }
  if(move == 3){
    if(*y < 4){
      t = simboard[*x][*y];
      simboard[*x][*y] = simboard[*x][*y + 1];
      simboard[*x][*y + 1] = t;
      ++(*y);
    }
    return;
  }
  if(move < 8){
    const int clockwise_next[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
    int j = move - 4;
    int p = *x,q = *y;
    bool canMove = true;
    for(int i = 0;i < 4;++i){
      if(p < 0 || p > 4){canMove = false;break;}
      if(q < 0 || q > 4){canMove = false;break;}
      p += clockwise_next[j][0];
      q += clockwise_next[j][1];
      j += 1;
      if(j == 4){j = 0;}
    }
    if(canMove){
      t = simboard[p][q];
      for(int i = 0;i < 3;++i){
	simboard[p][q] = simboard[p + clockwise_next[j][0]][q + clockwise_next[j][1]];
	p += clockwise_next[j][0];
	q += clockwise_next[j][1];
	j += 1;
	if(j == 4){j = 0;}
      }
      simboard[p][q] = t;
    }
  } else if(move < 12){
    const int counterclockwise_next[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};
    int j = move - 8;
    int p = *x,q = *y;
    bool canMove = true;
    for(int i = 0;i < 4;++i){
      if(p < 0 || p > 4){canMove = false;break;}
      if(q < 0 || q > 4){canMove = false;break;}
      p += counterclockwise_next[j][0];
      q += counterclockwise_next[j][1];
      j += 1;
      if(j == 4){j = 0;}
    }
    if(canMove){
      t = simboard[p][q];
      for(int i = 0;i < 3;++i){
	simboard[p][q] = simboard[p + counterclockwise_next[j][0]][q + counterclockwise_next[j][1]];
	p += counterclockwise_next[j][0];
	q += counterclockwise_next[j][1];
	j += 1;
	if(j == 4){j = 0;}
      }
      simboard[p][q] = t;
    }
  }
  for(int i = 0;i < 5;++i){
    for(int j = 0;j < 5;++j){
      if(simboard[i][j] == 2){
	*x = i;*y = j;
	return;
      }
    }
  }
}

int elim_board(int simboard[5][5]){
  bool eliminable[5][5] = {0};
  int score = 0;
  for(int i = 0;i < 5;++i){
    for(int j = 0;j < 5;++j){
      if(j <= 2){
	if(simboard[i][j] == simboard[i][j + 1] && simboard[i][j] == simboard[i][j + 2]){
	  eliminable[i][j] = true;
	  eliminable[i][j + 1] = true;
	  eliminable[i][j + 2] = true;
	}
      }
      if(i <= 2){
	if(simboard[i][j] == simboard[i + 1][j] && simboard[i][j] == simboard[i + 2][j]){
	  eliminable[i][j] = true;
	  eliminable[i + 1][j] = true;
	  eliminable[i + 2][j] = true;
	}
      }
    }
  }
  for(int i = 0;i < 5;++i){
    for(int j = 0;j < 5;++j){
      if(eliminable[i][j] && simboard[i][j] != 0){
	simboard[i][j] = 0;
	score += 20;
      }
    }
  }
  return score;
}

int NeuNetwork::Choose(int board[8][8]){
  static int lastmove = -1;
  int x = -1,y = -1;
  for(int i = 0;i < 8;++i){
    for(int j = 0;j < 8;++j){
      if(board[i][j] == 6){
	x = i;y = j;break;
      }
    }
    if(x >= 0){break;}
  }

  bool feasible[12];
  for(int i = 0;i < 12;++i){feasible[i] = true;}
  if(x == 0){feasible[0] = false;feasible[5] = false;feasible[6] = false;feasible[10] = false;feasible[11] = false;}
  if(x == 7){feasible[1] = false;feasible[4] = false;feasible[7] = false;feasible[8] = false;feasible[9] = false;}
  if(y == 0){feasible[2] = false;feasible[6] = false;feasible[7] = false;feasible[9] = false;feasible[10] = false;}
  if(y == 7){feasible[3] = false;feasible[4] = false;feasible[5] = false;feasible[8] = false;feasible[11] = false;}

  if(lastmove == 0){feasible[1] = false;}
  if(lastmove == 1){feasible[0] = false;}
  if(lastmove == 2){feasible[3] = false;}
  if(lastmove == 3){feasible[2] = false;}
  if(lastmove == 4){feasible[9] = false;}
  if(lastmove == 5){feasible[8] = false;}
  if(lastmove == 6){feasible[11] = false;}
  if(lastmove == 7){feasible[10] = false;}
  if(lastmove == 8){feasible[5] = false;}
  if(lastmove == 9){feasible[4] = false;}
  if(lastmove == 10){feasible[7] = false;}
  if(lastmove == 11){feasible[6] = false;}

  int simboard[5][5],simboardcpy[5][5];
  int decision = -1;
  int bestq = 0;

  for(int c = 0;c <= 5;++c){
    for(int i = 0;i < 25;++i){simboard[i / 5][i % 5] = 0;}
    for(int i = -2;i <= 2;++i){
      if(x + i < 0 || x + i >= 8){continue;}
      for(int j = -2;j <= 2;++j){
	if(y + j < 0 || y + j >= 8){continue;}
	if(board[(x + i)][(y + j)] == c){
	  simboard[i + 2][j + 2] = 1;
	}
      }
      simboard[2][2] = 2;
    }
    for(int i = 0;i < 50;++i){
      int mov = randEngine() % 144;
      int mova = mov / 12,movb = mov % 12;
      if(!feasible[mova]){continue;}
      for(int j = 0;j < 25;++j){
	simboardcpy[j / 5][j % 5] = simboard[j / 5][j % 5];
      }
      int bx = 2,by = 2;
      int score = 0;
      make_move(simboardcpy,mova,&bx,&by);
      score += elim_board(simboardcpy);
      make_move(simboardcpy,movb,&bx,&by);
      score += elim_board(simboardcpy);
      if(score > bestq){
	bestq = score;
	decision = mova;
      }
    }
  }

  if(bestq == 0){
    decision = randEngine() % 4;
    if(!feasible[decision]){
      for(int i = 0;i < 4;++i){
	if(feasible[i]){decision = i;}
      }
    }
  }

  lastmove = decision;
  return decision;
}
