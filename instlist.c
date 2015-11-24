/*
 *	instlist.h
 *	list of instructions
 *	
 */

// typy instrukci - cisla 900 - 950

#define IMOV	900	//:=
#define IADD	901	//+
#define ISUB	902	//-
#define IMUL	903	//*
#define IDIV	904	//div
#define IEQUAL	905	//=
#define ISMALL	906	//<
#define IBIG	907	//>
#define IEQSM	908	//<=
#define IEQBG	909	//>=
#define INOTEQ	910	//!=
#define INOT	911	//not
#define IAND	912	//and
#define IOR	913	//or
#define ILABEL	914	//navesti
#define IGOTO	915	//skok
#define IIFGOTO	916	//podmineny skok
#define IREADLN	917	//read line
#define IWRITE	918	//write
#define IFIND	919	//find
#define ISORT	920	//sort
#define IEND	921	//konec programu
#define IREADI	922	//read int
#define IREADD	923	//read double
#define IREADS	924	//read string

//opravit ukaztel na tabulku symbolu
typedef struct {
	int itype; 		//typ instrukce
	struct Hashtable *src1;	//zdroj1
	struct Hashtable *src2;	//zdroj2
	struct Hashtable *dest;	//cil
}Tinst;

typedef struct listItem{
	Tinst inst;		   //aktualni instrukce
	struct listItem *nextInst; //ukazatel na následující instrukci
}TlistItem;

typedef struct {
	struct listItem *first;	//ukazatel na prvni instrukci
	struct listItem *last;	//ukazatel na posledni instrukci
	struct listItem *act;	//ukazatel na aktualni instrukci
}TinstList;

void ListInit(TinstList *L);
void ListFree(TinstList *L);
void ListInsert(TinstList *L, Tinst I);
void ListSucc(TinstList *L);
void ListGoto(TinstList *L, struct Hashtable *dest);
Tinst *ListGetInst(TinstList *L);

// Inicializase seznamu instrukcí
void ListInit(TinstList *L)
{
    L->first = NULL;
    L->last = NULL;
    L->act = NULL;    
}

// Funkce pro uvolnění seznamu instrukcí
void ListFree(TinstList *L)
{
    TlistItem *ptr;
    
    while(L->first != NULL )
    {
        ptr = L->first;
        L->first = L->first->next;
        free(ptr);
    }
}

// Funkce pro vložení položky na konec seznamu
void ListInsert(TinstList *L, Tinst I)
{
    TlistItem *item;
    item = malloc(sizeof(TlistItem));
    item->inst = I;
    item->nextInst = NULLL;
    
    if(L->first == NULL)
        L->first = item;
    else
        L->last->nextInst = item;
        
    L->last = item;
} 

Tinst *ListGetInst(TinstList *L)
{
    if(L->act == NULL)
    {
        //printf("Chyba!");
        return NULL;
    }
    else
        return &(L->act->inst);
}
