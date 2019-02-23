#include "common.h"
#include "MachineInjection.h"
#include "MachineGame.h"
#include "GlobalObjects.h"

int MatchGameBeginAnim::Control(MachineGameStateMatchGame *ptr,int tick){
  animFrame += tick;
  if(animFrame >= 3000){
    ptr->canElim = true;
    ptr->lockControl = false;
    finished = true;
    render->finished = true;
  }
  return 0;
}

void MatchGameBeginAnim::Render::Render2D(cairo_t *cr,PangoLayout *textLayout){
  renderer.DrawQuad(723,1571,116,964,13 + controller->animFrame / 1000);
}

int MatchGameVictoryAnim::Control(MachineGameStateMatchGame *ptr,int tick){
  if(animFrame < 0){
    bool hasMovingBlock = false;
    for(int i = 0;i < 128;++i){
      if(ptr->tempBlockUsed[i]){
	hasMovingBlock = true;
	break;
      }
    }
    if(!hasMovingBlock){animFrame = 0;} else {return 0;}
  }
  animFrame += tick;
  if(animFrame >= 3000){
    finished = true;
    render->finished = true;
    return 3;
  }
  return 0;
}

void MatchGameVictoryAnim::Render::Render2D(cairo_t *cr,PangoLayout *textLayout){
  int animFrame = controller->animFrame;
  if(animFrame < 0){return;}
  if(animFrame <= 1000){
    cairo_set_source_rgba(cr,0.0,0.0,0.0,(1.0 / 2000) * animFrame);
    cairo_rectangle(cr,0,0,renderer.winW,renderer.winH);
    cairo_fill(cr);
  } else {
    cairo_set_source_rgba(cr,0.0,0.0,0.0,0.5);
    cairo_rectangle(cr,0,0,renderer.winW,renderer.winH);
    cairo_fill(cr);
    cairo_set_source_rgba(cr,1.0,0.0,0.0,1.0);
    pango_layout_set_markup(textLayout,"<span font='120'>Victory!</span>",-1);
    int tw,th;
    pango_layout_get_pixel_size(textLayout,&tw,&th);
    tw /= 2;
    th /= 2;
    cairo_move_to(cr,renderer.winW / 2 - tw,renderer.winH / 2 - th);
    pango_cairo_show_layout(cr,textLayout);
    if(animFrame >= 2000){
      cairo_set_operator(cr,CAIRO_OPERATOR_OVER);
      cairo_set_source_rgba(cr,0.0,0.0,0.0,(1.0 / 1000) * (animFrame - 2000));
      cairo_rectangle(cr,0,0,renderer.winW,renderer.winH);
      cairo_fill(cr);
    }
  }
}
