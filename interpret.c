/*
 *	interpret.c
 *	zaverecna interpretace ridiciho programu
 *	Autor: Petr Zufan
 *	
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "define.h" //definice konstant
#include "str.h" 
#include "ial.h" //funkce find a sort a hashovaci tabulka
#include "instlist.h" //seznamem instrukci
#include "stable.h" //tabulka symbolu
#include "errors.h" 
#include "interpret.h"


int interpret (globalTS *GTS){
  tGData *func;
  struct Frame *newF;
  int success = SUCCESS;
  Tinst *instr;
  tLData *destination;
  tLData *source1;
  tLData *source2;
  tLData *source3;

  //vytvori seznam instrukci  
  TinstList LOI;
  ListInit(&LOI);

  //vytvori zasobnik ramcu
  TstackFrame SF;
  StackInit(&SF);

  //vytvori promenou string start = "main/0"
  string start;
  strInit(&start);
  int strAddChar(&start, 'm');
  int strAddChar(&start, 'a');
  int strAddChar(&start, 'i');
  int strAddChar(&start, 'n');

  //vyhleda funkci main v globalni tabulce symbolu
  func = GtableSearch(GTS, &start)
  if (func == NULL){
    return SEMANTIC_ERROR;
  }
  
  //vytvori ramec promennych z lokalni tabulky mainu
  newF = FrameCreate(GTS, start);
  if (newf == NULL){
    return INTERN_ERROR;
  }

  //vlozi ramec na zasobnik
  success = PushFrame(&SF, newF);
  if (success != SUCCESS){
    return INTERN_ERROR;
  }

  //vlozi seznam instrukci mainu do seznamu instrukci
  success = ListConect(&LOI, func->Linstr); 
  if (success != SUCCESS){
    return INTERN_ERROR;
  }

  //aktivuje prvni instrukci
  ListActFirst(LOI);  

  //ziska instrukci ze seznamu instrukci
  instr = ListGetInst(LOI);
  if (instr == NULL){
    ListDispose(LOI);
    return INTERN_ERROR;
  }

  //vyhleda operandy instrukce a ulozi si uazatele na jejich data
  source1 = VariableSearch(LOI, instr->src1);
  source2 = VariableSearch(LOI, instr->src2);
  destination = VariableSearch(LOI, instr->dest);


  //zavola funkci, ktera provede prislusnou instrukci
  while ( instr->itype != IEND ){
    switch (instr->itype){

      case IMOV:
        success = move(source1, destination);
        break;

      case IADD:
        success = addition(source1, source2, destination);
        break;

      case ISUB:
        success = substraction(source1, source2, destination);
        break;

      case IMUL:
        success = multiplication(source1, source2, destination);
        break;

      case IDIV:
        success = division(source1, source2, destination);
        break;

      case IEQUAL:
        success = equal(source1, source2, destination);
        break;

      case ISMALL:
        success = smaller(source1, source2, destination);
        break;

      case IBIG:
        success = bigger(source1, source2, destination);
        break;

      case IEQSM:
        success = equalsmaller(source1, source2, destination);
        break;

      case IEQBG:
        success = equalbigger(source1, source2, destination);
        break;

      case INOTEQ:
        success = notequal(source1, source2, destination);
        break;

      case INOT:
        success = negation(source1, destination);
        break;
/*
      case IAND:
        success = konjunction(source1, source2, destination);
        break;

      case IOR:
        success = disjunction(source1, source2, destination);
        break;
*/
      case ILABEL:
        break;

      case IGOTO:
        success = jump(instr->dest, LOI);
        break;

      case IIFGOTO:
        success = jumpif(source1, instr->dest, LOI);
        break;

      case IREADI:
        success = readint(destination);
        break;

      case IREADD:  
        success = readdouble(destination);
        break;

      case IREADS:  
        success = readstring(destination);
        break;

      case IREAD: 
        success = read(destination);
        break;

      case IWRITE:  
        success = write(source1);
        break;

      case IFIND:
        success = findme(source1, source2, destination);
        break;

      case ISORT:
        success = sortme(source1, destination);
        break;

      case ICAT:
        success = concatenate(source1, source2, destination);
        break;

      case ILENGTH:
        success = lengthstring(source1, destination);
        break;

      case ISUBSTR1:
        //nacte dalsi instrukci (ocekava ISUBSTR2)
        ListSucc(LOI);
        instr = ListGetInst(LOI);
        if (instr == NULL){
          ListDispose(LOI);
          return INTERN_ERROR;
        }

        //pokud byla ISUBSTR2 provede funkci. jinak chyba.
        if (instr->itype == ISUBSTR2){
          source2 = VariableSearch(LOI, *instr->src1);
          source3 = VariableSearch(LOI, *instr->src2);
          success = substring(source1, source2, source3, destination);
        }else{
          success = INTERN_ERROR;
        }
        break;

      default:
        success = INTERN_ERROR;
        break;
    }

    //pokud skoncila funkce neuspechem vrati chybu
    if (success != SUCCESS){
      ListDispose(LOI);
      return success;
    }

    //presune se na dalsi instrukci
    ListSucc(LOI);
    
    //ziska instrukci ze seznamu instrukci
    instr = ListGetInst(LOI);
    if (instr == NULL){
      ListDispose(LOI);
      return INTERN_ERROR;
    }
    
    //vyhleda operandy instrukce a ulozi si uazatele na jejich data
    source1 = VariableSearch(LOI, *instr->src1);
    source2 = VariableSearch(LOI, *instr->src2);
    destination = VariableSearch(LOI, *instr->dest);

  }//opakuje dokud nenarazi na konec

  //uvolni seznam z pameti
  ListDispose(LOI);

  //vrati 0 nebo cislo chyby
  return success;
}

