#include <stdio.h>
#include <malloc.h>
#include "str.h"
#include "stable.h"

void tableInit(tSymbolTable *T)
// inicializace tabulky
{
  T->first = NULL;
}

tTableItem* tableInsert(tSymbolTable *T, string *key, int varType)
// funkce vlozi do tabulky symbolu novy identifikator
{

  tTableItem *ptr;
  int found;
  ptr = T->first;
  found = 0;
  // overeni existence polozky s danym klicem v tabulce
  while ((ptr != NULL) && (!found))
  {
    found = (strCmpString(&(ptr->key), key) == 0);
    if (!found) ptr = ptr->nextItem;
  }
  if (found)
  {
     return ptr;
  }
  else
  {
    // vlozeni nove polozky
    tTableItem *newItem;
    newItem = (tTableItem*) malloc(sizeof(tTableItem));
    strInit(&(newItem->key));
    strCopyString(&(newItem->key), key);
    newItem->data.varType = varType;
    newItem->nextItem = T->first;
    T->first = newItem;
    return NULL;
  }
}

tData *tableSearch(tSymbolTable *T, string *key)
// pokud se dana polozka s klicem key v tabulce symbolu nachazi,
// funkce vrati ukazatel na data teto polozky, jinak vrati NULL
{
  tTableItem *ptr;
  int found;
  ptr = T->first;
  found = 0;
  while ((ptr != NULL) && (!found))
  {
    found = (strCmpString(&(ptr->key), key) == 0);
    if (!found) ptr = ptr->nextItem;
  }
  if (found)
     return &(ptr->data);
  else
     return NULL;
}

tData* tableRead ( tSymbolTable *T, string *key) {
// funkce hleda polozku s danym klicem, pokud se nachazi v tabulce symbolu,
// funkce vrati ukazatel na data teto polozky a danou polozku smaze
// a navaze predchozi a nasledujici polozky na sebe
// jinak vraci NULL

  tTableItem *ptr;
  tTableItem *preptr;
  int found;
  ptr = T->first;
  preptr = NULL;
  found = 0;

  while ((ptr != NULL) && (!found))
  {
    found = (strCmpString(&(ptr->key), key) == 0);
    if (!found){
        preptr = ptr;
        ptr = ptr->nextItem;
    }
  }
  if (found){
     preptr->nextItem = ptr->nextItem;
     free(&ptr->key);
     return &(ptr->data);
  }else
     return NULL;
}

void tableItemDelete ( tSymbolTable *T, string *key) {
// funkce hleda polozku s danym klicem, pokud se nachazi v tabulce symbolu,
// funkce danou polozku smazea navaze predchozi a nasledujici polozky na sebe

  tTableItem *ptr;
  tTableItem *preptr;
  int found;
  ptr = T->first;
  preptr = NULL;
  found = 0;

  while ((ptr != NULL) && (!found))
  {
    found = (strCmpString(&(ptr->key), key) == 0);
    if (!found){
        preptr = ptr;
        ptr = ptr->nextItem;
    }
  }
  if (found){
     preptr->nextItem = ptr->nextItem;
     free(&ptr->key);
     strFree(&ptr->data.varValue.s);
  }
}

void tableFree(tSymbolTable *T)
// funkce dealokuje tabulku symbolu
{
  tTableItem *ptr;
  while (T->first != NULL)
  {
     ptr = T->first;
     T->first = T->first->nextItem;
     // uvolneni dat a klice
     strFree(&ptr->data.varValue.s);
     strFree(&ptr->key);
     // nakonec uvolnime celou polozku
     free(ptr);
  }
}
