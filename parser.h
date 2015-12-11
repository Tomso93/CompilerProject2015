/*
*	Projekt: Kompilator pro jazyk IFJ15=>podmnozina C++11
*	Autor: Tomas STRNKA & Martin WOJACZEK
*	Login: xstrnk01
*	Cast: Parser .c +.h
*	Verze: 1.4
*/



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
int parse(tSymbolTable *ST);

int program(tSymbolTable *global_table);

int func_dclr(tSymbolTable *global_table);

int param();

int param_n();

int select(tSymbolTable *global_table, string *id);

int body(tSymbolTable *global_table, string *id);

int stmnt(tSymbolTable *global_table, string *id);

int _if(tSymbolTable *global_table, string *id);

int _cin(tSymbolTable *global_table, string *id);

int _id_n(tSymbolTable *global_table, string *id);

int _cout(tSymbolTable *global_table, string *id);

int term_n(tSymbolTable *global_table, string *id);

int _for(tSymbolTable *global_table, string *id);

int _return(tSymbolTable *global_table, string *id);

int _prom(tSymbolTable *global_table, string *id);

int callf_dec(tSymbolTable *global_table, string *id);

int list_par();

int list_par_n();

int _i_prom(tSymbolTable *global_table, string *id);

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
