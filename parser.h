/*
*	Projekt: Kompilator pro jazyk IFJ15=>podmnozina C++11
*	Autor: Tomas STRNKA & Martin WOJACZEK
*	Login: xstrnk01
*	Cast: Parser .c +.h
*	Verze: 1.4
*/
#define MAX 20


// Strunktura zasobniku pro vkladani terminalu na zasobnik
typedef struct {
	string pom[MAX];	// text
	char prom_val[MAX]; // type promenne
	int top;			// vrchol
	char t_n[MAX];		// T , N 
	double *val[MAX];	// hodnota 
} Tstack;



// Rekurzivni sestup, deklr
// Vstup parseru
int parse(tSymbolTable *ST, TinstList *instrList);

int program(TinstList *instrList);

int func_dclr(TinstList *instrList);

int param();

int param_n();

int select(TinstList *instrList);

int body(TinstList *instrList);

int stmnt(TinstList *instrList);

int _if(TinstList *instrList);

int _cin(TinstList *instrList);

int _id_n(TinstList *instrList);

int _cout(TinstList *instrList);

int term_n(TinstList *instrList);

int _for(TinstList *instrList);

int _return(TinstList *instrList);

int _prom(TinstList *instrList);

int callf_dec(TinstList *instrList);

int list_par();

int list_par_n();

int _i_prom(TinstList *instrList);

int type();

int term();




// Precedence 
int select_ruler(string *zas_term, int term);

void SDipose(Tstack* St);

void InitialSt(Tstack* St);

void PushE(Tstack *St, char Type, void * data);

int FindBrc(Tstack *St);

int TPush(Tstack *St);

void DelI(Tstack* St, int n);

int FindT(Tstack* St);

int SReduction_expr(Tstack* St, int i, TinstList *instrList);

int comp_expr(TinstList *instrList);
