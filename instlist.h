/*
 *	instlist.h
 *	seznam instrukci
 *	Autor: Petr Zufan, Martin Wojaczek
 *	
 */


typedef struct {
	int itype; 	//typ instrukce
	tData *src1;	//zdroj1
	tData *src2;	//zdroj2
	tData *dest;	//cil
}Tinst;

typedef struct listItem{
	Tinst inst;		   //aktualni instrukce
	struct listItem *nextInst; //ukazatel na nasledujici instrukci
}TlistItem;

typedef struct {
	struct listItem *first;	//ukazatel na prvni instrukci
	struct listItem *last;	//ukazatel na posledni instrukci
	struct listItem *act;	//ukazatel na aktualni instrukci
}TinstList;

int ListInit(TinstList *L);
int ListDispose(TinstList *L);
int ListInsert(TinstList *L, Tinst I);
int ListSucc(TinstList *L);
int ListGoto(TinstList *L, tData *dest);
Tinst *ListGetInst(TinstList *L);
Tinst *ListGetLastInst(TinstList *L);
