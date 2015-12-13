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

int hash(string *key){
  int delka = strGetLength(key);
  int vysledek = 0; 
  for (int i = 0; i < delka; i++){
    vysledek = vysledek + (int)key->str[i];
  }
  vysledek = vysledek % TABLE_SIZE;
  return vysledek;
}

//-----lokalni tabulka symbolu-------------------------------------------------

int LtableInit(localTS *T){
  for (int i = 0; i < TABLE_SIZE; i++){
    (*T)[i] = NULL;
  }
  return SUCCESS;
}


int LtableFree (localTS *T){
//  int success;
  tLTableItem *act;
  for (int i = 0; i < TABLE_SIZE; i++){
    act = (*T)[i];
    while (act != NULL){
      strFree(&(act->key));
      if (act->data.varType == TOK_STRING){
        strFree(&(act->data.varValue.s));
      }
      (*T)[i] = act->nextItem;
      free(act);
      act = (*T)[i];
    }
  }
  return SUCCESS;
}


int LtableInsert (localTS *T, string *key, int varType){
  int hashed = hash(key);
//  tLTableItem *act;
  tLTableItem *newItem;
  newItem = malloc(sizeof(tLTableItem));
  if (newItem == NULL){
    return INTERN_ERROR;
  }
  
  strInit(&(newItem->key));
  strCopyString(&(newItem->key), key);
  newItem->data.varType = varType;
  newItem->data.isinit = false;
  newItem->nextItem = (*T)[hashed];
  (*T)[hashed] = newItem;
  return SUCCESS;
}


tLData *LtableSearch (localTS *T, string *key){
  int i;
  int hashed = hash(key);
  bool found = false;
  tLTableItem *act = (*T)[hashed];
  while (found != true && act != NULL){
    if ((i = strCmpString(key, &(act->key))) == 0){
      found = true;
    }else{
      act = act->nextItem;
    }
  }

  if (found){
    return &(act->data);
  }else{
    return NULL;
  }
}


int LtableInsertValue (localTS *T, string *key, Tvalue v){
  tLData *act;
  act = LtableSearch (T, key);
  if (act == NULL){
    return INTERN_ERROR;
  }
  switch(act->varType){

    case TOK_INT:
      act->varValue.i = v.i;
    break;

    case TOK_DOUBLE:
      act->varValue.d = v.d;
    break;

    case TOK_STRING:
      strInit(&(act->varValue.s));
      strCopyString(&(act->varValue.s), &(v.s));
    break;

    case TOK_AUTO:
      return CONVERT_ERROR;
    break;
  }
  act->isinit = true;
  return SUCCESS;
}


//-----globalni tabulka symbolu------------------------------------------------

int GtableInit (globalTS *T){
  for (int i = 0; i < TABLE_SIZE; i++){
    (*T)[i] = NULL;
  }
  return SUCCESS;
}


int GtableFree (globalTS *T){
  tGTableItem *act;
  for (int i = 0; i < TABLE_SIZE; i++){
    act = (*T)[i];
    while (act != NULL){
      strFree(&(act->key));

      for (int j = 0; j < TABLE_SIZE; j++){
        if (act->data.params[j] != NULL){
          strFree(act->data.params[j]);
        }
      } 
      LtableFree(act->data.LTable);
      ListDispose(act->data.LInstr);
      (*T)[i] = act->nextItem;
      free(act);
      act = (*T)[i];
    }
  }
  return SUCCESS;
}


int GtableInsert (globalTS *T, string *key, int funcType){
  localTS *LTS;
  TinstList *LIL;
  
  
  int success;
  int hashed = hash(key);
//  tGTableItem *act;
  tGTableItem *newItem;

  newItem = malloc(sizeof(tGTableItem));
  if (newItem == NULL){
    return INTERN_ERROR;
  }

  LTS = malloc(sizeof(localTS));
  if (LTS == NULL){
    return INTERN_ERROR;
  }

  LIL = malloc(sizeof(TinstList));
  if (LIL == NULL){
    return INTERN_ERROR;
  }

  success = LtableInit(LTS);
  if (success != SUCCESS){
    return INTERN_ERROR;
  }
  success = ListInit(LIL);
  if (success != SUCCESS){
    return INTERN_ERROR;
  }
  strInit(&(newItem->key));
  strCopyString(&(newItem->key), key);
  newItem->data.funcType = funcType;
  newItem->data.isdef = false;
  newItem->data.LTable = LTS;
  newItem->data.LInstr = LIL;
  for (int i = 0; i < TABLE_SIZE; i++){
    newItem->data.params[i] = NULL;
  } 
  newItem->nextItem = (*T)[hashed];
  (*T)[hashed] = newItem;
  return SUCCESS;
}


