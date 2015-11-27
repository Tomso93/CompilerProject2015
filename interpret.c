/*
 *	interpret.c
 *	interpretace ridiciho programu
 *	Autor: Petr Zufan
 *	
 */

#include <stdio.h>
#include "instlist.h" //seznamem instrukci
#include "interpret.h"
#include <string.h>
#include "str.h" //soubor od Martin Kvapil. funkce pro praci s retezci
#include "ial.h" //funkce find a sort a hashovaci tabulka
#include "stable.h" //tabulka symbolu
#include "define.h" //definice konstant
#include "errors.h" 


int interpret (TinstList *LOI){
  int success = SUCCESS;
  Tinst *instr;  

  instr = ListGetInstr(LOI);
  if (instr == NULL){
    printerror(INTERN_ERROR);
    return INTERN_ERROR;
  }

  while ( !(inst->itype == IEND) ){
    switch (inst->itype){

      case IMOV:
        success = move(inst->src1, inst->dest);
        break;

      case IADD:
        success = addition(inst->src1, inst->src2, inst->dest);
        break;

      case ISUB:
        success = substraction(inst->src1, inst->src2, inst->dest);
        break;

      case IMUL:
        success = multiplication(inst->src1, inst->src2, inst->dest);
        break;

      case IDIV:
        success = division(inst->src1, inst->src2, inst->dest);
        break;

      case IEQUAL:
        success = equal(inst->src1, inst->src2, inst->dest);
        break;

      case ISMALL:
        success = smaller(inst->src1, inst->src2, inst->dest);
        break;

      case IBIG:
        success = bigger(inst->src1, inst->src2, inst->dest);
        break;

      case IEQSM:
        success = equalsmaller(inst->src1, inst->src2, inst->dest);
        if (success){
          return INTERPRET_ERROR;
        }
        break;

      case IEQBG:
        success = equalbigger(inst->src1, inst->src2, inst->dest);
        break;

      case INOTEQ:
        success = notequal(inst->src1, inst->src2, inst->dest);
        break;

      case INOT:
        success = negation(inst->src1, inst->dest);
        break;
/*
      case IAND:
        success = konjunction(inst->src1, inst->src2, inst->dest);
        break;

      case IOR:
        success = disjunction(inst->src1, inst->src2, inst->dest);
        break;
*/
      case ILABEL:
        break;

      case IGOTO:
        success = jump(inst->dest);
        break;

      case IIFGOTO:
        success = jumpif(inst->src1, inst->dest);
        break;

      case IREADI:
        success = readint(inst->dest);
        break;

      case IREADD:  
        success = readdouble(inst->dest);
        break;

      case IREADS:  
        success = readstring(inst->dest);
        break;

      case IREAD: 
        success = read(inst->dest);
        break;

      case IWRITE:  
        success = write(inst->src1);
        break;

      case IFIND:
        success = findme(inst->src1, inst->src2, inst->dest);
        break;

      case ISORT:
        success = sortme(inst->src1, inst->dest);
        break;

      case ICAT:
        success = concatenate(inst->src1, inst->src2, inst->dest);
        break;

      case ILENGTH:
        success = lengthstring(inst->src1, inst->dest);
        break;
/*
      case ISUBSTR:
        success = substring(inst->src1, inst->src2, inst->dest);
        break;
*/
      default:
        success = INTERN_ERROR;
        break;
    }

    if (!success){
      ListDespose(LOI);
      printerror(success);
      return success;
    }
    ListSucc(LOI);
  }
  ListDespose(LOI);
  printerror(success)
  return success;
}

//-------------------IMOV------------------------------------------------------

