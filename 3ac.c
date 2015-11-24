/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */

#include <stdio.h>

int cnt = 1; // counter pro počítání hodnot generovaných proměnných

// Funkce na generování nové instrukce, která je následně vložena do seznamu instrukcí
void genInstr(int InstType, void *src1, void *src2, void *dest)
{
    Tinst TItem;
    TItem.itype = instType;
    TItem.src1 = src1;
    TItem.src2 = src2;
    TItem.dest = dest;
    ListInsert(list, TItem); 
}

// Fuknce na generování nových proměnných
void GenNewVariable(str *item)
{
    stringClean(item);
    stringAddItem(item, '#')
    int i;
    i = cnt;
    
    while(i != 0)
    {
        stringAddItem(item, char (i % 10 + '0')
        i = i / 10;
    }
    cnt++;                             
}
