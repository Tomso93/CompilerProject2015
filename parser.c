/*
*	Projekt: Kompilator pro jazyk IFJ15=>podmnozina C++11
*	Autor: Tomas STRNKA & Martin WOJACZEK
*	Login: xstrnk01
*	Cast: Parser .c +.h
*	Verze: 1.4 
*/

#include <stdio.h>
#include <stdbool.h>	
#include <malloc.h>
#include "define.h"		// instrukce, tokeny, err
#include "str.h"		// retezce
#include "lex.h"		// getNextToken()
#include "stable.h"		// tabulka symbolu
#include "instlist.h"	// seznam instukci
#include "3ac.h"		// genInstr, GenNewVariable


int token; //Globalni promennna pro pradavani tokenu 
string attr; //retezec tokenu
TinstList *list;	//instrukcni paska
int error;



//--------------------------------------------------------------------------------//
//					PRECEDENCNI SYNTAKTICKY ANALYZATOR							  //
//--------------------------------------------------------------------------------//																				

char prec_table[][14]={
{'>','>','<','<','>','>','>','>','>','>','<','>','<','>'},
{'>','>','<','<','>','>','>','>','>','>','<','>','<','>'},
{'>','>','>','>','>','>','>','>','>','>','<','>','<','>'},
{'>','>','>','>','>','>','>','>','>','>','<','>','<','>'},
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>'},
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>'},
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>'},
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>'},
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>'},
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>'},
{'<','<','<','<','<','<','<','<','<','<','=','<','<','0'},
{'>','>','>','>','>','>','>','>','>','>','0','>','0','>'},
{'>','>','>','>','>','>','>','>','>','>','0','>','0','>'},
{'<','<','<','<','<','<','<','<','<','<','<','0','<','0'},
};
//--hleda--rade/sloupec, aby mohl potom rozhodnout---------------------------------
int select_ruler(string *zas_term, int term){
	int radek, sloupec;
	
	//radek--jeste neni reseno, ze by bylo =<
	if (strCmpConstStr(zas_term, "*")==0  || strCmpConstStr(zas_term, "*<") == 0)  radek=0;
	
	if (strCmpConstStr(zas_term, "/")==0  || strCmpConstStr(zas_term, "/<") == 0) radek=1;
	
	if (strCmpConstStr(zas_term, "+")==0  || strCmpConstStr(zas_term, "+<") == 0) radek=2;
	
	if (strCmpConstStr(zas_term, "-")==0  || strCmpConstStr(zas_term, "-<") == 0) radek=3;
	
	if (strCmpConstStr(zas_term, "<")==0  || strCmpConstStr(zas_term, "<<") == 0) radek=4;

	if (strCmpConstStr(zas_term, ">")==0  || strCmpConstStr(zas_term, "><") == 0) radek=5;

	if (strCmpConstStr(zas_term, "<=")==0 || strCmpConstStr(zas_term, "<=<") == 0) radek=6;

	if (strCmpConstStr(zas_term, ">=")==0 || strCmpConstStr(zas_term, ">=<") == 0) radek=7;

	if (strCmpConstStr(zas_term, "==")==0 || strCmpConstStr(zas_term, "==<") == 0) radek=8;

	if (strCmpConstStr(zas_term, "!=")==0 || strCmpConstStr(zas_term, "!=<") == 0) radek=9;

	if (strCmpConstStr(zas_term, "(")==0  || strCmpConstStr(zas_term, "(<") == 0) radek=10;

	if (strCmpConstStr(zas_term, ")")==0  || strCmpConstStr(zas_term, ")<") == 0) radek=11;

	if (strCmpConstStr(zas_term, "id")==0 || strCmpConstStr(zas_term, "id<") == 0) radek=12;

	if (strCmpConstStr(zas_term, "$")==0  || strCmpConstStr(zas_term, "$<") == 0) radek=13;

	//sloupec
	switch (term){
		case TOK_ADDITION:
			sloupec=0;
			break;
		case TOK_SUBTRACTION:
			sloupec=1;
			break;
		case TOK_MULTIPLICATION:
			sloupec=2;
			break;
		case TOK_DIVISION:
			sloupec=3;
			break;
		case TOK_LESS_THAN:
			sloupec=4;
			break;
		case TOK_GREATER_THAN:
			sloupec=5;
			break;
		case TOK_GREATER_THAN_OR_EQUAL:
			sloupec=6;
			break;
		case TOK_LESS_THAN_OR_EQUAL:
			sloupec=7;
			break;
		case TOK_COMPARISON: 
			sloupec=8;
			break;
		case TOK_INEQUALITY:
			sloupec=9;
			break;
		case TOK_LEFT_BRACKET:
			sloupec=10;
			break;
		case TOK_RIGHT_BRACKET:
			sloupec=11;
			break;
		case TOK_ID:
			sloupec=12;
			break;
		default:
			sloupec=13;
			break;
	}

	return prec_table[radek][sloupec];
}


