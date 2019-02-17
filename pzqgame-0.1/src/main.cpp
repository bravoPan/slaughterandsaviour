#include "common.h"
#include "GlobalObjects.h"
#include "MachineGame.h"

int main(int argc,char *argv[]){
  if(!GlobalInitialize()){
    return 1;
  }

  //MachineGame machine;
  machine.Initiate();
  machine.RunGameLoop();

  GlobalFinalize();
  return 0;
}
