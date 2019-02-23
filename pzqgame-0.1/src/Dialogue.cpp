#include "common.h"
#include "Dialogue.h"

TutorialDialogue::TutorialDialogue() :
  dialogueLines{"This is a dialogue test",
		"This is another dialogue test",
		"This is the third dialogue test",
		"This is not the last dialogue test",
		"This is the last dialogue test"
}, currLineNo(0)
{
  currLine = dialogueLines[0];
}

Dialogue * TutorialDialogue::NextTurn(){
  ++currLineNo;
  if(currLineNo == 5){
    delete this;
    return NULL;
  } else {
    currLine = dialogueLines[currLineNo];
    return this;
  }
}