tGData* GtableSearch (globalTS *T, string *key){
  int i;
  int hashed = hash(key);
  bool found = false;
  tGTableItem *act = (*T)[hashed];
  while (found != true && act != NULL){
    if ((i = strCmpString(key, &(act->key))) == 0){
      found = true;
    }else{
      act = act->nextItem;
    }
  }

  if (found){
    return &(act->data);
  }else{
    return NULL;
  }
}


int GtableInsertVar (globalTS *T, string *funcID, string *varID, int varType){
  int success;
  tGData *act;
  act = GtableSearch (T, funcID);
  if (act == NULL){
    return INTERN_ERROR;
  }
  success = LtableInsert (act->LTable, varID, varType);
  act->isdef = true;
  return success;
}


int GtableInsertVarVal (globalTS *T, string *funcID, string *varID, Tvalue v){
  int success;
  tGData *act;
  act = GtableSearch (T, funcID);
  if (act == NULL){
    return INTERN_ERROR;
  }
  success = LtableInsertValue (act->LTable, varID, v);
  act->isdef = true;
  return success;
}


int GtableInsertParam (globalTS *T, string *funcID, string *parID, int parType){
  int success;
  int i;
  tGData *act;
  act = GtableSearch (T, funcID);
  if (act == NULL){
    return INTERN_ERROR;
  }
  success = LtableInsert (act->LTable, parID, parType);
  if (success != SUCCESS){
    return success;
  }
  i = 0;
  while (act->params[i] != NULL){
    i++;
  }
  act->params[i] = parID;
  act->isdef = true;
  return success;
}

int GtableInsertInstr (globalTS *T, string *funcID, Tinst *instrukce){
  int success;
  tGData *act;
  act = GtableSearch (T, funcID);
  if (act == NULL){
    return INTERN_ERROR;
  }
  success = ListInsert(act->LInstr, instrukce);
  act->isdef = true;
  return success;
}



int GtablePrintVars (globalTS *T, string *funcID){
  printf("\n");
  tGData *act;
  localTS *lokalni;
  tLTableItem *prom;
  act = GtableSearch (T, funcID);
  if (act == NULL){
    return INTERN_ERROR;
  }
  printf("%s variables:\n", funcID->str);
  lokalni = act->LTable;
  for (int i = 0; i < TABLE_SIZE; i++){
    prom = (*lokalni)[i];
    if(prom == NULL){
      printf("null\n");
    }else{
      printf("not null\n");
    }

    while (prom != NULL){
      if (prom->data.varType == TOK_INT){
        printf("int %s", prom->key.str);
      }else if (prom->data.varType == TOK_DOUBLE){
        printf("double %s", prom->key.str);
      }else if (prom->data.varType == TOK_STRING){
        printf("string %s", prom->key.str);
      }else if (prom->data.varType == TOK_AUTO){
        printf("auto %s", prom->key.str);
      }
      if (prom->data.isinit){
        if (prom->data.varType == TOK_INT){
          printf(" = %d",prom->data.varValue.i);
        }else if (prom->data.varType == TOK_DOUBLE){
          printf(" = %g",prom->data.varValue.d);
        }else if (prom->data.varType == TOK_STRING){
          printf(" = %s",prom->data.varValue.s.str);
        }
      }
      printf("\n");
      prom = prom->nextItem;
    }
  }
    int i = 0;
    while (act->params[i] != NULL){
      printf("param %s\n", act->params[i]->str);
      i++; 
    }
  
  return SUCCESS;
}

int GtablePrintVarsAll (globalTS *T){
  tGTableItem *funkce;
  for (int i = 0; i< TABLE_SIZE; i++){
    funkce = (*T)[i];
    while (funkce != NULL){
      printf("funkce %s", funkce->key.str);
      GtablePrintVars (T, &funkce->key);
      funkce = funkce->nextItem;
    }
  }
  return SUCCESS;
}

int GTablePrintInst (globalTS *T, string* funcID){
  tGData *funkce;
  TinstList *seznam;
  Tinst instrukce;

  char *str1;
  char *str2;
  char *str3;


  if(T == NULL){
    return INTERN_ERROR;
  }

  funkce = GtableSearch(T, funcID);
  if (funkce == NULL){
    return INTERN_ERROR;
  }

  seznam = funkce->LInstr;
  seznam->act = seznam->first;
  while(seznam->act != NULL){
    instrukce = seznam->act->inst;
    if (instrukce.src1 == NULL){
      str1 = "NULL\0";
    }else{
      str1 = instrukce.src1->str;
    }
    if (instrukce.src2 == NULL){
      str2 = "NULL\0";
    }else{
      str2 = instrukce.src2->str;
    }
    if (instrukce.dest == NULL){
      str3 = "NULL\0";
    }else{
      str3 = instrukce.dest->str;
    }
    printf("%d,\t%s,\t%s,\t%s\n", instrukce.itype, str1, str2, str3);
    seznam->act = seznam->act->nextInst;
  }

  return SUCCESS;
}