//-------------------IMOV------------------------------------------------------

int move(tData *src1, tData *dest){
  
  if (!src1->isinit){
    return UNINIT_ERROR;

  }else if (src1->varType == TOK_INT){
    if (dest->varType == TOK_INT){
      dest->varValue.i = src1->varValue.i;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (double)src1->varValue.i;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = src1->varValue.i;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if (src1->varType == TOK_DOUBLE){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (int)src1->varValue.d;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = src1->varValue.d;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_DOUBLE;
      dest->varValue.d = src1->varValue.d;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if (src1->varType == TOK_STRING){
    if (dest->varType == TOK_STRING){
      if (strCopyString(&dest->varValue.s, &src1->varValue.s) == STR_SUCCESS){
        dest->isinit = true;
        return SUCCESS;
      }else{
        return INTERN_ERROR;
      }
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_STRING;
      if (strCopyString(&dest->varValue.s, &src1->varValue.s) == STR_SUCCESS){
        dest->isinit = true;
        return SUCCESS;
      }else{
        return INTERN_ERROR;
      }
    }else{
      return TYPE_ERROR;
    }

  }else if (src1->varType == TOK_AUTO){
    return CONVERT_ERROR;

  }else{
    return TYPE_ERROR;
  }

  return INTERN_ERROR;
}

//-------------------IADD------------------------------------------------------

int addition(tData *src1, tData *src2, tData *dest){

  if (!src1->isinit || !src2->isinit){
    return UNINIT_ERROR;

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = src1->varValue.i + src2->varValue.i;
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)(src1->varValue.i + src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_INT;
       dest->varValue.i = (src1->varValue.i + src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d + (double)src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (src1->varValue.d + (double)src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_DOUBLE;
       dest->varValue.d = (src1->varValue.d + (double)src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)((double)src1->varValue.i + src2->varValue.d);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = ((double)src1->varValue.i + src2->varValue.d);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_DOUBLE;
       dest->varValue.d = ((double)src1->varValue.i + src2->varValue.d);
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d + src2->varValue.d);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = src1->varValue.d + src2->varValue.d;
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_DOUBLE;
       dest->varValue.d = (src1->varValue.d + src2->varValue.d);
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_AUTO) || (src2->varType == TOK_AUTO) ){
    return CONVERT_ERROR;

  }else{
    return TYPE_ERROR;
  }

  return INTERN_ERROR;

}

//-------------------ISUB------------------------------------------------------
int substraction(tData *src1, tData *src2, tData *dest){
  
  if (!src1->isinit || !src2->isinit){
    return UNINIT_ERROR;

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = src1->varValue.i - src2->varValue.i;
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)(src1->varValue.i - src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_INT;
       dest->varValue.i = (src1->varValue.i - src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d - (double)src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (src1->varValue.d - (double)src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_DOUBLE;
       dest->varValue.d = (src1->varValue.d - (double)src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)((double)src1->varValue.i - src2->varValue.d);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)src1->varValue.i - src2->varValue.d;
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_DOUBLE;
       dest->varValue.d = (double)src1->varValue.i - src2->varValue.d;
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d - src2->varValue.d);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = src1->varValue.d - src2->varValue.d;
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_INT;
       dest->varValue.d = src1->varValue.d - src2->varValue.d;
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_AUTO) || (src2->varType == TOK_AUTO) ){
    return CONVERT_ERROR;

  }else{
    return TYPE_ERROR;
  }

  return INTERN_ERROR;
}

//-------------------IMUL------------------------------------------------------
int multiplication(tData *src1, tData *src2, tData *dest){

  if (!src1->isinit || !src2->isinit){
    return UNINIT_ERROR;

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = src1->varValue.i * src2->varValue.i;
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)(src1->varValue.i * src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_INT;
       dest->varValue.i = src1->varValue.i * src2->varValue.i;
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d * (double)src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (src1->varValue.d * (double)src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_DOUBLE;
       dest->varValue.d = (src1->varValue.d * (double)src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)((double)src1->varValue.i * src2->varValue.d);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)src1->varValue.i * src2->varValue.d;
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_DOUBLE;
       dest->varValue.d = (double)src1->varValue.i * src2->varValue.d;
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d * src2->varValue.d);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = src1->varValue.d * src2->varValue.d;
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_DOUBLE;
       dest->varValue.d = src1->varValue.d * src2->varValue.d;
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_AUTO) || (src2->varType == TOK_AUTO) ){
    return CONVERT_ERROR;

  }else{
    return TYPE_ERROR;
  }

  return INTERN_ERROR;
}

