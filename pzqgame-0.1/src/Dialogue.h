#ifndef PZQGAME_DIALOGUE_H
#define PZQGAME_DIALOGUE_H

struct Dialogue{
  virtual Dialogue * NextTurn(){return this;}
  const char * currLine;
};

struct TutorialDialogue : Dialogue{
  TutorialDialogue();
  Dialogue * NextTurn();
  
  const char * dialogueLines[5];
  int currLineNo;
};

#endif
