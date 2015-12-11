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
int parse(globalTS *ST);

int program(globalTS *global_table);

int func_dclr(globalTS *global_table);

int param();

int param_n();

int select(globalTS *global_table, string *id);

int body(globalTS *global_table, string *id);

int stmnt(globalTS *global_table, string *id);

int _if(globalTS *global_table, string *id);

int _cin(globalTS *global_table, string *id);

int _id_n(globalTS *global_table, string *id);

int _cout(globalTS *global_table, string *id);

int term_n(globalTS *global_table, string *id);

int _for(globalTS *global_table, string *id);

int _return(globalTS *global_table, string *id);

int _prom(globalTS *global_table, string *id);

int callf_dec(globalTS *global_table, string *id);

int list_par();

int list_par_n();

int _i_prom(globalTS *global_table, string *id);

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

int SReduction_expr(Tstack* St, int i, globalTS *global_table, string *id);

int comp_expr(globalTS *global_table, string *id);

string TopT(Tstack* St);