//----------------------------Smaze---zasobnik------------------------------
void SDipose(Tstack* St) {

	while (St->top > 0) {
		strFree(&(St->pom[St->top]));
		St->top--;
	}
}
//----------------------------INIT-Zasobniku--------------------------------
void InitialSt(Tstack* St) {
	string pom;
	strInit(&pom);
	strAddChar(&pom, '$');

	St->top = 0;
	St->t_n[St->top] = 'T';
	St->pom[St->top] = pom;
	St->val[St->top] = NULL;
	St->prom_val[St->top] = 'N';
}

//--------------------------------------------------------------------------
void PushE(Tstack *St, char Type, void * data) {
	//dam pravidlo na zasobnik
	if (St->top != MAX) {
		string pom;
		strInit(&pom);
		strAddChar(&pom, 'E');

		// vlozi o vlozem informace
		St->top++;
		St->t_n[St->top] = 'N';
		St->pom[St->top] = a;
		St->val[St->top] = data;	//

		if (data == NULL)
			St->prom_val[St->top] = Type;	//
	}
}
//----------------------------hleda--prvni--hnadle--------------------------
int FindBrc(Tstack *St) {
	// hleda prvni handle, jinak vraci syn. err
	int i = 0;
	char pom;
	if (St->pom[(St->top - i)].length >1)
		pom = St->pom[(St->top - i)].str[St->pom[(St->top - i)].length - 1];

	else pom = 0;

	while (pom != '<' && i < St->top) {
		i++;

		if (St->pom[(St->top - i)].length >1)
			pom = St->pom[(St->top - i)].str[S->pom[(St->top - i)].length - 1];
		else pom = 0;
	}

	if (pom != '<' && i == St->top) return SYNTAX_ERROR;

	else return St->top - i;
}

//----------------------------Vloz--token--terminal-------------------------
int TPush(Tstack *St) {
	//dam terminal na zasobnik
	if (St->top == MAX) return INTERN_ERROR;	//vnitrni chyba interperetu

	else {
		string pom;
		strInit(&pom);
		strCopyString(&pom, &attr);			// vlozim na zasobnik prichozi terminal

		St->top++;
		St->type[St->top] = 'T';
		St->pom[St->top] = pom;
	}

	return SUCCESS;
}
//--------------------Mazu---polozky--------------------------------------------
void DelI(Tstack* St, int n) {
	int i = 0;

	while (i<n) {
		strFree(&(St->pom[St->top]));
		St->top--;
		i++;
	}
}

