#include <stdio.h>
#include <stdlib.h>
#include "str.h"
#include "lex.h"
#include "define.h"

string attr;

int main(int argc, char *argv[]){
    //int token = 0;
    FILE * f;
    strInit(&attr);

    if(argc >= 2){
        f = fopen (argv[1],"rt");
        if(NULL == f){
                fprintf(stderr, "Soubor se nepodarilo otevrit!");
                return 99;
        } else {
            // vol�n� parseru
            }

    } else {
        fprintf(stderr, "Zadej soubor!");
        return 99;
    }
    return 0;
}
