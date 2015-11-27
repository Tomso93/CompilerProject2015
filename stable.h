// definice jednotlivych typu

typedef struct
{
  string varType;  // typ dane promenne
  string varValue;
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
tTableItem* tableInsert(tSymbolTable *T, string *key, string varType);
tData* tableSearch(tSymbolTable *T, string *key);
tData* tableRead ( tSymbolTable *T, string *key );
void tableItemDelete ( tSymbolTable *T, string *key );
void tableFree(tSymbolTable *T);
