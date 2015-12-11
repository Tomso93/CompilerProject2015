/*
 *	varframe.c
*	definice zasobniku ramcu promennych
 *	Autor: Petr Zufan
 *	
 */


#include <string.h>
#include "define.h"
#include "str.h"
#include <stdbool.h>
#include "instlist.h"
#include "stable.h"
#include "varframe.h"

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
  int success == SUCCESS;

  F->down = NULL;
  success = LtableFree (F->proms);
  free(F);
  return success;
}


struct Frame *FrameCreate(globalTS *GTS, string *funcName){
  int success = SUCCESS;
  struct Frame *newF;
  tGData *func;

  func = GtableSearch(GTS, funcName);
  if (func == NULL){
    return NULL;
  }

  //copy Ltables (LTable) do LTS
  localTS LTS;
  struct LtableItem *prom;
  int i;

  success = LtableInit(&LTS);
  if (success != SUCCESS){
    return NULL;
  }

  for(i = 0; i < TABLE_SIZE; i++){
    prom = (*(func->LTable))[i]; 
    while(prom != NULL){
      success = LtableInsert (&LTS, &prom->key, prom->data.varType);
      if (success != 0){
        return NULL;
      }
      if (prom->isinit){
        success = LtableInsertValue (&LTS, &prom->key, prom->data.varValue);
        if (success != SUCCESS){
          return NULL;
        }
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


tLData *VariableSearch(Tstackframe *S, string *varName){
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


int FrameInsertValue(Tstackframe *S, string *varName, Tvalue val){
  tLData *pom;

  pom = VariableSearch(S, varName);
  if (pom == NULL) {
    return DEFINE_ERROR;
  }else{
    if (pom->varType == TOK_INT){
      pom->varValue.i = val.i;
    }else if (pom->varType == TOK_DOUBLE){
      pom->varValue.d = val.d;
    }else if (pom->varType == TOK_STRING){
      strCopyString(&pom->varValue.s, &val.s);
    }else if (pom->varType == TOK_AUTO){
      return CONVERT_ERROR;
    }else{
      return TYPE_ERROR;
    }
    pom->isinit = true;
  }
  return SUCCESS;
}


int StackDispose (Tstackframe *S){
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

int FrameInsertVar(struct Frame *F, string *varName, int varType, Tvalue varVal){
  int success;
  success = LtableInsert (F->proms, varName, varType);
  if (success == SUCCESS){
    success = LtableInsertValue (F->proms, varName, varVal);    
  }

  return success;
}