//-------------------IDIV------------------------------------------------------
int division(tData *src1, tData *src2, tData *dest){

  if (!src1->isinit || !src2->isinit){
    return UNINIT_ERROR;

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
     if (src2->varValue.i == 0){
       return ZERO_DIV_ERROR;
     }else if (dest->varType == TOK_INT){
       dest->varValue.i = src1->varValue.i / src2->varValue.i;
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)(src1->varValue.i / src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_INT;
       dest->varValue.i = src1->varValue.i / src2->varValue.i;
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
     if (src2->varValue.i == 0){
       return ZERO_DIV_ERROR;
     }else if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d / (double)src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (src1->varValue.d / (double)src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_DOUBLE;
       dest->varValue.d = (src1->varValue.d / (double)src2->varValue.i);
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
     if (src2->varValue.d == 0.0){
       return ZERO_DIV_ERROR;
     }else if (dest->varType == TOK_INT){
       dest->varValue.i = (int)((double)src1->varValue.i / src2->varValue.d);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)src1->varValue.i / src2->varValue.d;
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_DOUBLE;
       dest->varValue.d = (double)src1->varValue.i / src2->varValue.d;
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
     if (src2->varValue.d == 0.0){
       return ZERO_DIV_ERROR;
     }else if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d / src2->varValue.d);
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = src1->varValue.d / src2->varValue.d;
       dest->isinit = true;
       return SUCCESS;
     }else if (dest->varType == TOK_AUTO){
       dest->varType = TOK_DOUBLE;
       dest->varValue.d = src1->varValue.d / src2->varValue.d;
       dest->isinit = true;
       return SUCCESS;
     }else{
       return TYPE_ERROR;
     }

  }else if ( (src1->varType == TOK_AUTO) || (src2->varType == TOK_AUTO) ){
    return CONVERT_ERROR;

  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IEQUAL----------------------------------------------------
int equal(tData *src1, tData *src2, tData *dest){

  if (!src1->isinit || !src2->isinit){
    return UNINIT_ERROR;

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.i == src2->varValue.i) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.i == src2->varValue.i) ? 1.0 : 0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.i == src2->varValue.i) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.d == src2->varValue.d) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.d == src2->varValue.d) ? 1.0 : 0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.d == src2->varValue.d) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = ((double)src1->varValue.i == src2->varValue.d) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
     dest->varValue.d = ((double)src1->varValue.i == src2->varValue.d)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = ((double)src1->varValue.i == src2->varValue.d) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.d == (double)src2->varValue.i) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
     dest->varValue.d = (src1->varValue.d == (double)src2->varValue.i)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.d == (double)src2->varValue.i) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_STRING) && (src2->varType == TOK_STRING) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = 
           (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) == 0) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = 
         (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) == 0)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i =
           (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) == 0) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_AUTO) || (src2->varType == TOK_AUTO) ){
    return CONVERT_ERROR;

  }else{
    return TYPE_ERROR;
  }

  return INTERN_ERROR;
}

