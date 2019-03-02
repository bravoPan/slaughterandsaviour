#include "common.h"
#include "GlobalObjects.h"
#include "MachineGame.h"

int main(int argc,char *argv[]){
  if(!GlobalInitialize()){
    return 1;
  }

  try{
    //MachineGame machine;
    machine.Initiate();
    machine.RunGameLoop();
  } catch(std::exception &e){
    std::cout << e.what() << std::endl;
  }

  GlobalFinalize();
  return 0;
}
