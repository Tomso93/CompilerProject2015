/*
*	Projekt: Kompilator pro jazyk IFJ15=>podmnozina C++11
*	Autor: Tomas STRNKA
*	Login: xstrnk01
*	Cast: Parser .c +.h
*	Verze: 1.4 
*/

#include <stdio.h>
#include "lex.h"		// getNextToken()
#include "define.h"		// instrukce, tokeny, err
#include "3ac.h"		// genInstr, GenNewVariable  
#include "str.h"		// retezce
#include "stable.h"		// tabulka symbolu
#include "instlist.h"	// seznam instukci
#define SYNTAX_OK  0		// zatim bez knihovny, potom dopisu

int token; //Globalni promennna pro pradavani tokenu 
string attr; 

int body();	// fce je volana drive nez je definovana

char prec_table[][14]={
{'>','>','<','<','>','>','>','>','>','>','<','>','<','>'}
{'>','>','<','<','>','>','>','>','>','>','<','>','<','>'}
{'>','>','>','>','>','>','>','>','>','>','<','>','<','>'}
{'>','>','>','>','>','>','>','>','>','>','<','>','<','>'}
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>'}
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>'}
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>'}
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>'}
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>'}
{'<','<','<','<','>','>','>','>','>','>','<','>','<','>'}
{'<','<','<','<','<','<','<','<','<','<','=','<','<','0'}
{'>','>','>','>','>','>','>','>','>','>','0','>','0','>'}
{'>','>','>','>','>','>','>','>','>','>','0','>','0','>'}
{'<','<','<','<','<','<','<','<','<','<','<','0','<','0'}
};
//--hleda--rade/sloupec, aby mohl potom rozhodnout---------------------------------

//--------------EXPR---------------------------------------------------------------	
int comp_expr(){

	while(&&(token !=TOK_LEFT_BRACE || token !=TOK_SEMICOLON))
}
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
int _i_prom(){
	int result;

	switch(token){
		case TOK_SEMICOLON:
			return SYNTAX_OK;
			break;
		
		case TOK_EQUALS:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			
			result= comp_expr();
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
int callf_dec(){
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
		 	result= comp_expr();
		 	if(result !=SYNTAX_OK) return result;

		 	return SYNTAX_OK;
		 	break;
	}
	return SYNTAX_ERROR;
}

//-----PROM->--id--=--CALL_DEF--;||->TYPE--id--I_PROM--;-------------------------
int _prom(){
	int result;
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	
	switch(token){
		case TOK_EQUALS:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			
			result= callf_dec();
			if(result !=SYNTAX_OK)return result;
			
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if(token !=TOK_SEMICOLON) return SYNTAX_ERROR;
			
			return SYNTAX_OK;
			break;
		
		case TOK_ID:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			result= _i_prom();

			if(result !=SYNTAX_OK) return result;

			return SYNTAX_OK;
			break;
	}
	return SYNTAX_ERROR;
}

//-----RETURN->--return--EXPR--;------------------------------------------------
int _return(){
	int result;
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;

	result= comp_expr();
	if (result !=SYNTAX_OK) return result;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if(token !=TOK_SEMICOLON) return SYNTAX_ERROR;
	
	//return je dobre zapsan, neni co resit
	return SYNTAX_OK;
}

//-----------FOR->for--(--TYPE--id--I_PROM--;--EXPR--;--id--=--EXPR)--BODY------
int _for(){
	int result; 
	
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACKET) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	
	result= type();
	if (result !=SYNTAX_OK) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_ID) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result= _i_prom();
	if (result !=SYNTAX_OK) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result= comp_expr();
	if (result !=SYNTAX_OK) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_SEMICOLON) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_ID) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_EQUALS) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result= comp_expr();
	if (result !=SYNTAX_OK) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_RIGHT_BRACKET) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACE) return SYNTAX_ERROR;
	result= body();

	if(result !=SYNTAX_OK) return result;

	//cely for je v tom, ze je to opravdu for a dokonce spravne zapsany >:D
	return SYNTAX_OK;
}

//-----------TERM_N->--<<--TERM--TERM_N--||eps--------------------------------
int term_n(){
	int result;

	if(token == TOK_SEMICOLON) return SYNTAX_OK;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_DOUBLE_ARROW_LEFT) return SYNTAX_ERROR;
	
	result= term();
	if (result !=SYNTAX_OK) return result;

	//mozna jich je jeste vic, radeji si ho zavolam znovu
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	return term_n();

}

//-----------COUT->--cout--<<--TERM--TERM_N--;--------------------------------
int _cout(){
	int result;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_DOUBLE_ARROW_LEFT) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result = term();

	if(token !=SYNTAX_OK) return result;

	//zavolam scanner a jdu zjistit, jestli termu neni vic a jestli jsou ok
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result = term_n();

	if (result !=SYNTAX_OK) return result;
	
	return SYNTAX_OK;
}

//-----------ID_N->-->>--id--ID_N--||--eps------------------------------------
int _id_n(){

	switch (token){
		case TOK_SEMICOLON;
			return SYNTAX_OK;
			break;
		
		case TOK_DOUBLE_ARROW_RIGHT:
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if (token !=TOK_ID) return SYNTAX_ERROR;
			
			//a zavolam si ji znova
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			return _id_n();
			break;

	}
	return SYNTAX_ERROR;
}

