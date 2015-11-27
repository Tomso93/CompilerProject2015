/*
 *	errors.c
 *	zpracuje chybu a vypise chybove hlaseni
 *	Autor: Petr Zufan
 *	
 */

#include<stdio.h>
#include"errors.h"
#include"define.h"

void printerror(int errornumber){
  switch(errornumber){
    case LEXICAL_ERROR:
      fprintf(stderr, "Error %d: lexical error", errornumber);
      break;

    case SYNTAX_ERROR:
      fprintf(stderr, "Error %d: syntax error", errornumber);
      break;

    case DEFINE_ERROR:
      fprintf(stderr, "Error %d: undefine function or variable or redefining function", errornumber);
      break;

    case TYPE_ERROR:
      fprintf(stderr, "Error %d: type mismatch or wrong count of parametrs in function", errornumber);
      break;

    case CONVERT_ERROR:
      fprintf(stderr, "Error %d: converting variable type error", errornumber);
      break;

    case SEMANTIC_ERROR:
      fprintf(stderr, "Error %d: semantic error", errornumber);
      break;

    case READ_NUM_ERROR:
      fprintf(stderr, "Error %d: reading number error ", errornumber);
      break;

    case UNINIT_ERROR:
      fprintf(stderr, "Error %d: uninitialized variable", errornumber);
      break;

    case ZERO_DIV_ERROR:
      fprintf(stderr, "Error %d: devide by zero", errornumber);
      break;

    case RUNTIME_ERROR:
      fprintf(stderr, "Error %d: runtime error", errornumber);
      break;

    case INTERN_ERROR:
      fprintf(stderr, "Error %d: intern error. You are not a dumbass. Authors are. Have a nice day.", errornumber);
      break;

    case SUCCESS:
      break;

    default
      fprintf(stderr, "Error %d: intern error. You are not a dumbass. Authors are. Have a nice day.", errornumber);
      break;
  }
}
