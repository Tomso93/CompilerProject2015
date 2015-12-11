#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "define.h"
#include "str.h"
#include "errors.h"
#include "instlist.h"
#include "stable.h"
#include "lex.h"
#include "interpret.h"
#include "parser.h"

string attr;

int main(int argc, char *argv[]){
    //int token = 0;
    FILE * f;
    strInit(&attr);
    
    tSymbolTable symtab;
    TinstList instlist;
    int result;
     
    ListInit(&instlist);
    tableInit(&symtab);

    if(argc >= 2){
        f = fopen (argv[1],"rt");
        if(NULL == f){
                fprintf(stderr, "Soubor se nepodarilo otevrit!");
                return 99;
        } else {
            // volání parseru
            result = parse(&symtab, &instlist);
            if (result != SUCCESS){
                printerror(result);
                return result;
                fclose(f);
            }
            result = interpret(&instlist);
            if (result != SUCCESS){
                printerror(result);
                return result;
            }
        }

    } else {
        fprintf(stderr, "Zadej soubor!");
        return 99;
    }
    return 0;
}
