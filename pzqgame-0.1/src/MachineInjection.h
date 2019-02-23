#ifndef PZQGAME_MACHINE_INJECTION_H
#define PZQGAME_MACHINE_INJECTION_H

struct MachineGameStateBase;
struct MachineGameStateMainMenu;
struct MachineGameStateWorldmap;
struct MachineGameStateMatchGame;

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
  virtual void Render2D(cairo_t *cr,PangoLayout *textLayout){}
};

struct MatchGameBeginAnim : MachineInjection{
  int animFrame;
  struct Render : Renderable {
    MatchGameBeginAnim *controller;
    Render(MatchGameBeginAnim *ptr) : controller(ptr){}
    void Render2D(cairo_t *cr,PangoLayout *textLayout);
  } *render;
  
  MatchGameBeginAnim() : animFrame(0),render(new Render(this)){}
  int Control(MachineGameStateMatchGame *ptr,int tick);
};

struct MatchGameVictoryAnim : MachineInjection{
  int animFrame;
  struct Render : Renderable {
    MatchGameVictoryAnim *controller;
    Render(MatchGameVictoryAnim *ptr) : controller(ptr){}
    void Render2D(cairo_t *cr,PangoLayout *textLayout);
  } *render;
  
  MatchGameVictoryAnim() : animFrame(-1),render(new Render(this)){}
  int Control(MachineGameStateMatchGame *ptr,int tick);
};

#endif
