// definice jednotlivych typu

typedef union {
  int i;
  double d;
  string s;
}Tvalue;

typedef struct
{
  int varType;  // typ dane promenne
  Tvalue varValue;
} tData;

typedef struct tableItem
{
  string key;                  // klic, podle ktereho se bude vyhledavat = nazev identifikatoru
  tData data;                  // data, ktera jsou ke klici pridruzena
  struct tableItem *nextItem;  // ukazatel na dalsi prvek tabulky symbolu
} tTableItem;

typedef struct
{
  struct tableItem *first;
} tSymbolTable;

void tableInit(tSymbolTable *T);
tTableItem* tableInsert(tSymbolTable *T, string *key, int varType);
tData* tableSearch(tSymbolTable *T, string *key);
tData* tableRead ( tSymbolTable *T, string *key );
void tableItemDelete ( tSymbolTable *T, string *key );
void tableFree(tSymbolTable *T);

int tableInsertValue (tSymbolTable *T, string *key, Tvalue v);
