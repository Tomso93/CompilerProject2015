/*
 *	instlist.c
 *	seznam instrukci
 *	Autor: Petr Zufan, Martin Wojaczek
 *	
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "define.h"
#include "str.h"
#include "errors.h"
#include "instlist.h"
#include "stable.h"
#include "lex.h"
#include "varframe.h"
#include "interpret.h"
#include "parser.h"


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

int ListInsert (TinstList *L, Tinst *I) {

  struct listItem *new;

  new = malloc(sizeof(struct listItem));
  if (new == NULL){
    return INTERN_ERROR;
  }else{
    new->inst = *I;               //tady by mohl byt problem. 
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

int ListGoto(TinstList *L, string *dest){
  bool found = false;
  L->act = L->first;
  while ( (!found) && (L->act != NULL) ){
    if (L->act->inst.itype == ILABEL){
      if (strCmpString(L->act->inst.src1, dest) == 0){
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

int ListActLast(TinstList *L){
  L->act = L->last;
  return SUCCESS;
}

int ListActFirst(TinstList *L){
  L->act = L->first;
  return SUCCESS;
}

int ListConect(TinstList *L1, TinstList *L2){
  if (L1->first == NULL){
    L1->first = L2->first;
    L1->last = L2->last;
    L1->act = L1->first;
    return SUCCESS;
  }else if (L1->act == NULL){
    return INTERN_ERROR;
  }else{
    L2->last->nextInst = L1->act->nextInst;
    L1->act->nextInst = L2->first;
    if (L1->act == L1->last){
      L1->last = L2->last;
    }
    return SUCCESS;
  }
} 
