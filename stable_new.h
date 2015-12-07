

//lokalni tabulka symbolu

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

struct LtableItem *localTS[30];


//globalni tabulka symbolu

typedef struct
{
  int funcType;
  bool isdef
  localTS *params;
  localTS *LTable;
  TinstList *LInstr;
} tGData;

typedef struct GtableItem
{
  string key;                  // klic, podle ktereho se bude vyhledavat = nazev identifikatoru
  tGData data;                  // data, ktera jsou ke klici pridruzena
  struct GtableItem *nextItem;  // ukazatel na dalsi prvek tabulky symbolu
} tGTableItem;

struct GtableItem *globalTS[30];


//hashovaci funkce
//vraci index do tabulky symbolu
int hash(string *key);

//funkce pro praci s tabukou smybolu
//funkce vraci int: 0 = uspech, 1-99 = neuspech (cislo chyby)
//funkce vraci ukazatel: platny ukazatel = uspech, NULL = neuspech 

//globalni
int GtableInit (globalTS *T);
int GtableFree (globalTS *T);
int GtableInsert (globalTS *T, string *key, int varType);
tGData* GtableSearch (globalTS *T, string *key);
int GtableItemDelete (globalTS *T, string *key);
tGData* GtableRead (globalTS *T, string *key);
int GtableInsertValue (globalTS *T, string *key, Tvalue v);

//lokalni
int LtableInit (localTS *T);
int LtableFree (localTS *T);
int LtableInsert (localTS *T, string *key, int varType);
tLData* LtableSearch (localTS *T, string *key);
int LtableItemDelete (localTS *T, string *key);
tLData* LtableRead (localTS *T, string *key);
int LtableInsertValue (localTS *T, string *key, Tvalue v);

