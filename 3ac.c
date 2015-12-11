/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */

#include <stdio.h>
#include "str.h"
#include "stable.h"
#include "instlist.h"
#include "3ac.h"

int cnt = 1; // counter pro poÄŤĂ­tĂˇnĂ­ hodnot generovanĂ˝ch promÄ›nnĂ˝ch

// Funkce na generovĂˇnĂ­ novĂ© instrukce, kterĂˇ je nĂˇslednÄ› vloĹľena do seznamu instrukcĂ­
Tinst *genInstr(int InstType, string src1, string src2, string dest)
{
    Tinst TItem;
    TItem.itype = InstType;
    TItem.src1 = &src1;
    TItem.src2 = &src2;
    TItem.dest = &dest;
    return &TItem;
}

// Fuknce na generovĂˇnĂ­ novĂ˝ch promÄ›nnĂ˝ch
void GenNewVariable(string *item)
{
    strClear(item);
    strAddChar(item, '#');
    int i;
    i = cnt;
    
    while(i != 0)
    {
        strAddChar(item, (char)(i % 10 + '0'));
        i = i / 10;
    }
    cnt++;                             
}

string *ReadNameVar(TinstList *list)
{
    Tinst *I = ListGetLastInst(list);
    
    return I->dest;
}
