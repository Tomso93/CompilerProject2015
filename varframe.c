/*
 *	varframe.c
 *	definice zasobniku ramcu promennych
 *	Autor: Petr Zufan
 *	
 */

#include "varframe.h"
#include <string.h>
#include "define.h"
#include "str.h"
#include <stdbool.h>
#include "instlist.h"
#include "stable.h"

//!!!!!!!!!!!!!!globalni promenna!!!!!!!!!!!!!!
globalTS *GTS;


int StackInit(Tstackframe *S){
  S->top = NULL;
  S->act = NULL;
  return SUCCESS;
}

int StackPush(Tstackframe *S, struct Frame *F){
  F->down = S->top;
  S->top = F;
  S->act = F;
  return SUCCESS;
}

struct Frame *StackPop(Tstackframe *F){
  struct Frame *pom;

  if (S->top == NULL){
    return NULL;
  }
  pom = S->top;
  S->top = S->top->down;
  S->act = S->top;
  return pom;
}

int FrameDelete(struct Frame *F){
  struct FrameItem *pom;
  int success == 0;

  F->down = NULL;
  success = LtableFree (F->proms);
  free(F);
  return success;
}


struct Frame *FrameCreate(string funcName){
  int success = 0;
  struct Frame *newF;
  tGData *func;

  func = GtableSearch(GTS, funcName);
  if (func == NULL){
    return NULL;
  }

  //copy Ltables (LTable + params) to LTS
  localTS LTS;
  struct LtableItem *prom;
  int i;

  success = LtableInit(&LTS);
  if (success != 0){
    return NULL;
  }
  for(i = 0; i < 20; i++){
    prom = func->params[i]; 
    while(prom != NULL){
      success = LtableInsert (&LTS, &prom->key, prom->data.varType);
      if (success != 0){
        return NULL;
      }
      prom = prom->nextItem;
    }
    prom = func->LTable[i]; 
    while(prom != NULL){
      success = LtableInsert (&LTS, &prom->key, prom->data.varType);
      if (success != 0){
        return NULL;
      }
      prom = prom->nextItem;
    }
  }

  //

  newF = malloc(sizeof(struct Frame));
  if (newF == NULL){
    return NULL;
  }

  newF->base = true;
  newF->down = NULL;
  newF->proms = &LTS;

  return new;
}


tLData *VariableSearch(TstackFrame *S, string varName){
  tLData *result;
  S->act = S->top;

    while(S->act->base == false){
      if ( (result = LtableSearch(S->act->proms, varName)) != NULL){
        return result;
      }else{
        S->act = S->act->down;
      }
    }
    if ( (result = LtableSearch(S->act->proms, varName)) != NULL){
      return result;
    }

  return NULL;
}

int FrameInsertValue(TstackFrame *S, string varName, Tvalue val){
  tLData *pom;

  pom = VariableSearch(S, varName);
  if (pom == NULL) {
    return DEFINE_ERROR;
  }else{
    pom->varValue = val;
    pom->isinit = true;
  }
  return SUCCESS;
}


int StackDispose (TstackFrame *S){
  struct Frame *ramec;
  int success;

  while (S->top != NULL){
    ramec = PopFrame(S);
    if (ramec == NULL){
      return INTERN_ERROR;
    }
    success = FrameDelete(ramec);
    if (success != 0){
      return INTERN_ERROR;
    }
  }

  return SUCCESS;
}