//-------------------ISMALL----------------------------------------------------
int smaller(tData *src1, tData *src2, tData *dest){

  if (!src1->isinit || !src2->isinit){
    return UNINIT_ERROR;

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.i < src2->varValue.i) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.i < src2->varValue.i) ? 1.0 : 0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.i < src2->varValue.i) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.d < src2->varValue.d) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.d < src2->varValue.d) ? 1.0 : 0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.d < src2->varValue.d) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = ((double)src1->varValue.i < src2->varValue.d) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = ((double)src1->varValue.i < src2->varValue.d)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = ((double)src1->varValue.i < src2->varValue.d) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.d < (double)src2->varValue.i) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.d < (double)src2->varValue.i)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.d < (double)src2->varValue.i) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_STRING) && (src2->varType == TOK_STRING) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = 
           (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) < 0) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = 
          (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) < 0)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i =
           (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) < 0) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_AUTO) || (src2->varType == TOK_AUTO) ){
    return CONVERT_ERROR;

  }else{
    return TYPE_ERROR;
  }

  return INTERN_ERROR;
}

//-------------------IBIG------------------------------------------------------
int bigger(tData *src1, tData *src2, tData *dest){

  if (!src1->isinit || !src2->isinit){
    return UNINIT_ERROR;

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.i > src2->varValue.i) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.i > src2->varValue.i) ? 1.0 : 0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.i > src2->varValue.i) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.d > src2->varValue.d) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.d > src2->varValue.d) ? 1.0 : 0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.d > src2->varValue.d) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = ((double)src1->varValue.i > src2->varValue.d) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = ((double)src1->varValue.i > src2->varValue.d)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = ((double)src1->varValue.i > src2->varValue.d) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.d > (double)src2->varValue.i) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.d > (double)src2->varValue.i)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.d > (double)src2->varValue.i) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_STRING) && (src2->varType == TOK_STRING) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = 
           (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) > 0) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = 
          (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) > 0)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i =
           (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) > 0) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_AUTO) || (src2->varType == TOK_AUTO) ){
    return CONVERT_ERROR;

  }else{
    return TYPE_ERROR;
  }

  return INTERN_ERROR;
}

//-------------------IEQSM-----------------------------------------------------
int equalsmaller(tData *src1, tData *src2, tData *dest){

  if (!src1->isinit || !src2->isinit){
    return UNINIT_ERROR;

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.i <= src2->varValue.i) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.i <= src2->varValue.i) ? 1.0 : 0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.i <= src2->varValue.i) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.d <= src2->varValue.d) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.d <= src2->varValue.d) ? 1.0 : 0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.d <= src2->varValue.d) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = ((double)src1->varValue.i <= src2->varValue.d) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
     dest->varValue.d = ((double)src1->varValue.i <= src2->varValue.d)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = ((double)src1->varValue.i <= src2->varValue.d) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.d <= (double)src2->varValue.i) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
     dest->varValue.d = (src1->varValue.d <= (double)src2->varValue.i)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.d <= (double)src2->varValue.i) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_STRING) && (src2->varType == TOK_STRING) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = 
           (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) <= 0) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = 
         (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) <= 0)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i =
           (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) <= 0) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_AUTO) || (src2->varType == TOK_AUTO) ){
    return CONVERT_ERROR;

  }else{
    return TYPE_ERROR;
  }

  return INTERN_ERROR;
}

