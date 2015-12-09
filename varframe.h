/*
 *	varframe.h
 *	deklarace zasobniku ramce promennych
 *	Autor: Petr Zufan
 *	
 */

typedef struct Frame {
  localTS *proms;		//ukazatel na kopii lokalni tabulky
  bool base			//flag zda je zakladnim ramcem
  struct Frame *down;		//ukazatel na dalsi ramec v zasobniku
}Tframe


//deklarace zasobniku ramcu
typedef struct {
  struct Frame *top;		//ukazatel na vrchol zasobniku
  struct Frame *act;		//ukazatel na ramec, se kterym se pracuje
}Tstackframe



//inicializace zasobniku
int StackInit(Tstackframe *S);

//uvolneni zasobniku z pameti
int StackDispose(Tstackframe *S);

//vynda ramec ze zasobniku a vrati na nej uazatel
struct Frame *PopFrame(Tstackframe *S);

//vlozi ramec na vrchol zasobniku
int PushFrame(Tstackframe *S, struct Frame *F);

//odstrani ramec (ktery uz neni na zasobniku)
int FrameDelete(struct Frame *F);

//vytvori ramec (ale neda ho na zasobnik)	
struct Frame *FrameCreate(globalTS *GTS, string funcName);

//vyhleda promennou v zasobniku (nejhloubeji v zakladnim ramci)
tLData *VariableSearch(TstackFrame *S, string varName);

//vlozi hodnotu do promenne
int FrameInsertValue(TstackFrame *S, string varName, Tvalue val);

//vlozi promennou do ramce
int FrameInsertVar(struct Frame *F, string varName, int varType, Tvalue varVal);