//--------------------hleda-prvni--term--v--zasobniku---------------------------
int FindT(Tstack* St) {
	int i = 0;
	while (St->t_n[(St->top - i)] != 'T' && i < St->top) {
		i++;
	}
	return St->top - i;
}
//------------Redugujeme--:D---------------------------------------------------
int SReduction_expr(Tstack* St, int i, TinstList *instrList) {
	// pokusi se aplikovat pravidlo a zredukovat vyraz
	char E1;
	double * value1;
	double * value3;
	if (error != ERR) {
		E1 = St->promVal[i];
		value1 = St->val[i];
		value3 = malloc(sizeof(double));
		if (value3 == NULL)
			return INTERN_ERROR;
	}
	//----------------------------- E-> id------------------------------------------	
	if (strCmpConstStr(&(St->pom[i]), "id") == 0) {

		if (error != ERR)
			//Generovani instrukce
			genInstr(IMOV, (void *)value1, NULL, (void *)value3, instrList);
		DelI(St, 1);

		PushE(St, E1, value3);
		return SYNTAX_OK;
	}
	//------------------------------E-> (E)------------------------------------------	
	else if ((strCmpConstStr(&(St->a[i]), "(") == 0) &&
		(strCmpConstStr(&(St->a[i + 1]), "E") == 0) &&
		(strCmpConstStr(&(St->a[i + 2]), ")")) {


		if (error != ERR) {
			E1 = St->prom_val[i + 1];
			value1 = St->val[i + 1];
			//Generovani instrukce
			genInstr(IMOV, (void *)value1, NULL, (void *)value3, instrList);
		}

		DelI(St, 3);
		PushE(S, E1, value3);
		return SYNTAX_OK;
	}
	//------------------------------E-> E*E------------------------------------------	
	else if ((strCmpConstStr(&(St->a[i]), "E") == 0) &&
		(strCmpConstStr(&(St->a[i + 1]), "*") == 0) &&
		(strCmpConstStr(&(St->a[i + 2]), "E") == 0)) {


		if (error != ERR) {
			char E2 = St->prom_val[i + 2];
			double *value2 = St->val[i + 2];

			if (E1 != E2) return SEMANTIC_ERROR;
			//Generovani instrukce
			genInstr(IMUL, (void *)value1, (void *)value2, (void *)value3, instrList);
		}


		DelI(St, 3);
		// 3 se ulozi do zasobniku a ceka 
		PushE(St, E1, value3);
		return SYNTAX_OK;

	}

	//---------------------------E-> E/E---------------------------------------------	
	else if ((strCmpConstStr(&(St->a[i + 1]), "/") == 0) &&
		(strCmpConstStr(&(St->a[i + 2]), "E") == 0)) {

		// E-> E/E
		if (error != ERR) {
			char E2 = St->prom_val[i + 2];
			double *value2 = St->val[i + 2];

			//seman
			if (E1 != E2) return SEMANTIC_ERROR;
			//Generovani instrukce
			genInstr(IDIV, (void *)value1, (void *)value2, (void *)value3, instrList);
		}


		DelI(St, 3);
		PushE(St, E1, value3);
		return SYNTAX_OK;

	}

	//------------------------E-> E+E----------------------------------------------	
	else if ((strCmpConstStr(&(St->a[i + 1]), "+") == 0) &&
		(strCmpConstStr(&(St->a[i + 2]), "E") == 0)) {


		if (error != ERR) {
			char E2 = St->prom_val[i + 2];
			double *value2 = St->val[i + 2];

			//seman
			if (E1 != E2) return SEMANTIC_ERROR;
			//Generovani instrukce
			genInstr(IADD, (void *)value1, (void *)value2, (void *)value3, instrList);
		}


		DelI(St, 3);
		PushE(St, E1, value3);
		return SYNTAX_OK;

	}

	//-------------------------------E-> E-E---------------------------------------	
	else if ((strCmpConstStr(&(St->a[i + 1]), "-") == 0) &&
		(strCmpConstStr(&(St->a[i + 2]), "E") == 0)) {


		if (error != ERR) {
			char E2 = St->prom_val[i + 2];
			double *value2 = St->val[i + 2];

			//seman
			if (E1 != E2) return SEMANTIC_ERROR;
			//Generovani instrukce
			genInstr(ISUB, (void *)value1, (void *)value2, (void *)value3, instrList);
		}


		DelI(St, 3);
		PushE(St, E1, value3);
		return SYNTAX_OK;
	}


	//---------------------------E-> E>E--------------------------------------------	
	else if ((strCmpConstStr(&(St->a[i + 1]), ">") == 0) &&
		(strCmpConstStr(&(St->a[i + 2]), "E") == 0)) {


		if (error != ERR) {
			char E2 = St->prom_val[i + 2];
			double *value2 = St->val[i + 2];

			//seman
			if (E1 != E2) return SEMANTIC_ERROR;
			//Generovani instrukce
			genInstr(IBIG, (void *)value1, (void *)value2, (void *)value3, instrList);

		}


		DelI(St, 3);
		PushE(St, E1, value3);
		return SYNTAX_OK;
	}

	//----------------------------E-> E>=E--------------------------------------------	
	else if ((strCmpConstStr(&(St->a[i + 1]), ">=") == 0) &&
		(strCmpConstStr(&(St->a[i + 2]), "E") == 0)) {


		if (error != ERR) {
			char E2 = St->prom_val[i + 2];
			double *value2 = St->val[i + 2];

			//seman
			if (E1 != E2) return SEMANTIC_ERROR;
			//Generovani instrukce
			genInstr(IEQBG, (void *)value1, (void *)value2, (void *)value3, instrList);


		}


		DelI(St, 3);
		PushE(St, E1, value3);
		return SYNTAX_OK;
	}

	//-----------------------E-> E<E-------------------------------------------------	
	else if ((strCmpConstStr(&(St->a[i + 1]), "<") == 0) &&
		(strCmpConstStr(&(St->a[i + 2]), "E") == 0)) {


		if (error != ERR) {
			char E2 = St->prom_val[i + 2];
			double *value2 = St->val[i + 2];

			//seman
			if (E1 != E2) return SEMANTIC_ERROR;
			//Generovani instrukce
			genInstr(ISMALL, (void *)value1, (void *)value2, (void *)value3, instrList);

		}


		DelI(St, 3);
		PushE(St, E1, value3);
		return SYNTAX_OK;
	}


	//------------------------E-> E<=E-----------------------------------------------	
	else if ((strCmpConstStr(&(St->a[i + 1]), "<=") == 0) &&
		(strCmpConstStr(&(St->a[i + 2]), "E") == 0)) {

		if (error != ERR) {
			char E2 = St->prom_val[i + 2];
			double *value2 = St->val[i + 2];

			//seman
			if (E1 != E2) return SEMANTIC_ERROR;
			//Generovani instrukce
			genInstr(IEQSM, (void *)value1, (void *)value2, (void *)value3, instrList);
		}

		DelI(St, 3);
		PushE(St, E1, value3);
		return SYNTAX_OK;
	}


	//------------------------E-> E=E------------------------------------------------	
	else if ((strCmpConstStr(&(St->a[i + 1]), "==") == 0) &&
		(strCmpConstStr(&(St->a[i + 2]), "E") == 0)) {


		if (error != ERR) {
			char E2 = St->prom_val[i + 2];
			double *value2 = St->val[i + 2];

			//seman
			if (E1 != E2) return SEMANTIC_ERROR;
			//Generovani instrukce
			genInstr(IEQUAL, (void *)value1, (void *)value2, (void *)value3, instrList);
		}


		DelI(St, 3);
		PushE(St, E1, value3);
		return SYNTAX_OK;
	}


	//--------------------------E-> E!=E---------------------------------------------	
	else if ((strCmpConstStr(&(St->a[i + 1]), "!=") == 0) &&
		(strCmpConstStr(&(St->a[i + 2]), "E") == 0)) {


		if (error != ERR) {
			char E2 = St->prom_val[i + 2];
			double *value2 = St->val[i + 2];

			seman
				if (E1 != E2) return SEMANTIC_ERROR;
			//Generovani instrukce
			genInstr(INOTEQ, (void *)value1, (void *)value2, (void *)value3, instrList);

		}


		DelI(St, 3);
		PushE(St, E1, value3);
		return SYNTAX_OK;
	}

	return SYNTAX_ERROR;

}
//-----------------------------------------------------------------------------------------
int comp_expr(TinstList *instrList) {

	char result;
	int i;
	int chba;
	int vyber;
	Tstack St;
	string pom;

	strInit(&pom);
	StackInit(&st);
	pom = TopT(&st);

	while (strCmpConstStr(&pom, "$") != 0 && (token != TOK_LEFT_BRACE || token != TOK_SEMICOLON)) {

		result = select_ruler(&pom, token);

		if (result == '<') vyber = TOK_LESS_THAN;
		else if (result == '>') vyber = TOK_GREATER_THAN;
		else if (result == '=') vyber = TOK_EQUALS;
		else vyber = OTHER;


		switch (vyber) {
		case TOK_LESS_THAN:
			TAdd(&St, '<');
			strAddChar(&(St->a[FindT(St)]), symbol);
			token = getNextToken(&attr);
			break;

		case TOK_GREATER_THAN:
			if ((i = FindBrc(&St)) == SYNTAX_ERROR) {
				SDipose(&St);
				error = ERR;
				return SYNTAX_ERROR;
			}

			strDelLastChar(&(St.pom[i]));
			chba = (TReduction(&St, i + 1, instrList);
			if (chba != SYNTAX_OK)
				SDipose(&St);
			error = ERR;
			return result;

			break;

		case TOK_EQUALS:
			chba = Tpush(&stack);
			if (chba != SUCCESS) return chba;

			token = getNextToken(&attr);
			break;

		case OTHER:
			SDipose(&stack);
			error = ERR;
			return SYNTAX_ERROR;
			break;
		}

		pom = St->pom[FinT(St)];	//prvni term.
	}


	strFree(&pom);
	SDipose(&St);
	return SYNTAX_OK;
}


//--------------------------------------------------------------------------------//
//								REKURZIVNI SESTUP								  //
//--------------------------------------------------------------------------------//

//-TERM->TOK_STR--||--TOK_ID--||--TOK_DECIMAL_NUMBER--||--TOK_FLOATING_POINT_NUMBER
int term(){
	if (token != (TOK_STR || TOK_ID || TOK_DECIMAL_NUMBER || TOK_FLOATING_POINT_NUMBER)){
		return SYNTAX_ERROR;
	}

	return SYNTAX_OK;
}
//-----------NONTERMINAL-TYPE---------------------------------------------------
int type(){
	if (token != (TOK_INT || TOK_DOUBLE || TOK_STRING || TOK_AUTO)){
		return SYNTAX_ERROR;
	}

	return SYNTAX_OK;
}

//-----I_PROM->--=--EXPR--||--eps
int _i_prom(TinstList *instrList){
	int result;

	switch(token){
		case TOK_SEMICOLON:
			return SYNTAX_OK;
			break;
		
		case TOK_EQUALS:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			
			result= comp_expr(instrList);
			if(result !=SYNTAX_OK) return result;

			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if(token !=TOK_SEMICOLON) return SYNTAX_ERROR;
			
			return SYNTAX_OK;
			break;

	}
	return SYNTAX_ERROR;
}

//-----LIST_PAR_N->--,--TERM--LIST_PAR_N--||--eps--------------------------------
int list_par_n(){
	int result;
	switch(token){
		case TOK_RIGHT_BRACKET:
			return SYNTAX_OK;
			break;
		
		case TOK_COMMA:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			result= term();
			if(result !=SYNTAX_OK) return result;

			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			return list_par_n();
			break;

	}
	return SYNTAX_ERROR;
}
//-----LIST_PAR->--TERM--LIST_PAR_N--||--eps-------------------------------------
int list_par(){
	int result;

	switch(token){
		case TOK_RIGHT_BRACKET:
			return SYNTAX_OK;
			break;
		default:
			result= term();

			if(result !=SYNTAX_OK) return result;

			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			result= list_par_n();

			if (result !=SYNTAX_OK) return result;
			return SYNTAX_OK;
			break;
	}
	return SYNTAX_ERROR;		
}
//-----CALLF_DEC->--id--(--LIST_PAR--)--||->EXPR---------------------------------
int callf_dec(TinstList *instrList){
	int result;

	switch(token){
		case TOK_ID:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if(token !=TOK_LEFT_BRACKET) return SYNTAX_ERROR;

			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			
			result= list_par();
			if(result !=SYNTAX_OK) return result;

			return SYNTAX_OK;
			break;

		 default:
		 	result= comp_expr(instrList);
		 	if(result !=SYNTAX_OK) return result;

		 	return SYNTAX_OK;
		 	break;
	}
	return SYNTAX_ERROR;
}

//-----PROM->--id--=--CALL_DEF--;||->TYPE--id--I_PROM--;-------------------------
int _prom(TinstList *instrList){
	int result;
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	
	switch(token){
		case TOK_EQUALS:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			
			result= callf_dec(instrList);
			if(result !=SYNTAX_OK)return result;
			
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if(token !=TOK_SEMICOLON) return SYNTAX_ERROR;
			
			return SYNTAX_OK;
			break;
		
		case TOK_ID:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			result= _i_prom(instrList);

			if(result !=SYNTAX_OK) return result;

			return SYNTAX_OK;
			break;
	}
	return SYNTAX_ERROR;
}

//-----RETURN->--return--EXPR--;------------------------------------------------
int _return(TinstList *instrList){
	int result;
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;

	result= comp_expr(instrList);
	if (result !=SYNTAX_OK) return result;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if(token !=TOK_SEMICOLON) return SYNTAX_ERROR;
	
	//return je dobre zapsan, neni co resit
	return SYNTAX_OK;
}

//-----------FOR->for--(--TYPE--id--I_PROM--;--EXPR--;--id--=--EXPR)--BODY------
int _for(TinstList *instrList){
	int result; 
	
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACKET) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	
	result= type();
	if (result !=SYNTAX_OK) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_ID) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result= _i_prom(instrList);
	if (result !=SYNTAX_OK) return SYNTAX_ERROR;

    string Label_1; //label, pro navrat
    strInit(&Label_1); //inicializace
    GenNewVariable(&Label_1);  // vygenerovani promenne
    tTableItem* prom = tableInsert(local_table, &Label_1, TOK_STRING);
    tableInsertValue(local_table, &Label_1, Label_1)
   // tData *newVariableInfo;
   // newVariableInfo = tableSearch(local_table, &Label_1);
   // strFree(&Label_1);
    // instrukce pro label
    genInstr(ILABEL, &prom->data, NULL, NULL, instrList);
    
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result= comp_expr(instrList);
	if (result !=SYNTAX_OK) return SYNTAX_ERROR;
    
    // najde promennou ve ktere je vyhodnocena podminka
    tData *LastVar = ReadNameVar(instrList);
    genInstr(INOT, LastVar, NULL, LastVar, instrList);
    
    string Label_2; //label, pro navrat
    strInit(&Label_2); //inicializace
    GenNewVariable(&Label_2);  // vygenerovani promenne
    tTableItem* prom2 = tableInsert(local_table, &Label_2, TOK_INT);
    tableInsertValue(local_table, &Label_2, Label_2)
    genInstr(IIFGOTO, LastVar, NULL, &prom2->data, instrList);
     
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_SEMICOLON) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_ID) return SYNTAX_ERROR;
	if(!(tableSearch(local_table, &attr))) return SEMANTIC_ERROR;
	
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_EQUALS) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result= comp_expr(instrList);

	if (result !=SYNTAX_OK) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_RIGHT_BRACKET) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACE) return SYNTAX_ERROR;
	result= body(instrList);

    // instrukce skoku
    genInstr(IGOTO, NULL, NULL, &prom->data, instrList);
    //instrukce label pro skonceni cyklu
    genInstr(ILABEL, &prom->data, NULL, NULL, instrList);
	if(result !=SYNTAX_OK) return result;


	//cely for je v tom, ze je to opravdu for a dokonce spravne zapsany >:D
	return SYNTAX_OK;
}