int move(Tdata *src1, Tdata *dest){

  if ( (src1->varType == TOK_INT) && (dest->varType == TOK_INT) ){
    dest->varValue.i = src1->varValue.i;
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.d = src1->varValue.d;
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)src1->varValue.d;
    return SUCCESS;
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.d = (double)src1->varValue.i;
    return SUCCESS;
  }else if ( (src1->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
    if (strCopyString(&(dest->varValue.s), &(src1->varValue.s)) == STR_ERROR){
      return RUNTIME_ERROR;
    }else{
      return SUCCESS;
    }
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IADD------------------------------------------------------

int addition(Tdata *src1, Tdata *src2, Tdata *dest){

  if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = src1->varValue.i + src2->varValue.i;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)(src1->varValue.i + src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d + (double)src2->varValue.i);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (src1->varValue.d + (double)src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)((double)src1->varValue.i + src2->varValue.d;)
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)src1->varValue.i + src2->varValue.d;
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d + src2->varValue.d);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = src1->varValue.d + src2->varValue.d;
       return SUCCESS;
     }
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;

}

//-------------------ISUB------------------------------------------------------
int substraction(Tdata *src1, Tdata *src2, Tdata *dest){
  
  if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = src1->varValue.i - src2->varValue.i;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)(src1->varValue.i - src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d - (double)src2->varValue.i);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (src1->varValue.d - (double)src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)((double)src1->varValue.i - src2->varValue.d;)
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)src1->varValue.i - src2->varValue.d;
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d - src2->varValue.d);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = src1->varValue.d - src2->varValue.d;
       return SUCCESS;
     }
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IMUL------------------------------------------------------
int multiplication(Tdata *src1, Tdata *src2, Tdata *dest){

  if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = src1->varValue.i * src2->varValue.i;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)(src1->varValue.i * src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d * (double)src2->varValue.i);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (src1->varValue.d * (double)src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)((double)src1->varValue.i * src2->varValue.d;)
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)src1->varValue.i * src2->varValue.d;
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
     if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d * src2->varValue.d);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = src1->varValue.d * src2->varValue.d;
       return SUCCESS;
     }
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IDIV------------------------------------------------------
int division(Tdata *src1, Tdata *src2, Tdata *dest){
  if ( (src1->varType == TOK_INT) && (src2->varType == TOK_INT) ){
     if (src2->varValue.i == 0){
       return ZERO_DIV_ERROR;
     }else if (dest->varType == TOK_INT){
       dest->varValue.i = src1->varValue.i % src2->varValue.i;
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)(src1->varValue.i % src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_INT) ){
     if (src2->varValue.i == 0){
       return ZERO_DIV_ERROR;
     }else if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d / (double)src2->varValue.i);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (src1->varValue.d / (double)src2->varValue.i);
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_INT) && (src2->varType == TOK_DOUBLE) ){
     if (src2->varValue.d == 0.0){
       return ZERO_DIV_ERROR;
     }else if (dest->varType == TOK_INT){
       dest->varValue.i = (int)((double)src1->varValue.i / src2->varValue.d;)
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = (double)src1->varValue.i / src2->varValue.d;
       return SUCCESS;
     }

  }else if ( (src1->varType == TOK_DOUBLE) && (src2->varType == TOK_DOUBLE) ){
     if (src2->varValue.d == 0.0){
       return ZERO_DIV_ERROR;
     }else if (dest->varType == TOK_INT){
       dest->varValue.i = (int)(src1->varValue.d / src2->varValue.d);
       return SUCCESS;
     }else if (dest->varType == TOK_DOUBLE){
       dest->varValue.d = src1->varValue.d / src2->varValue.d;
       return SUCCESS;
     }
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IEQUAL----------------------------------------------------
int equal(Tdata *src1, Tdata *src2, Tdata *dest){

  if (dest->varType != TOK_INT){
    if (dest->varType == TOK_DOUBLE){
      dest->varType = TOK_INT;
    }else{
      return TYPE_ERROR;
    }
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.i == src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)(src1->varValue.d == src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.d == (double)src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)((double)src1->varValue.i == src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
  dest->varValue.i = (int)(strCmpString(&(src1->varValue.s), &(src2->varValue.s)) == 0);
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------ISMALL----------------------------------------------------
int smaller(Tdata *src1, Tdata *src2, Tdata *dest){

  if (dest->varType != TOK_INT){
    if (dest->varType == TOK_DOUBLE){
      dest->varType = TOK_INT;
    }else{
      return TYPE_ERROR;
    }
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.i < src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)(src1->varValue.d < src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.d < (double)src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)((double)src1->varValue.i < src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
  dest->varValue.i = (int)(strCmpString(&(src1->varValue.s), &(src2->varValue.s)) < 0);
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IBIG------------------------------------------------------
int bigger(Tdata *src1, Tdata *src2, Tdata *dest){

  if (dest->varType != TOK_INT){
    if (dest->varType == TOK_DOUBLE){
      dest->varType = TOK_INT;
    }else{
      return TYPE_ERROR;
    }
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.i > src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)(src1->varValue.d > src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_INT) ){
    dest->varVlue.i = (int)(src1->varValue.d > (double)src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)((double)src1->varValue.i > src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
  dest->varValue.i = (int)(strCmpString(&(src1->varValue.s), &(src2->varValue.s)) > 0);
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IEQSM-----------------------------------------------------
int equalsmaller(Tdata *src1, Tdata *src2, Tdata *dest){

  if (dest->varType != TOK_INT){
    if (dest->varType == TOK_DOUBLE){
      dest->varType = TOK_INT;
    }else{
      return TYPE_ERROR;
    }
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.i <= src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)(src1->varValue.d <= src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.d <= (double)src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)((double)src1->varValue.i <= src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
  dest->varValue.i = (int)(strCmpString(&(src1->varValue.s), &(src2->varValue.s)) <= 0);
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IEQBG-----------------------------------------------------
int equalbigger(Tdata *src1, Tdata *src2, Tdata *dest){

  if (dest->varType != TOK_INT){
    if (dest->varType == TOK_DOUBLE){
      dest->varType = TOK_INT;
    }else{
      return TYPE_ERROR;
    }
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.i >= src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)(src1->varValue.d >= src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.d >= (double)src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)((double)src1->varValue.i >= src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
  dest->varValue.i = (int)(strCmpString(&(src1->varValue.s), &(src2->varValue.s)) >= 0);
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------INOTEQ----------------------------------------------------
int notequal(Tdata *src1, Tdata *src2, Tdata *dest){

  if (dest->varType != TOK_INT){
    if (dest->varType == TOK_DOUBLE){
      dest->varType = TOK_INT;
    }else{
      return TYPE_ERROR;
    }
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_INT) ){
    dest->varValue.i = (int)(src1->varValue.i != src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)(src1->varValue.d != src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_DOUBLE) && (dest->varType == ) ){
    dest->varValue.i = (int)(src1->varValue.d != (double)src2->varValue.i);
    return SUCCESS;
  }else if ( (src1->varType == TOK_INT) && (dest->varType == TOK_DOUBLE) ){
    dest->varValue.i = (int)((double)src1->varValue.i != src2->varValue.d);
    return SUCCESS;
  }else if ( (src1->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
  dest->varValue.i = (int)(strCmpString(&(src1->varValue.s), &(src2->varValue.s)) != 0);
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------INOT------------------------------------------------------
int negation(Tdata *src1, Tdata *dest){

  if (dest->varType != TOK_INT){
    if (dest->varType == TOK_DOUBLE){
      dest->varType = ;
    }else{
      return TYPE_ERROR;
    }else if (src1->varType == TOK_INT){
      dest->varValue.i = (int)(src1->varValue.i == 0);
      return SUCCESS;
    }else if (src1->varType == TOK_DOUBLE){
      dest->varValue.i = (int)((int)src1->varValue.d == 0);
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }
  }
  return INTERN_ERROR;
}

//-------------------IGOTO-----------------------------------------------------
int jump(Tdata *dest){ 
  return ListGoto(LOI, dest);
}

//-------------------IIFGOTO---------------------------------------------------
int jumpif(Tdata *src1, Tdata *dest){
  if (*src1) {
    return ListGoto(LOI, dest);
  }else{
    return SUCCESS;
  }
  return INTERN_ERROR;
}

//-------------------IREADI----------------------------------------------------
int readint(Tdata *dest){ 
  if (dest->varType != TOK_INT){
    return TYPE_ERROR;
  }else if (scanf("%d", &(dest->varValue.i)) < 0){
    return READ_NUM_ERROR;
  }
  return SUCCESS;
}

//-------------------IREADD----------------------------------------------------
int readdouble(Tdata *dest){ 
  if (dest->varType != TOK_DOUBLE){
    return TYPE_ERROR;
  }else if (scanf("%g", &(dest->varValue.d)) < 0){
    return READ_NUM_ERROR;
  }
  return SUCCESS;
}

//-------------------IREADS----------------------------------------------------
int readstring(Tdata *dest){
  char c;
 
  if (dest->varType != TOK_STRING){
    return TYPE_ERROR;
  }

  c=getchar();
  while ((c == '\n') && (c == '\t') && (c == '\\')){
    c=getchar();
  }

  while((c != '\n') && (c != '\t') && (c != '\\') && (c != EOF) && (c != ';')){
    if ( strAddChar(&(dest->varValue.s), c) == 1 ){
      return INTERN_ERROR;
    }
    c=getchar();
  }
  return SUCCESS;
}

//-------------------IREAD----------------------------------------------------
int read(Tdata *dest){ 
  if (dest->varType == TOK_INT){
    return readint(dest);
  }else if (dest->varType == TOK_INT){
    return readdouble(dest);
  }else if (dest->varType == TOK_STRING){
    return readstring(dest);
  }else{
    return INTERN_ERROR;
  }
}

//-------------------IWRITE----------------------------------------------------
int write(Tdata *src1){ 
  if (dest->varType == TOK_DOUBLE){
     if (printf("%g", src1->varValue.d) < 0){
       return RUNTIME_ERROR;
     }else{
       return SUCCESS;
     }

  }else if (dest->varType == TOK_INT){
    if (printf("%d", src1->varValue.i) < 0){
       return RUNTIME_ERROR;
     }else{
       return SUCCESS;
     }

  }else if (dest->varType == TOK_STRING){
    if (printf("%s", src1->varValue.s.str) < 0){
       return RUNTIME_ERROR;
     }else{
       return SUCCESS;
     }
  return INTERN_ERROR;
}

//-------------------ISORT-----------------------------------------------------
int sortme(Tdata *src1, Tdata *dest){
  if ( (dest->varType == TOK_STRING) && (src1->varType == TOK_STRING) ){
    dest->varValue.s = sort(src1->varValue.s);
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------IFIND----------------------------------------------------
int findme(Tdata *src1, Tdata *src2, Tdata *dest){
  if ( (src1->varType == TOK_STRING) && (src2->varType == TOK_STRING) ){
    if (dest->varType == TOK_INT){
      dest->varValue.i = find(src1->varValue.s, src2->varValue.s)
      return SUCCESS;
    }else if (dest->varType == TOK_DOUBLE){
      dest->varValue.d = (double)find(src1->varValue.s, src2->varValue.s)
      return SUCCESS;
    }else{
      return TYPE_ERROR;
    }
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------ILENGTH-----------------------------------------------------
int lengthstring(Tdata *src1, Tdata *dest){
  if ( (dest->varType == TOK_INT) && (src1->varType == TOK_STRING) ){
    dest->varValue.i = strGetLength(&(src1->varValue.s));
    return SUCCESS;
  }else if ( (dest->varType == TOK_DOUBLE) && (src1->varType == TOK_STRING) ){
    dest->varValue.d = (double)strGetLength(&(src1->varValue.s));
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}

//-------------------ICAT-----------------------------------------------------
int concatenate(Tdata *src1, Tdata *src2, Tdata *dest){
  if ( (src1->varType == TOK_STRING) && (src2->varType == TOK_STRING) && (dest->varType == TOK_STRING) ){
    dest->varValue.s = concat(src1->varValue.s, src2->varValue.s)
    return SUCCESS;
  }else{
    return TYPE_ERROR;
  }
  return INTERN_ERROR;
}
  
