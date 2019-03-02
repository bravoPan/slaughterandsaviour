#include "common.h"
#include "Quest.h"
#include "MachineInjection.h"
#include "MachineGame.h"

DestroyGate::DestroyGate(int x,int y,int dir,int goal){
  static const char * const dirName[4] = {"North","South","West","East"};
  static const float dirAngle[4] = {0,M_PI,M_PI / 2,3 * M_PI / 2};
  static const int dirDisp[4][2] = {{3,0},{-3,0},{0,3},{0,-3}};
  std::sprintf(title,"Attack %s Gate",dirName[dir]);
  gateDir = dirAngle[dir];
  goalScore = goal;
  
  animPosX = x * 3 + dirDisp[dir][0];
  animPosY = y * 3 + dirDisp[dir][1];

  for(int i = 0;i < 3;++i){
    int u,v;
    if(x % 2 == 1){
      u = x * 3 + i - 1;
      v = y * 3;
    } else {
      u = x * 3;
      v = y * 3 + i - 1;
    }
    gate[i] = u * 73 + v;
  }
}

int DestroyGate::Control(MachineGameStateMatchGame *ptr,int tick){
  if(ptr->currScore >= ptr->globalState->goalScore && stage == 1){
    ++stage;
  }
  return 0;
}

int DestroyGate::Control(MachineGameStateWorldmap *ptr,int tick){
  if(stage == 0){
    auto ptr2 = new WorldmapTransToMatchAnim;
    ptr->backupInjections.push_back(ptr2);
    ptr->backupRenderObjects.push_back(ptr2->render);
    if(goalScore > 0){
      ptr->globalState->goalScore = goalScore;
    } else {
      ptr->globalState->goalScore = ptr->globalState->playerScore + 2000;
    }
    ++stage;
    return 0;
  }

  if(stage == 2){
    ptr->fposX = animPosX - 36;
    ptr->fposY = animPosY - 36;
    ptr->posX = animPosX - 36;
    ptr->posY = animPosY - 36;
    ptr->xyRot = gateDir;
    ptr->yawn = M_PI / 18;
    ptr->lockControl = true;
    ++stage;
    animFrame = 0;
    return 0;
  }

  if(stage == 3){
    animFrame += tick;
    if(animFrame >= 1000){
      animFrame = 1000;
    }

    for(int i = 0;i < 3;++i){
      int u = gate[i] / 73 - 36;
      int v = gate[i] % 73 - 36;
      ptr->worldObjects[gate[i]]->SetPosition(v,0.5 - animFrame * 0.001,u);
    }

    if(animFrame == 1000){
      for(int i = 0;i < 3;++i){
	ptr->globalState->isWall[gate[i] / 73][gate[i] % 73] = false;
      }
      ptr->lockControl = false;
      return -1;
    }
    return 0;
  }
  
  return 0;
}
