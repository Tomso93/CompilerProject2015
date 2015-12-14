/*
 *	ial.h
 *	Popis: hlavicka vestavenych funkci
 *	Autor: Petr Gibis, Petr Zufan
 *
 */


int length(string s);
string concat(string s1, string s2);
string substr(string s, int pos, int n);

int find(string *s, string *str);
void FailCreate(char *s, int delka, int fail[]);
string sort(string s);
void siftdown(char *str1, int left, int right);

//--------------------------------HASH---TABLE----------------------

//-----------lokalni tabulka symbolu------------------------------

typedef union {
	int i;
	double d;
	string s;
}Tvalue;

typedef struct
{
	int varType;  // typ dane promenne
	Tvalue varValue;
	bool isinit;
} tLData;

typedef struct LtableItem
{
	string key;                  // klic, podle ktereho se bude vyhledavat = nazev identifikatoru
	tLData data;                  // data, ktera jsou ke klici pridruzena
	struct LtableItem *nextItem;  // ukazatel na dalsi prvek tabulky symbolu
} tLTableItem;

typedef tLTableItem *localTS[TABLE_SIZE];


//----------globalni tabulka symbolu-----------------------------------

typedef struct
{
	int funcType;
	bool isdef;
	string *params[TABLE_SIZE];
	localTS *LTable;
	TinstList *LInstr;
} tGData;

typedef struct GtableItem
{
	string key;  // klic, podle ktereho se bude vyhledavat = nazev identifikatoru
	tGData data;                  // data, ktera jsou ke klici pridruzena
	struct GtableItem *nextItem;  // ukazatel na dalsi prvek tabulky symbolu
} tGTableItem;

typedef tGTableItem *globalTS[TABLE_SIZE];


//------------------hashovaci funkce-------------------------------------
//vraci index do tabulky symbolu
int hash(string *key);

//-----------------funkce pro praci s tabukou smybolu------------------------
//funkce vraci int: 0 = uspech, 1-99 = neuspech (cislo chyby)
//funkce vraci ukazatel: platny ukazatel = uspech, NULL = neuspech 


//----------------------globalni------------------------------------------
int GtableInit(globalTS *T);
int GtableFree(globalTS *T);

//vlozi funkci bez parametru, promennych a instrukci. (isdef == false)
//Pro vlozeni parametru pouzij funkci GtableInsertParam. 
//Pro vlozeni promenne pouzij funkci GtableInsertVar.
//Pro vlozeni hodnoty do promenne pouzij funkci GtableInsertVarVal.
//Pro vlozeni instrukce pouzij funkci GtableinsertInstr. 
int GtableInsert(globalTS *T, string *key, int funcType);

tGData* GtableSearch(globalTS *T, string *key);
int GtableInsertParam(globalTS *T, string *funcID, string *parID, int parType);
int GtableInsertVar(globalTS *T, string *funcID, string *varID, int varType);
int GtableInsertVarVal(globalTS *T, string *funcID, string *varID, Tvalue v);
int GtableInsertInstr(globalTS *T, string *funcID, Tinst *instrukce);
//int GtableItemDelete (globalTS *T, string *key); jestli bude potreba, dodelam
//tGData* GtableRead (globalTS *T, string key); jestli bude potreba, dodelam



//-----------------lokalni---------------------------------------
int LtableInit(localTS *T);
int LtableFree(localTS *T);

//vlozi promennou bez hodnoty (isinit = false).
//Pro nasledne vlozeni hodnoty pouzij LtableInsertValue.
int LtableInsert(localTS *T, string *key, int varType);

tLData* LtableSearch(localTS *T, string *key);
int LtableInsertValue(localTS *T, string *key, Tvalue v);
//int LtableItemDelete (localTS *T, string *key); jestli bude potreba, dodelam
//tLData* LtableRead (localTS *T, string key); jestli bude potreba, dodelam

int GtablePrintVars(globalTS *T, string *funcID);
int GtablePrintVarsAll(globalTS *T);
int GTablePrintInst(globalTS *T, string* funcID);