//-----------TERM_N->--<<--TERM--TERM_N--||eps--------------------------------
int term_n(TinstList *instrList){
	int result;

	switch (token) {
		case TOK_SEMICOLON:
			return SYNTAX_OK;
			break;
		
		case TOK_DOUBLE_ARROW_LEFT:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			
			result = term();
			if (result != SYNTAX_OK) return result;
			
			//generovani instukce
			genInstr(IWRITE, NULL, NULL, token, instrList);
			
			//mozna jich je jeste vic, radeji si ho zavolam znovu
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			return term_n(instrList);
			break;
	}
	return SYNTAX_ERROR;
}

//-----------COUT->--cout--<<--TERM--TERM_N--;--------------------------------
int _cout(TinstList *instrList){
	int result;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_DOUBLE_ARROW_LEFT) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result = term();

	if(token !=SYNTAX_OK) return result;
	
	// instrukce pro zapis, pokud jich je vice generuji se v term_n()
    	genInstr(IWRITE, NULL, NULL, token, instrList);
	
	//zavolam scanner a jdu zjistit, jestli termu neni vic a jestli jsou ok
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result = term_n(instrList);

	if (result !=SYNTAX_OK) return result;
	
	return SYNTAX_OK;
}

//-----------ID_N->-->>--id--ID_N--||--eps------------------------------------
int _id_n(TinstList *instrList){

	switch (token){
		case TOK_SEMICOLON:
			return SYNTAX_OK;
			break;
		
		case TOK_DOUBLE_ARROW_RIGHT:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if (token !=TOK_ID) return SYNTAX_ERROR;
			if(!(tableSearch(local_table, &attr))) return SEMANTIC_ERROR;
			
			// vygeneruju instrukci
			genInstr(IREAD, NULL, NULL, token, instrList);
			
			//a zavolam si ji znova
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			return _id_n(instrList);
			break;

	}
	return SYNTAX_ERROR;
}

