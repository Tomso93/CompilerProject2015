#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "define.h"
#include "str.h"
#include "errors.h"
#include "instlist.h"
#include "ial.h"
#include "lex.h"
#include "varframe.h"
#include "interpret.h"
#include "parser.h"

string attr;

int main(int argc, char *argv[]){
    //int token = 0;
    FILE * f;
    strInit(&attr);
    
    globalTS symtab;
    int result;
     
    GtableInit(&symtab);

    if(argc >= 2){
        f = fopen (argv[1],"rt");
        if(NULL == f){
                fprintf(stderr, "Soubor se nepodarilo otevrit!");
                return 99;
        } else {
            set_source_file(f);
            // volani parseru
            result = parse(&symtab);
            if (result != SUCCESS){
                printerror(result);
                fclose(f);
                return result;
            }
            result = interpret(&symtab);
            if (result != SUCCESS){
                printerror(result);
                fclose(f);
                return result;
            }

	}

    } else {
        fprintf(stderr, "Zadej soubor!");
        return 99;
    }
    return 0;
}
