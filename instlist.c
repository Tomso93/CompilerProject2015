/*
 *	instlist.c
 *	seznam instrukci
 *	Autor: Petr Zufan, Martin Wojaczek
 *	
 */

#include <stdbool.h>
#include <stdio.h>
#include <malloc.h>
#include "str.h"
#include "stable.h"
#include "instlist.h"
#include "define.h"

int ListInit (TinstList *L){
  L->first = NULL;
  L->last = NULL;
  L->act = NULL;
  return SUCCESS;
}

int ListDispose (TinstList *L) {
  struct listItem *pom;

  L->act = NULL;
  L->last = NULL;
  while(L->first != NULL){
    pom = L->first;
    L->first = L->first->nextInst;
    free(pom);
  }
  return SUCCESS;
}

int ListInsert (TinstList *L, Tinst I) {

  struct listItem *new;

  new = malloc(sizeof(struct listItem));
  if (new == NULL){
    return INTERN_ERROR;
  }else{
    new->inst = I;
    new->nextInst = NULL;
    if (L->first == NULL){
      L->first = new;
      L->act = new;
      L->last = new;
    }else{
      L->last->nextInst = new;
      L->last = new;
    }
  }
  return SUCCESS;
}

int ListSucc(TinstList *L){
  L->act = L->act->nextInst;
  return SUCCESS;
}

int ListGoto(TinstList *L, tData *dest){
  bool found = false;
  L->act = L->first;
  while ( (!found) && (L->act->nextInst != NULL) ){
    if (L->act->inst.itype == ILABEL){
      if (strCmpString(&(L->act->inst.src1->varValue.s), &(dest->varValue.s)) == 0){
        found = true;
      }else{
        L->act = L->act->nextInst;
      }
    }else{
      L->act = L->act->nextInst;
    }
  }
  if (!found){
    return INTERN_ERROR;
  }
  return SUCCESS;
}

Tinst *ListGetInst(TinstList *L){
  if ( (L == NULL) || (L->act == NULL) ){
    return NULL;
  }
  return &(L->act->inst);  
}

Tinst *ListGetLastInst(TinstList *L){
  if ( (L == NULL) || (L->act == NULL) ){
    return NULL;
  }
  return &(L->last->inst);  
}
