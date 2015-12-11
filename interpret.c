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
#include "varframe.h" //ramec promennych
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
  tGData *source4;
  int counter;

  //vytvori seznam instrukci  
  TinstList LOI;
  ListInit(&LOI);

  //vytvori zasobnik ramcu
  Tstackframe SF;
  StackInit(&SF);

  //vytvori promenou string start = "main/0"
  string start;
  strInit(&start);
  strAddChar(&start, 'm');
  strAddChar(&start, 'a');
  strAddChar(&start, 'i');
  strAddChar(&start, 'n');

  //vyhleda funkci main v globalni tabulce symbolu
  func = GtableSearch(GTS, &start);
  if (func == NULL){
    return SEMANTIC_ERROR;
  }
  
  //vytvori ramec promennych z lokalni tabulky mainu
  newF = FrameCreate(GTS, &start);
  if (newF == NULL){
    return INTERN_ERROR;
  }

  //vlozi ramec na zasobnik
  success = PushFrame(&SF, newF);
  if (success != SUCCESS){
    return INTERN_ERROR;
  }

  //vlozi seznam instrukci mainu do seznamu instrukci
  success = ListConect(&LOI, func->LInstr); 
  if (success != SUCCESS){
    return INTERN_ERROR;
  }

  //aktivuje prvni instrukci
  ListActFirst(&LOI);  

  //ziska instrukci ze seznamu instrukci
  instr = ListGetInst(&LOI);
  if (instr == NULL){
    ListDispose(&LOI);
    return INTERN_ERROR;
  }

  //zavola funkci, ktera provede prislusnou instrukci
  while ( instr->itype != IEND ){
    switch (instr->itype){

      case IMOV:
        source1 = VariableSearch(&SF, instr->src1);
        destination = VariableSearch(&SF, instr->dest);
        success = move(source1, destination);
      break;

      case IADD:
        source1 = VariableSearch(&SF, instr->src1);
        source2 = VariableSearch(&SF, instr->src2);
        destination = VariableSearch(&SF, instr->dest);
        success = addition(source1, source2, destination);
      break;

      case ISUB:
        source1 = VariableSearch(&SF, instr->src1);
        source2 = VariableSearch(&SF, instr->src2);
        destination = VariableSearch(&SF, instr->dest);
        success = substraction(source1, source2, destination);
      break;

      case IMUL:
        source1 = VariableSearch(&SF, instr->src1);
        source2 = VariableSearch(&SF, instr->src2);
        destination = VariableSearch(&SF, instr->dest);
        success = multiplication(source1, source2, destination);
      break;

      case IDIV:
        source1 = VariableSearch(&SF, instr->src1);
        source2 = VariableSearch(&SF, instr->src2);
        destination = VariableSearch(&SF, instr->dest);
        success = division(source1, source2, destination);
      break;

      case IEQUAL:
        source1 = VariableSearch(&SF, instr->src1);
        source2 = VariableSearch(&SF, instr->src2);
        destination = VariableSearch(&SF, instr->dest);
        success = equal(source1, source2, destination);
      break;

      case ISMALL:
        source1 = VariableSearch(&SF, instr->src1);
        source2 = VariableSearch(&SF, instr->src2);
        destination = VariableSearch(&SF, instr->dest);
        success = smaller(source1, source2, destination);
      break;

      case IBIG:
        source1 = VariableSearch(&SF, instr->src1);
        source2 = VariableSearch(&SF, instr->src2);
        destination = VariableSearch(&SF, instr->dest);
        success = bigger(source1, source2, destination);
      break;

      case IEQSM:
        source1 = VariableSearch(&SF, instr->src1);
        source2 = VariableSearch(&SF, instr->src2);
        destination = VariableSearch(&SF, instr->dest);
        success = equalsmaller(source1, source2, destination);
      break;

      case IEQBG:
        source1 = VariableSearch(&SF, instr->src1);
        source2 = VariableSearch(&SF, instr->src2);
        destination = VariableSearch(&SF, instr->dest);
        success = equalbigger(source1, source2, destination);
      break;

      case INOTEQ:
        source1 = VariableSearch(&SF, instr->src1);
        source2 = VariableSearch(&SF, instr->src2);
        destination = VariableSearch(&SF, instr->dest);
        success = notequal(source1, source2, destination);
      break;

      case INOT:
        source1 = VariableSearch(&SF, instr->src1);
        destination = VariableSearch(&SF, instr->dest);
        success = negation(source1, destination);
      break;

      case ILABEL:
      break;

      case IGOTO:
        success = jump(instr->dest, &LOI);
      break;

      case IIFGOTO:
        source1 = VariableSearch(&SF, instr->src1);
        success = jumpif(source1, instr->dest, &LOI);
      break;

      case IREADI:
        destination = VariableSearch(&SF, instr->dest);
        success = readint(destination);
      break;

      case IREADD:
        destination = VariableSearch(&SF, instr->dest);  
        success = readdouble(destination);
      break;

      case IREADS:
        destination = VariableSearch(&SF, instr->dest);  
        success = readstring(destination);
      break;

      case IREAD: 
        destination = VariableSearch(&SF, instr->dest);
        success = read(destination);
      break;

      case IWRITE:
        source1 = VariableSearch(&SF, instr->src1);  
        success = write(source1);
      break;

      case IFIND:
        source1 = VariableSearch(&SF, instr->src1);
        source2 = VariableSearch(&SF, instr->src2);
        destination = VariableSearch(&SF, instr->dest);
        success = findme(source1, source2, destination);
      break;

      case ISORT:
        source1 = VariableSearch(&SF, instr->src1);
        destination = VariableSearch(&SF, instr->dest);
        success = sortme(source1, destination);
      break;

      case ICAT:
        source1 = VariableSearch(&SF, instr->src1);
        source2 = VariableSearch(&SF, instr->src2);
        destination = VariableSearch(&SF, instr->dest);
        success = concatenate(source1, source2, destination);
      break;

      case ILENGTH:
        source1 = VariableSearch(&SF, instr->src1);
        destination = VariableSearch(&SF, instr->dest);
        success = lengthstring(source1, destination);
      break;

      case ISUBSTR1:
        source1 = VariableSearch(&SF, instr->src1);
        destination = VariableSearch(&SF, instr->dest);
        //nacte dalsi instrukci (ocekava ISUBSTR2)
        ListSucc(&LOI);
        instr = ListGetInst(&LOI);
        if (instr == NULL){
          ListDispose(&LOI);
          return INTERN_ERROR;
        }
        //pokud byla ISUBSTR2 provede funkci. jinak chyba.
        if (instr->itype == ISUBSTR2){
          source2 = VariableSearch(&SF, instr->src1);
          source3 = VariableSearch(&SF, instr->src2);
          success = substring(source1, source2, source3, destination);
        }else{
          success = INTERN_ERROR;
        }
      break;

      case IPRECALL:
        success = precall(instr->src1, GTS, newF);
        counter = 0;
      break;

      case IPAR:
        source4 = GtableSearch(GTS, instr->src1);
        source2 = VariableSearch(&SF, instr->src2);
        success = parametr(source4, source2, newF, counter);
        counter++;
      break;

      case ICALL:
        source4 = GtableSearch(GTS, instr->src1);
        success = call(source4, newF, &LOI, &SF);
      break;

      case IRET:
        source1 = VariableSearch(&SF, instr->src1);        
        success = ret(source1, &SF);
      break;

      default:
        success = INTERN_ERROR;
      break;
    }

    //pokud skoncila funkce neuspechem vrati chybu
    if (success != SUCCESS){
      ListDispose(&LOI);
      return success;
    }

    //presune se na dalsi instrukci
    ListSucc(&LOI);
    
    //ziska instrukci ze seznamu instrukci
    instr = ListGetInst(&LOI);
    if (instr == NULL){
      ListDispose(&LOI);
      return INTERN_ERROR;
    }
    
  }//opakuje dokud nenarazi na konec

  //uvolni seznam z pameti
  ListDispose(&LOI);

  //vrati 0 nebo cislo chyby
  return success;
}

