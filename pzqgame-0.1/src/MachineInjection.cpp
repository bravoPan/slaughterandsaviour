#include "common.h"
#include "MachineInjection.h"
#include "MachineGame.h"
#include "GlobalObjects.h"
#include "Dialogue.h"
#include "Quest.h"

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

int WorldmapDialogueBox::Control(MachineGameStateWorldmap *ptr,int tick){
  ptr->lockControl = true;
  if(ptr->enterPressed){
    ptr->enterPressed = false;
    diag = diag->NextTurn();
    if(diag == NULL){
      finished = true;
      render->finished = true;
      ptr->lockControl = false;
    }
  }
  return 0;
}

void WorldmapDialogueBox::Render::Render2D(cairo_t *cr,PangoLayout *textLayout){
  cairo_set_source_rgba(cr,0.0,0.0,1.0,0.5);
  cairo_rectangle(cr,60,800,1800,200);
  cairo_fill(cr);
  cairo_set_source_rgba(cr,1.0,1.0,1.0,1.0);
  cairo_move_to(cr,70,820);
  pango_layout_set_width(textLayout,1800 * PANGO_SCALE);
  pango_layout_set_markup(textLayout,controller->diag->currLine,-1);
  pango_cairo_show_layout(cr,textLayout);
}

int WorldmapTransToMatchAnim::Control(MachineGameStateWorldmap *ptr,int tick){
  animFrame += tick;
  if(animFrame >= 1000){
    finished = true;
    render->finished = true;
    return STATE_MATCHGAME;
  }
  return 0;
}

void WorldmapTransToMatchAnim::Render::Render2D(cairo_t *cr,PangoLayout *textLayout){
  cairo_set_source_rgba(cr,0.0,0.0,0.0,0.001 * controller->animFrame);
  cairo_rectangle(cr,0,0,renderer.winW,renderer.winH);
  cairo_fill(cr);
}

int WorldmapQuestBox::Control(MachineGameStateWorldmap *ptr,int tick){
  if(ptr->globalState->inQuest){
    int ret = ptr->globalState->currQuest->Control(ptr,tick);
    if(ret == -1){
      delete ptr->globalState->currQuest;
      ptr->globalState->currQuest = NULL;
      ptr->globalState->inQuest = false;
    }
    return 0;
  } else {
    int u = ptr->posX + 36,v = ptr->posY + 36;
    if(u % 6 == 0 || v % 6 == 0){
      inRoom = false;
      for(int i = 0;i < 5;++i){displayedQuest[i] = NULL;}
    } else {
      if(!inRoom){
	inRoom = true;
	for(int i = 0;i < 5;++i){displayedQuest[i] = NULL;}
	QuestList *ptr2 = ptr->globalState->questList[u / 6][v / 6];
	int t = 0;
	while(t < 5 && ptr2 != NULL){
	  displayedQuest[t] = ptr2;
	  ptr2 = ptr2->next;
	  ++t;
	}
      }
    }
    
    if(ptr->tPressed){
      ptr->tPressed = false;
      displayQuestList = !displayQuestList;
      ptr->lockControl = displayQuestList;
      return 0;
    }
    if(displayQuestList){
      if(ptr->downPressed){
	ptr->downPressed = false;
	if(displayedQuest[0]->next == NULL){return 0;}
	for(int i = 0;i < 4;++i){displayedQuest[i] = displayedQuest[i + 1];}
	if(displayedQuest[4] != NULL){displayedQuest[4] = displayedQuest[4]->next;}
	return 0;
      }
      if(ptr->upPressed){
	ptr->upPressed = false;
	if(displayedQuest[0]->prev == NULL){return 0;}
	for(int i = 4;i > 0;--i){displayedQuest[i] = displayedQuest[i - 1];}
        displayedQuest[0] = displayedQuest[0]->prev;
	return 0;
      }
      if(ptr->enterPressed){
	if(displayedQuest[0] == NULL){return 0;}
	ptr->globalState->inQuest = true;
	ptr->globalState->currQuest = displayedQuest[0]->qst;
	if(displayedQuest[0]->prev == NULL){
	  ptr->globalState->questList[u / 6][v / 6] = displayedQuest[0]->next;
	} else {
	  displayedQuest[0]->prev->next = displayedQuest[0]->next;
	}
	if(displayedQuest[0]->next != NULL){
	  displayedQuest[0]->next->prev = displayedQuest[0]->prev;
	}
	delete displayedQuest[0];
	displayQuestList = false;
	ptr->lockControl = false;
	inRoom = false;
      }
    }
    return 0;
  }
}

void WorldmapQuestBox::Render::Render2D(cairo_t *cr,PangoLayout *textLayout){
  if(!controller->displayQuestList){return;}
  cairo_set_source_rgba(cr,0.0,0.0,1.0,0.5);
  cairo_rectangle(cr,500,200,1000,500);
  cairo_fill(cr);
  cairo_set_source_rgba(cr,1.0,1.0,1.0,1.0);
  if(controller->displayedQuest[0] == NULL){
    pango_layout_set_width(textLayout,960 * PANGO_SCALE);
    pango_layout_set_markup(textLayout,"There is nothing to do here",-1);
    cairo_move_to(cr,520,220);
    pango_cairo_show_layout(cr,textLayout);
  } else {
    cairo_set_source_rgba(cr,0.0,1.0,1.0,0.5);
    cairo_rectangle(cr,515,275,900,80);
    cairo_fill(cr);
    cairo_set_source_rgba(cr,1.0,1.0,1.0,1.0);
    pango_layout_set_width(textLayout,960 * PANGO_SCALE);
    pango_layout_set_markup(textLayout,"Things you can do here:",-1);
    cairo_move_to(cr,520,220);
    pango_cairo_show_layout(cr,textLayout);
    for(int i = 0;i < 5;++i){
      if(controller->displayedQuest[i] == NULL){break;}
      pango_layout_set_markup(textLayout,controller->displayedQuest[i]->qst->title,-1);
      cairo_move_to(cr,520,280 + i * 80);
      pango_cairo_show_layout(cr,textLayout);
    }
  }
}
