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

void MatchGameBeginAnim::Render::Render2D(){
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

void MatchGameVictoryAnim::Render::Render2D(){
  int animFrame = controller->animFrame;
  if(animFrame < 0){return;}
  if(animFrame <= 1000){
    renderer.DrawPureColor(0,1920,0,1080,0,0,0,1.0 / 2000 * animFrame);
  } else {
    renderer.DrawPureColor(0,1920,0,1080,0,0,0,0.5);
    int width = renderer.GetTextWidth("Victory!",120);
    renderer.DrawText("Victory!",960 - width / 2,600,width + 10,120,1,0,0,1);
    if(animFrame >= 2000){
      renderer.DrawPureColor(0,1920,0,1080,0,0,0,1.0 / 1000 * (animFrame - 2000));
    }
  }
}

/*
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
*/

int WorldmapTransToMatchAnim::Control(MachineGameStateWorldmap *ptr,int tick){
  animFrame += tick;
  if(animFrame >= 1000){
    finished = true;
    render->finished = true;
    return STATE_MATCHGAME;
  }
  return 0;
}

void WorldmapTransToMatchAnim::Render::Render2D(){
  renderer.DrawPureColor(0,1920,0,1080,0,0,0,0.001 * controller->animFrame);
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

void WorldmapQuestBox::Render::Render2D(){
  if(!controller->displayQuestList){return;}
  renderer.DrawPureColor(500,1500,580,880,0,0,1.0,0.5);
  if(controller->displayedQuest[0] == NULL){
    renderer.DrawText("There is nothing to do here",520,860,1000,40,1,1,1,1);
  } else {
    renderer.DrawPureColor(515,1415,725,805,0.0,1.0,1.0,0.5);
    renderer.DrawText("Things you can do here:",520,860,1000,45,1,1,1,1);
    for(int i = 0;i < 5;++i){
      if(controller->displayedQuest[i] == NULL){break;}
      renderer.DrawText(controller->displayedQuest[i]->qst->title,520,790 - i * 80,1000,45,1,1,1,1);
    }
  }
}