//-------------------IMOV------------------------------------------------------

int move(tLData *src1, tLData *dest){
  
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

int addition(tLData *src1, tLData *src2, tLData *dest){

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
int substraction(tLData *src1, tLData *src2, tLData *dest){
  
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
int multiplication(tLData *src1, tLData *src2, tLData *dest){

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
int division(tLData *src1, tLData *src2, tLData *dest){

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
int equal(tLData *src1, tLData *src2, tLData *dest){

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
int smaller(tLData *src1, tLData *src2, tLData *dest){

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
int bigger(tLData *src1, tLData *src2, tLData *dest){

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
int equalsmaller(tLData *src1, tLData *src2, tLData *dest){

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
int equalbigger(tLData *src1, tLData *src2, tLData *dest){

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
int notequal(tLData *src1, tLData *src2, tLData *dest){

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
int negation(tLData *src1, tLData *dest){

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
int jumpif(tLData *src1, string *dest, TinstList *LOI){
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
int readint(tLData *dest){ 
  if (dest->varType != TOK_INT){
    return TYPE_ERROR;
  }else if (scanf("%d", &(dest->varValue.i)) != 1){
    return READ_NUM_ERROR;
  }
  dest->isinit = true;
  return SUCCESS;
}

//-------------------IREADD----------------------------------------------------
int readdouble(tLData *dest){ 
  if (dest->varType != TOK_DOUBLE){
    return TYPE_ERROR;
  }else if (scanf("%lg", &(dest->varValue.d)) != 1){
    return READ_NUM_ERROR;
  }
  dest->isinit = true;
  return SUCCESS;
}

//-------------------IREADS----------------------------------------------------
int readstring(tLData *dest){
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
int read(tLData *dest){ 
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
int write(tLData *src1){ 
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
int sortme(tLData *src1, tLData *dest){

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
int findme(tLData *src1, tLData *src2, tLData *dest){

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
int lengthstring(tLData *src1, tLData *dest){

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
int concatenate(tLData *src1, tLData *src2, tLData *dest){

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
int substring(tLData *src1, tLData *src2, tLData *src3, tLData *dest){

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


//-------------------IPRECALL--------------------------------------------------
int precall(string *funcName, globalTS *GTS, struct Frame *newF){
  tGData* func;
  func = GtableSearch (GTS, funcName);
  if (!func->isdef){
    return DEFINE_ERROR;
  }
  newF = FrameCreate(GTS, funcName);
  if (newF == NULL){
    return INTERN_ERROR;
  }
  return SUCCESS;
}


//-------------------IPAR------------------------------------------------------
int parametr(tGData *source1, tLData *source2, struct Frame *newF, int cnt){
  int success;
  

  if (!source2->isinit){
    return UNINIT_ERROR;
  }

  //ziskani jmena parametru
  string *param;
  param = source1->params[cnt];
  //

  //hledani parametru v ramci  
  tLData *searched;
  searched = LtableSearch (newF->proms, param);
  //

  
  if (searched->varType != source2->varType){
    return TYPE_ERROR;
  }

  success = LtableInsertValue (newF->proms, param, source2->varValue);

  return success;
}


//-------------------ICALL-----------------------------------------------------
int call(tGData *source1, struct Frame *newF, TinstList *LOI, Tstackframe *S){
  int success;

  success = PushFrame(S, newF);
  if (success != SUCCESS){
    return success;
  }

  success = ListConect(LOI, source1->LInstr);
  if (success != SUCCESS){
    return success;
  }

  return SUCCESS;
}

//-------------------IRET------------------------------------------------------
int ret(tLData *source1, Tstackframe *S){
  int success;
  struct Frame *oldF;
  tLData *RET;

  string R;
  strInit(&R);
  strAddChar(&R, '#');
  strAddChar(&R, 'R');
  strAddChar(&R, 'E');
  strAddChar(&R, 'T');
  strAddChar(&R, 'U');
  strAddChar(&R, 'R');
  strAddChar(&R, 'N');

  oldF = PopFrame(S);

  RET = VariableSearch(S, &R);
  if (RET == NULL){
    success = FrameInsertVar(S->top, &R, source1->varType, source1->varValue); 
  }else{
    RET->varType = source1->varType;
    success = FrameInsertValue(S, &R, source1->varValue);
  }
  
  FrameDelete(oldF);
  return success;
}