//-----------CIN->cin-->>--id--ID_N--;
int _cin(TinstList *instrList){
	int result;
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_DOUBLE_ARROW_RIGHT) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_ID) return SYNTAX_ERROR;
	if(!(tableSearch(local_table, &attr))) return SEMANTIC_ERROR;
	
	// generuji prvni instrukci, ostatni se generujou v _id_n()
	genInstr(IREAD, NULL, NULL, token, instrList);
	
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	
	//podivu se, jestli tam neni vic identifikatoru
	result= _id_n(instrList);
	if(result !=SYNTAX_OK) return result;

	// cin je dobre po syn. strance
	return SYNTAX_OK;
}

//-----------IF->if--(--EXPR--)--BODY--else--BODY----------------------------
int _if(tSymbolTable *global_table, string *id){
	int result;
	
	//nasleduje leva zavorka a v ni vyraz
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACKET) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result = comp_expr(global_table, id);

	if (result !=SYNTAX_OK) return result;
	//vyraz je ve v poradku uzavru ho

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_RIGHT_BRACKET) return SYNTAX_ERROR;

    //generovani pomocne promenne
    tData *LastVar //= ReadNameVar(instrList); // funkce na cteni nazvu posledni instrukce 
    genInstr(INOT, LastVar, NULL, LastVar); // negace podminky
    
    string Label_1; //novy label, skok na vetev else
    strInit(&Label_1); //inicializace
    GenNewVariable(&Label_1);  // vygenerovani promenne
    //tTableItem* prom  = tableInsert(local_table, &Label_1, TOK_STRING);    // vlozeni do lokalni tabulky symbolu
    //tableInsertValue (local_table, &Label_1, Label_1);
    
    //generovani skoku na ELSE vetev
    Tinst *instrukce = genInstr(IIFGOTO, LastVar, NULL, &prom->data);
    GtableInsertInstr(global_table, id, instrukce);
	//telo pokud je v if pravda
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACE) return SYNTAX_ERROR;
	result= body(global_table);

    string Label_2;  // label dva, skok az za else, podminka v IF byla pravda
    strInit(&Label_2);
    GenNewVariable(&Label_2);
    //tTableItem* prom2 = tableInsert(local_table, &Label_2, TOK_STRING);
    //tableInsertValue(local_table, &Label_2, Label_2);
    
    // skok za ELSE
    instrukce = genInstr(IGOTO, NULL, NULL, &prom2->data);
    GtableInsertInstr(global_table, id, instrukce);
    instrukce = genInstr(ILABEL, &prom->data, NULL, NULL); // musime vlozit label za telo IFu
    GtableInsertInstr(global_table, id, instrukce);
    
	if(result !=SYNTAX_OK) return result;
	//vse ok, nasleduje else a za ni else

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_ELSE) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACE) return SYNTAX_ERROR;
	result= body(global_table, id);

    // instrukce pro label_2, sem se skoci jestlize podminka IF byla pravda
    instrukce = genInstr(ILABEL,(void *) &Label_2, NULL, NULL);
    GtableInsertInstr(global_table, id, instrukce);
	if(result !=SYNTAX_OK) return result;
	// konstrukce if je v poradku muze opustit s pozitvni odpovedi
	return SYNTAX_OK;

}
//-----------STMNT->IF||BODY||FOR||CIN||COUT||RETURN||PROM-------------------
int stmnt(tSymbolTable *global_table, string *id){
	int result;

	switch (token){
		case TOK_LEFT_BRACE:
			return body(global_table, id);
			break;
		
		case TOK_IF:
			result= _if(global_table, id);
			if (result != SYNTAX_OK) return result;
			return stmnt(global_table, id);
			break;
		
		case TOK_FOR:
			result= _for(global_table, id);
			if (result != SYNTAX_OK) return result;
			return stmnt(global_table, id);
			break;

		case TOK_CIN:
			result= _cin(global_table, id);
			if (result != SYNTAX_OK) return result;
			return stmnt(global_table, id);
			break;

		case TOK_COUT:
			result= _cout(global_table, id);
			if (result != SYNTAX_OK) return result;
			// volam rekurzvine funkci
			return stmnt(global_table, id);
			break;
		
		case TOK_RETURN:
			result= _return(global_table, id);
			if (result != SYNTAX_OK) return result;
			return SYNTAX_OK;
			break;

		//PROM
		case TOK_ID:
		case TOK_INT:
		case TOK_STRING:
		case TOK_DOUBLE:
		case TOK_AUTO:
			result= _prom(global_table, id);
			if (result != SYNTAX_OK) return result;
			return SYNTAX_OK;
			break;
		
		//prazdny statement
		case TOK_RIGHT_BRACE:
			return SYNTAX_OK;
			break;
	}
	return SYNTAX_ERROR;
}
//-----------BODY->{STMNT}----------prazdny-statement-nebo-zaplneny----------
int body(tSymbolTable *global_table, string *id){
	int result;
	// jsme v body a melo bz nasledovat prud } nebo eps
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;

	result = stmnt(global_table, id);
	if (result !=SYNTAX_OK) return result;

	return SYNTAX_OK;

}
//-----------SELECT->BODY--||--;----bud-je-to-funkce-nebo-deklarace----------
int select(tSymbolTable *global_table, string *id){
	int result;
	//ocekavam v token { nebo ;, podle toho poznam, zda se jedna o fce nebo dek.

	switch (token){
		case TOK_LEFT_BRACE:
			result=body(global_table, id);
			if (result !=SYNTAX_OK) return result;
			
			return SYNTAX_OK;
			break;
		case TOK_SEMICOLON:
			// jedna se o deklaraci fce
			return SYNTAX_OK;
			break;
	}
	return SYNTAX_ERROR;
}

