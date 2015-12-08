/*
 *	instlist.h
 *	seznam instrukci
 *	Autor: Petr Zufan, Martin Wojaczek
 *	
 */


typedef struct {
	int itype; 	//typ instrukce
	string *src1;	//zdroj1
	string *src2;	//zdroj2
	string *dest;	//cil
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
int ListGoto(TinstList *L, string *dest);
Tinst *ListGetInst(TinstList *L);
Tinst *ListGetLastInst(TinstList *L);
int ListActLast(TinstList *L);
int ListActFirst(TinstList *L);
