/*
 *	errors.c
 *	zpracuje chybu a vypise chybove hlaseni
 *	Autor: Petr Zufan
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

void printerror(int errornumber){
  switch(errornumber){
    case LEXICAL_ERROR:
      fprintf(stderr, "Error %d: lexical error\n", errornumber);
      break;

    case SYNTAX_ERROR:
      fprintf(stderr, "Error %d: syntax error\n", errornumber);
      break;

    case DEFINE_ERROR:
      fprintf(stderr, "Error %d: undefine function or variable or redefining function\n", errornumber);
      break;

    case TYPE_ERROR:
      fprintf(stderr, "Error %d: type mismatch or wrong count of parametrs in function\n", errornumber);
      break;

    case CONVERT_ERROR:
      fprintf(stderr, "Error %d: converting variable type error\n", errornumber);
      break;

    case SEMANTIC_ERROR:
      fprintf(stderr, "Error %d: semantic error\n", errornumber);
      break;

    case READ_NUM_ERROR:
      fprintf(stderr, "Error %d: reading number error\n", errornumber);
      break;

    case UNINIT_ERROR:
      fprintf(stderr, "Error %d: uninitialized variable\n", errornumber);
      break;

    case ZERO_DIV_ERROR:
      fprintf(stderr, "Error %d: devide by zero\n", errornumber);
      break;

    case RUNTIME_ERROR:
      fprintf(stderr, "Error %d: runtime error\n", errornumber);
      break;

    case INTERN_ERROR:
      fprintf(stderr, "Error %d: intern error\n", errornumber);
      break;

    case SUCCESS:
      break;

    default:
      fprintf(stderr, "Error %d: unknown error\n", errornumber);
      break;
  }
}