//-----------PARAM_N->-,-TYPE--ID--PARAM_N-----------------------------------
int param_n(tSymbolTable *global_table, string *id){
	int result;
	
	//pozadam o dalsi token a musi byt ) nebo ,
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;

	switch (token){
		case TOK_RIGHT_BRACKET:
			return SYNTAX_OK;
			break;
		case TOK_COMMA:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			
			//musi nasledovat TYPE a id
			result = type();
			if (result != SYNTAX_OK) return SYNTAX_ERROR;
			int InternalType = token;
			
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if (token != TOK_ID) return SYNTAX_ERROR;
			string *idParam = attr;
			
			GtableInsertParam(global_table, id, idParam, InternalType);
			// zavolam si zpet funkci, zda nema dalsi parametry
			return param_n(global_table, id);
			
			break;
	}
	return SYNTAX_ERROR;
}

//-----------PARAM->TYPE--id--PARAM_N----------------------------------------
int param(tSymbolTable *global_table, string *id){
	int result;
	
	//pozdaval jsem o dalsi token a ocekavam () nebo type
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	
	
	
	switch (token){
		case TOK_RIGHT_BRACKET:
			return SYNTAX_OK;
			break;
		case TOK_INT:
		case TOK_AUTO:
		case TOK_STRING:
		case TOK_DOUBLE:
			int InternalType = token; // ulozeni typu parametru, pro pozdejsi vlozeni do TS
			//po Type nasleduje ID
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if (token != TOK_ID) return SYNTAX_ERROR;
			// ulozeni identifikatoru pro parametr
			string *idParam = attr;
			//vlozeni parametru do GTS
			GtableInsertParam(global_table, id, idParam, InternalType);
			//zjistim, zda neni v zavorce vice parametru
			result=param_n(id);
			if (result !=SYNTAX_OK) return result;

			return SYNTAX_OK;
			break;
	}
	return SYNTAX_ERROR;
}
//-----------FUNC_DCLR->TYPE--id--(--PARAM--)--SELECT---FUNC_DCLR------------
int func_dclr(tSymbolTable *global_table){
	int result;
	
	
	
	result= type();
	if (result != SYNTAX_OK) return SYNTAX_ERROR;
///	
	int InternalType = token; 
	
	// dalsi token ID
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token != TOK_ID) return SYNTAX_ERROR;
	
	string *id = attr;
	GtableInsert(global_table, id, InternalType);
