#ifndef PZQGAME_MACHINE_INJECTION_H
#define PZQGAME_MACHINE_INJECTION_H

struct MachineGameStateBase;
struct MachineGameStateMainMenu;
struct MachineGameStateWorldmap;
struct MachineGameStateMatchGame;
struct Dialogue;
struct Quest;
struct QuestList;

struct MachineInjection{
  bool active;
  bool finished;
  MachineInjection() : active(true), finished(false){}
  virtual int Control(MachineGameStateBase *ptr,int tick){return 0;}
  virtual int Control(MachineGameStateMainMenu *ptr,int tick){return 0;}
  virtual int Control(MachineGameStateWorldmap *ptr,int tick){return 0;}
  virtual int Control(MachineGameStateMatchGame *ptr,int tick){return 0;}
};

struct Renderable{
  bool active;
  bool finished;
  Renderable() : active(true), finished(false){}
  virtual void Render3D(){}
  virtual void Render2D(){}
};

struct MatchGameBeginAnim : MachineInjection{
  int animFrame;
  struct Render : Renderable {
    MatchGameBeginAnim *controller;
    Render(MatchGameBeginAnim *ptr) : controller(ptr){}
    void Render2D();
  } *render;
  
  MatchGameBeginAnim() : animFrame(0),render(new Render(this)){}
  int Control(MachineGameStateMatchGame *ptr,int tick);
};

struct MatchGameVictoryAnim : MachineInjection{
  int animFrame;
  struct Render : Renderable {
    MatchGameVictoryAnim *controller;
    Render(MatchGameVictoryAnim *ptr) : controller(ptr){}
    void Render2D();
  } *render;
  
  MatchGameVictoryAnim() : animFrame(-1),render(new Render(this)){}
  int Control(MachineGameStateMatchGame *ptr,int tick);
};

/*
struct WorldmapDialogueBox : MachineInjection{
  struct Render : Renderable {
    WorldmapDialogueBox *controller;
    Render(WorldmapDialogueBox *ptr) : controller(ptr){}
    void Render2D(cairo_t *cr,PangoLayout *textLayout);
  } *render;

  Dialogue * diag;

  WorldmapDialogueBox(Dialogue * input) : render(new Render(this)),diag(input){}
  int Control(MachineGameStateWorldmap *ptr,int tick);
};
*/

struct WorldmapTransToMatchAnim : MachineInjection{
  struct Render : Renderable {
    WorldmapTransToMatchAnim *controller;
    Render(WorldmapTransToMatchAnim *ptr) : controller(ptr){}
    void Render2D();
  } *render;

  int animFrame;

  WorldmapTransToMatchAnim() : render(new Render(this)),animFrame(0){}
  int Control(MachineGameStateWorldmap *ptr,int tick);
};

struct WorldmapQuestBox : MachineInjection{
  struct Render : Renderable {
    WorldmapQuestBox *controller;
    Render(WorldmapQuestBox *ptr) : controller(ptr){}
    void Render2D();
  } *render;
  
  int Control(MachineGameStateWorldmap *ptr,int tick);

  bool displayQuestList;
  bool inRoom;
  QuestList * displayedQuest[5];

 WorldmapQuestBox() : displayQuestList(false),render(new Render(this)),displayedQuest{NULL,NULL,NULL,NULL,NULL},inRoom(false){}
};

#endif