//-------------------IEQBG-----------------------------------------------------
int equalbigger(tData *src1, tData *src2, tData *dest){

  if (!src1->isinit || !src2->isinit){
    return UNINIT_ERROR;

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.i >= src2->varValue.i) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.i >= src2->varValue.i) ? 1.0 : 0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.i >= src2->varValue.i) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.d >= src2->varValue.d) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.d >= src2->varValue.d) ? 1.0 : 0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.d >= src2->varValue.d) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = ((double)src1->varValue.i >= src2->varValue.d) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
     dest->varValue.d = ((double)src1->varValue.i >= src2->varValue.d)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = ((double)src1->varValue.i >= src2->varValue.d) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.d >= (double)src2->varValue.i) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
     dest->varValue.d = (src1->varValue.d >= (double)src2->varValue.i)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.d >= (double)src2->varValue.i) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_STRING) && (src2->varType == TOK_STRING) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = 
           (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) >= 0) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = 
         (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) >= 0)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i =
           (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) >= 0) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_AUTO) || (src2->varType == TOK_AUTO) ){
    return CONVERT_ERROR;

  }else{
    return TYPE_ERROR;
  }

  return INTERN_ERROR;
}

//-------------------INOTEQ----------------------------------------------------
int notequal(tData *src1, tData *src2, tData *dest){

  if (!src1->isinit || !src2->isinit){
    return UNINIT_ERROR;

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.i != src2->varValue.i) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.i != src2->varValue.i) ? 1.0 : 0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.i != src2->varValue.i) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.d != src2->varValue.d) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.d != src2->varValue.d) ? 1.0 : 0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.d != src2->varValue.d) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = ((double)src1->varValue.i != src2->varValue.d) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
     dest->varValue.d = ((double)src1->varValue.i != src2->varValue.d)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = ((double)src1->varValue.i != src2->varValue.d) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.d != (double)src2->varValue.i) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
     dest->varValue.d = (src1->varValue.d != (double)src2->varValue.i)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = (src1->varValue.d != (double)src2->varValue.i) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_STRING) && (src2->varType == TOK_STRING) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = 
           (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) != 0) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = 
         (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) != 0)?1.0:0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i =
           (strCmpString(&(src1->varValue.s), &(src2->varValue.s)) != 0) ? 1:0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if ( (src1->varType == TOK_AUTO) || (src2->varType == TOK_AUTO) ){
    return CONVERT_ERROR;

  }else{
    return TYPE_ERROR;
  }

  return INTERN_ERROR;
}

