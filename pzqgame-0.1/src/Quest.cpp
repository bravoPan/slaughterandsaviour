#include "common.h"
#include "Quest.h"
#include "MachineGame.h"

FirstQuest::FirstQuest() : stage(0) {}

int FirstQuest::Control(MachineGameStateWorldmap *ptr,int tick){
  if(ptr->currInnerState == MachineGameStateWorldmap::INNERSTATE_NORMAL && ptr->tPressed && stage == 0){
    ptr->tPressed = false;
    ptr->currInnerState = MachineGameStateWorldmap::INNERSTATE_TRANS_TO_MATCH_ANIM;
    ptr->animFrame = 0;
    ptr->globalState->goalScore = 1000;
    ++stage;
    return 0;
  }

  if(ptr->currInnerState == MachineGameStateWorldmap::INNERSTATE_NORMAL && stage == 1){
    ptr->fposX = -33.0;
    ptr->fposY = -33.0;
    ptr->posX = -33;
    ptr->posY = -33;
    ptr->xyRot = 3 * M_PI / 2;
    ptr->yawn = M_PI / 18;
    ++stage;
    animFrame = 0;
    return 0;
  }

  if(ptr->currInnerState == MachineGameStateWorldmap::INNERSTATE_NORMAL && stage == 2){
    animFrame += tick;
    if(animFrame >= 1000){
      delete this;
      ptr->inQuest = false;
      ptr->currQuest = NULL;
      ptr->globalState->isWall[2][6] = false;
      ptr->globalState->isWall[3][6] = false;
      ptr->globalState->isWall[4][6] = false;
      return 0;
    }
    ptr->worldObjects[73 * 2 + 6]->SetPosition(-30,0.5 - animFrame * 0.001,-34);
    ptr->worldObjects[73 * 3 + 6]->SetPosition(-30,0.5 - animFrame * 0.001,-33);
    ptr->worldObjects[73 * 4 + 6]->SetPosition(-30,0.5 - animFrame * 0.001,-32);
    return 0;
  }
}