///
	
	// po ID nasleduje (
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token != TOK_LEFT_BRACKET) return SYNTAX_ERROR;
	// TYPE ID(PARAM)
	result=param(global_table, id);
	
	if (result !=SYNTAX_OK) return result;
	// uvnitr zavorky je vse ok, frcim dal
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result=select(global_table, id);

	if (result !=SYNTAX_OK) return result;
	//!!!!!!! nevim, jestli muzu ukoncit uspesne, protoze pokud prijde pouze dekalarace
	// a nenasleduje zadna fuknce, neni to prece validni
	
	
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	// mela bz prijit rekurze, ale nevim jestli bude fungovat
	return SYNTAX_OK;

}

//------------PROGRAM->FUNC_DCLR---------------------------------------------
int program(tSymbolTable *global_table){
	int result;
	result= func_dclr(global_table);
	if(result != SYNTAX_OK) return result;
	//prosel jsem cely program a scanner uz nema co davat
	while (token != TOK_END_OF_FILE) {
		//cyklim, dokud mi nedojde konec souboru
		
		func_dclr(global_table);
	}
	
	//generuji konec programu
	string *id = attr;
	Tinst *instrukce = genInstr(IEND,NULL,NULL,NULL);
	GtableInsertInstr(global_table, id, instrukce);
	return SYNTAX_OK;
	
}
//------------------START-POINT--------------------------------------------------
int parse(tSymbolTable *ST){

  int result;
  tSymbolTable *global_table = ST;
  strInit(&attr);

	if((token = getNextToken(&attr))== LEX_ERROR) return LEX_ERROR;
	
	else
		result=program(global_table);	// volam prvni neterminal 
	
	strFree(&attr);
	return result;
}
