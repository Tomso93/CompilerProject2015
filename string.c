/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */


#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define LENGHT_STRING 8
#define ERROR 0
#define OK 1

typedef struct {
    char *str;
    int lenght;  // délka
    int size; // velikost alokované paměti 
}str;

void strFree(str *s);
void stringClean(str *s);
int stringAddItem(str *s, char item);

void strFree(str *s)
{
    free(s->str);
}

void stringClean(str *s)
{
    s->str[0] = '0';
    s->lenght = 0; 
}

int stringAddItem(str *s, char item)
{
    if(s->lenght + 1 >= s->size)
    {
        if((s->str = (char*) realloc(s->str, s->length + LENGHT_STRING)) == NULL)
        {
            return ERROR;
        }
        s->size = s->lenght + LENGHT_STRING;
    }
    
    s->str[s->length] = item;
    s->length++;
    s->str[s->length] = '\0';
    return OK;
}