//-----------CIN->cin-->>--id--ID_N--;
int _cin(){
	int result;
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_DOUBLE_ARROW_RIGHT) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_ID) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	
	//podivu se, jestli tam neni vic identifikatoru
	result= _id_n();
	if(result !=SYNTAX_OK) return result;

	// cin je dobre po syn. strance
	return SYNTAX_OK;
}

//-----------IF->if--(--EXPR--)--BODY--else--BODY----------------------------
int _if(){
	int result;
	
	//nasleduje leva zavorka a v ni vyraz
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACKET) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result = comp_expr();

	if (result !=SYNTAX_OK) return result;
	//vyraz je ve v poradku uzavru ho

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_RIGHT_BRACKET) return SYNTAX_ERROR;

	//telo pokud je v if pravda
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACE) return SYNTAX_ERROR;
	result= body();

	if(result !=SYNTAX_OK) return result;
	//vse ok, nasleduje else a za ni else

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_ELSE) return SYNTAX_ERROR;

	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token !=TOK_LEFT_BRACE) return SYNTAX_ERROR;
	result= body();

	if(result !=SYNTAX_OK) return result;
	// konstrukce if je v poradku muze opustit s pozitvni odpovedi
	return SYNTAX_OK;

}
//-----------STMNT->IF||BODY||FOR||CIN||COUT||RETURN||PROM-------------------
int stmnt(){
	int result;

	switch (token){
		case TOK_LEFT_BRACE:
			return body();
			break;
		
		case TOK_IF:
			result= _if();
			if (result != SYNTAX_OK) return result;
			return stmnt();
			break;
		
		case TOK_FOR:
			result= _for();
			if (result != SYNTAX_OK) return result;
			return stmnt();
			break;

		case TOK_CIN:
			result= _cin();
			if (result != SYNTAX_OK) return result;
			return stmnt();
			break;

		case TOK_COUT:
			result= _cout();
			if (result != SYNTAX_OK) return result;
			// volam rekurzvine funkci
			return stmnt();
			break;
		
		case TOK_RETURN:
			result= _return();
			if (result != SYNTAX_OK) return result;
			return SYNTAX_OK;
			break;

		//PROM
		case TOK_ID:
		case TOK_INT:
		case TOK_STRING:
		case TOK_DOUBLE:
		case TOK_AUTO:
			result= _prom();
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
int body(){
	int result;
	// jsme v body a melo bz nasledovat prud } nebo eps
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;

	result = stmnt();
	if (result !=SYNTAX_OK) return result;

	return SYNTAX_OK;

}
//-----------SELECT->BODY--||--;----bud-je-to-funkce-nebo-deklarace----------
int select(){
	int result;
	//ocekavam v token { nebo ;, podle toho poznam, zda se jedna o fce nebo dek.

	switch (token){
		case TOK_LEFT_BRACE:
			result=body();
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
int param_n(){
	int result;
	
	//pozadam o dalsi token a musi byt } nebo ,
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

			
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if (token != TOK_ID) return SYNTAX_ERROR;
			// zavolam si zpet funkci, zda nema dalsi parametry
			return param_n();
			
			break;
	}
	return SYNTAX_ERROR;
}

//-----------PARAM->TYPE--id--PARAM_N----------------------------------------
int param(){
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
			//po Type nasleduje ID
			if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			if (token != TOK_ID) return SYNTAX_ERROR;
			
			//zjistim, zda neni v zavorce vice parametru
			result=param_n();
			if (result !=SYNTAX_OK) return result;

			return SYNTAX_OK;
			break;
	}
	return SYNTAX_ERROR
}
//-----------FUNC_DCLR->TYPE--id--(--PARAM--)--SELECT---FUNC_DCLR------------
int func_dclr(){
	int result;
	
	result= type();
	if (result != SYNTAX_OK) return SYNTAX_ERROR;
	// dalsi token ID
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token != TOK_ID) return SYNTAX_ERROR;
	// po ID nasleduje (
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token != TOK_LEFT_BRACKET) return SYNTAX_ERROR;
	// TYPE ID(PARAM)
	result=param();
	if (result !=SYNTAX_OK) return result;
	// uvnitr zavorky je vse ok, frcim dal
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	result=select();

	if (result !=SYNTAX_OK) return result;
	// nevim, jestli muzu ukoncit uspesne, protoze pokud prijde pouze dekalarace
	// a nenasleduje zadna fuknce, neni to prece validni
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	// mela bz prijit rekurze, ale nevim jestli bude fungovat
	return SYNTAX_OK;

}

//------------PROGRAM->FUNC_DCLR---------------------------------------------
int program(){
	int result;
	result= func_dclr();
	if(result != SYNTAX_OK) return result;
	//prosel jsem cely program a scanner uz nema co davat
	if (token != END_OF_FILE) return SYNTAX_ERROR;
	//generuji konec programu
	genInstr(IEND,NULL,NULL,NULL)
	return SYNTAX_OK;
	
}
//------------------START-POINT--------------------------------------------------
int parse(){

	int result;	// vychozvi bod PARSERU
	

	if((token = getNextToken(&atrr))== LEX_ERROR)
		result=LEX_ERROR;
	else
		result=program();	// volam prvni neterminal 
	
	
	return result;
}