//-------------------INOT------------------------------------------------------
int negation(tData *src1, tData *dest){

  if (!src1->isinit){
    return UNINIT_ERROR;

  }else if (src1->varType == TOK_INT){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.i == 0) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.i == 0) ? 1.0 : 0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varValue.i = (src1->varValue.i == 0) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if (src1->varType == TOK_DOUBLE){
    if (dest->varType == TOK_INT){
      dest->varValue.i = (src1->varValue.d == 0.0) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (src1->varValue.d == 0.0) ? 1.0 : 0.0;
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varValue.i = (src1->varValue.d == 0.0) ? 1 : 0;
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else if (src1->varType == TOK_AUTO){
    return CONVERT_ERROR;

  }else{
      return TYPE_ERROR;
  }

  return INTERN_ERROR;
}

//-------------------IGOTO-----------------------------------------------------
int jump(string *dest, TinstList *LOI){ 
  return ListGoto(LOI, dest);
}

//-------------------IIFGOTO---------------------------------------------------
int jumpif(tData *src1, string *dest, TinstList *LOI){
  if (!src1->isinit){
    return UNINIT_ERROR;

  }else if (src1->varValue.i) {
    return ListGoto(LOI, dest);
  }else{
    return SUCCESS;
  }
  return INTERN_ERROR;
}

//-------------------IREADI----------------------------------------------------
int readint(tData *dest){ 
  if (dest->varType != TOK_INT){
    return TYPE_ERROR;
  }else if (scanf("%d", &(dest->varValue.i)) != 1){
    return READ_NUM_ERROR;
  }
  dest->isinit = true;
  return SUCCESS;
}

//-------------------IREADD----------------------------------------------------
int readdouble(tData *dest){ 
  if (dest->varType != TOK_DOUBLE){
    return TYPE_ERROR;
  }else if (scanf("%lg", &(dest->varValue.d)) != 1){
    return READ_NUM_ERROR;
  }
  dest->isinit = true;
  return SUCCESS;
}

//-------------------IREADS----------------------------------------------------
int readstring(tData *dest){
  char c;
 
  if (dest->varType != TOK_STRING){
    return TYPE_ERROR;
  }

  strFree(&dest->varValue.s);
  strInit(&dest->varValue.s);

  if (scanf("%c", &c) != 1){
    return RUNTIME_ERROR;
  }
  while ((c == '\n') || (c == '\t') || (c == ' ')){
    if (scanf("%c", &c) != 1){
      return RUNTIME_ERROR;
    }
  }

  while((c != '\n') && (c != '\t') && (c != ' ') && (c != EOF) && (c != ';')){
    if ( strAddChar(&(dest->varValue.s), c) == 1 ){
      return INTERN_ERROR;
    }
    if (scanf("%c", &c) != 1){
      return RUNTIME_ERROR;
    }
  }
  dest->isinit = true;
  return SUCCESS;
}

//-------------------IREAD----------------------------------------------------
int read(tData *dest){ 
  if (dest->varType == TOK_INT){
    return readint(dest);

  }else if (dest->varType == TOK_DOUBLE){
    return readdouble(dest);

  }else if (dest->varType == TOK_STRING){
    return readstring(dest);

  }else if (dest->varType == TOK_AUTO){
    return CONVERT_ERROR;

  }else{
    return INTERN_ERROR;
  }
}

//-------------------IWRITE----------------------------------------------------
int write(tData *src1){ 
  if (!src1->isinit){
    return UNINIT_ERROR;

  }else if (src1->varType == TOK_DOUBLE){
     if (printf("%g", src1->varValue.d) < 0){
       return RUNTIME_ERROR;
     }else{
       return SUCCESS;
     }

  }else if (src1->varType == TOK_INT){
    if (printf("%d", src1->varValue.i) < 0){
       return RUNTIME_ERROR;
     }else{
       return SUCCESS;
     }

  }else if (src1->varType == TOK_STRING){
    if (printf("%s", src1->varValue.s.str) < 0){
       return RUNTIME_ERROR;
     }else{
       return SUCCESS;
     }

  }else if (src1->varType == TOK_AUTO){
    return CONVERT_ERROR;
  }

  return INTERN_ERROR;
}

//-------------------ISORT-----------------------------------------------------
int sortme(tData *src1, tData *dest){

  if (!src1->isinit){
    return UNINIT_ERROR;

  }else if (src1->varType == TOK_STRING){
    if (dest->varType == TOK_STRING){
      dest->varValue.s = sort(src1->varValue.s);
      dest->isinit = true;
      return SUCCESS;

    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_STRING;
      dest->varValue.s = sort(src1->varValue.s);
      dest->isinit = true;
      return SUCCESS;

    }else{
      return TYPE_ERROR; 
    }

  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IFIND-----------------------------------------------------
int findme(tData *src1, tData *src2, tData *dest){

  if (!src1->isinit || !src2->isinit){
    return UNINIT_ERROR;

  }else if ( (src1->varType == TOK_STRING) && (src2->varType == TOK_STRING) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = find(&(src1->varValue.s), &(src2->varValue.s));
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (double)find(&src1->varValue.s, &src2->varValue.s);
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = find(&src1->varValue.s, &src2->varValue.s);
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------ILENGTH---------------------------------------------------
int lengthstring(tData *src1, tData *dest){

  if (!src1->isinit){
    return UNINIT_ERROR;

  }else if (src1->varType == TOK_STRING){
    if (dest->varType == TOK_INT){
      dest->varValue.i = length(src1->varValue.s);
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (double)length(src1->varValue.s);
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_INT;
      dest->varValue.i = length(src1->varValue.s);
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else{
    return TYPE_ERROR;
  }

  return INTERN_ERROR;
}

//-------------------ICAT------------------------------------------------------
int concatenate(tData *src1, tData *src2, tData *dest){
  if (!src1->isinit || !src2->isinit){
    return UNINIT_ERROR;

  }else if ( (src1->varType == TOK_STRING) && 
             (src2->varType == TOK_STRING) ){
    if (dest->varType == TOK_STRING){
      dest->varValue.s = concat(src1->varValue.s, src2->varValue.s);
      dest->isinit = true;
      return SUCCESS;
    }else if (dest->varType == TOK_AUTO){
      dest->varType = TOK_STRING;
      dest->varValue.s = concat(src1->varValue.s, src2->varValue.s);
      dest->isinit = true;
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }

  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------ISUBSTR---------------------------------------------------
int substring(tData *src1, tData *src2, tData *src3, tData *dest){

  if (!src1->isinit || !src2->isinit || !src3->isinit){
    return UNINIT_ERROR;

  }else if (src1->varType == TOK_STRING){
    if ( (src2->varType == TOK_INT) && 
         (src3->varType == TOK_INT) ){
      if (dest->varType == TOK_STRING){
        dest->varValue.s = substr(src1->varValue.s, src2->varValue.i, src3->varValue.i);
        dest->isinit = true;
        return SUCCESS;
      }else if (dest->varType == TOK_AUTO){
        dest->varType = TOK_STRING;
        dest->varValue.s = substr(src1->varValue.s, src2->varValue.i, src3->varValue.i);
        dest->isinit = true;
        return SUCCESS;
      }else{
        return TYPE_ERROR;
      }

    }else if ( (src2->varType == TOK_INT)    && 
               (src3->varType == TOK_DOUBLE) ){
      if (dest->varType == TOK_STRING){
        dest->varValue.s = substr(src1->varValue.s, src2->varValue.i, (int)src3->varValue.d);
        dest->isinit = true;
        return SUCCESS;
      }else if (dest->varType == TOK_AUTO){
        dest->varType = TOK_STRING;
        dest->varValue.s = substr(src1->varValue.s, src2->varValue.i, (int)src3->varValue.d);
        dest->isinit = true;
        return SUCCESS;
      }else{
        return TYPE_ERROR;
      }

    }else if ( (src2->varType == TOK_DOUBLE) && 
               (src3->varType == TOK_INT)    ){
      if (dest->varType == TOK_STRING){
        dest->varValue.s = substr(src1->varValue.s, (int)src2->varValue.d, src3->varValue.i);
        dest->isinit = true;
        return SUCCESS;
      }else if (dest->varType == TOK_AUTO){
        dest->varType = TOK_STRING;
        dest->varValue.s = substr(src1->varValue.s, (int)src2->varValue.d, src3->varValue.i);
        dest->isinit = true;
        return SUCCESS;
      }else{
        return TYPE_ERROR;
      }

    }else if ( (src2->varType == TOK_DOUBLE) && 
               (src3->varType == TOK_DOUBLE) ){
      if (dest->varType == TOK_STRING){
        dest->varValue.s = substr(src1->varValue.s, (int)src2->varValue.d, (int)src3->varValue.d);
        dest->isinit = true;
        return SUCCESS;
      }else if (dest->varType == TOK_AUTO){
        dest->varType = TOK_STRING;
        dest->varValue.s = substr(src1->varValue.s, (int)src2->varValue.d, (int)src3->varValue.d);
        dest->isinit = true;
        return SUCCESS;
      }else{
        return TYPE_ERROR;
      }


    }else if ( (src2->varType == TOK_AUTO) || 
               (src3->varType == TOK_AUTO) ){
      return CONVERT_ERROR;
    }else{
      return TYPE_ERROR;
    }
  }else{
    return TYPE_ERROR;
  }


  return INTERN_ERROR;
}
