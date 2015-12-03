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


int token; //Globalni promennna pro pradavani tokenu 
string attr; //retezec tokenu
tSymbolTable *global_table;	// tabulka pro funkce a deklarace
tSymbolTable *local_table; //tabulka pro telo kazde funkce
int error;


int body();	// fce je volana drive nez je definovana
//--------------------------------------------------------------------------------//
//					PRECEDENCNI SYNTAKTICKY ANALYZATOR							  //
//--------------------------------------------------------------------------------//																				

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
int select_ruler(string *zas_term, int term){
	int radek, sloupec;
	
	//radek--jeste neni reseno, ze by bylo =<
	if (strCmpConstStr(Pchar, "*"))==0) radek=0;
	
	if (strCmpConstStr(Pchar, "/"))==0) radek=1;
	
	if (strCmpConstStr(Pchar, "+"))==0) radek=2;
	
	if (strCmpConstStr(Pchar, "-"))==0) radek=3;
	
	if (strCmpConstStr(Pchar, "<"))==0) radek=4;

	if (strCmpConstStr(Pchar, ">"))==0) radek=5;

	if (strCmpConstStr(Pchar, "<="))==0) radek=6;

	if (strCmpConstStr(Pchar, ">="))==0) radek=7;

	if (strCmpConstStr(Pchar, "=="))==0) radek=8;

	if (strCmpConstStr(Pchar, "!="))==0) radek=9;

	if (strCmpConstStr(Pchar, "("))==0) radek=10;

	if (strCmpConstStr(Pchar, ")"))==0) radek=11;

	if (strCmpConstStr(Pchar, "id"))==0) radek=12;

	if (strCmpConstStr(Pchar, "$"))==0) radek=13;

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

	return prec_table[radek][sloupec]
}
//---------------------------------------------------------------------------------
// struktura zasobniku
typedef struct	{                          
    string a[25];	// text
	char type[25];	// T , N 
	double *value[25];	// hodnota 
	char valueType [25]; // type promenne
    int top;
} tStackTN;


//---------------------------Init-zasobnik-----------------------------------------
void StackInit(tStackTN * S){
// inicializuje zasobnik, na nultou pozici da "eps"
	string a;
	strInit(&a);
	
	strAddChar(&a, '$');
	
	S->top = 0; 
	S->type[S->top] = 'T';
	S->a[S->top]=a;
	S->value[S->top] = NULL;
	S->valueType[S->top] = 'N';
} 


//----------------------------Vloz--token--terminal--------------------------------
void SPushTerm (tStackTN *S){
//dam terminal na zasobnik
  if (S->top==25) 
    fprintf(stderr,"zasobnik pretekl\n");
  
  else {  
		string a;
		strInit(&a);
		strCopyString(&a, &token);
			
		S->top++;  
		S->type[S->top] = 'T';
		S->a[S->top] = a;
	}		
}	


//-----------hod--pravidlo--na--zasobnik--a--oznac--jako--neterminal-------------
void SPushNeterm(tStackTN *S, char Type, void * data){
//dam pravidlo na zasobnik
  if (S->top==25) 
    fprintf(stderr,"zasobnik pretekl\n");
  else {  
		string a;
		strInit(&a);
		strAddChar(&a,'E');
		
		// vlozi o vlozem informace
		S->top++;  
		S->type[S->top] = 'N';
		S->a[S->top] = a;		
		S->value[S->top] = data;
		if (data == NULL)
		S->valueType[S->top] = Type ;
	
	}
}	


//--------------index--vrcholu--zasobniku----------------------------------------
int SSearchTerm(tStackTN * S){
	// index prvniho terminalu
	int i=0;	
	while (S->type[(S->top-i)] != 'T' && i < S->top ){
		i++;		
	}
	return S->top - i;
}

//-------------hleda--prvni--handle-----------------------------------------------
int SSearchBracket(tStackTN * S){
 // hleda prvni handle, jinak vraci syn. err
	int i=0;
	char a;
	if (S->a[(S->top - i)].length >1)
		a=S->a[(S->top - i)].str[ S->a[(S->top - i)].length - 1];
	else a = 0;
	
	while (a != '<' && i < S->top){
		i++;
		if (S->a[(S->top - i)].length >1)
			a=S->a[(S->top - i)].str[ S->a[(S->top - i)].length - 1];
		else a = 0;
	}
	
	if (a != '<' && i == S->top)
		return -1;
	else return S->top - i;
}


//--------------hodnota--vrcholu--zasobniku---------------------------------------
string STopTerm(tStackTN * S){
	// hodnota prvniho terminalu
	return S->a[SSearchTerm(S)];


} 


//-------------prida---handle--za--terminal---------------------------------------
void SAddTerm(tStackTN * S, char symbol){
	// prida k terminalu nakonec znak
	strAddChar(&(S->a[SSearchTerm(S)]), symbol);


}


//-------------smaze--urcite--polozky--ve--stacku---------------------------------
void SDeleteItem(tStackTN * S, int count){
	// smaze polozky ze zasobniku
	int i=0;
	
	while (i<count){
		strFree(&(S->a[S->top]));
		S->top--;
		i++;
	}

}


//-----------------smaze-zasobnik--------------------------------------------------
void SDipose(tStackTN *S){
// smaze zasobnik
	while (S->top > 0){
		strFree(&(S->a[S->top]));
		S->top--;
	}
}

nt SReduction_expr (tStackTN * S, int index){
// pokusi se aplikovat pravidlo a zredukovat vyraz
	char E1;
	double * value1 ;
	double * value3 ;
	if (error != 2){
		E1 =  S -> valueType[index];
		value1 = S-> value[index];
		value3 = malloc(sizeof (double));
	}
	
	if (strCmpConstStr(&(S->a[index]), "id")==0){
	// E-> id
		// generuje instrukci ve ktere priradi hodnotu z adresy 1 do adresy 3
		if (error != 2)
			//tady asi instrukce
		SDeleteItem(S, 1);
		// adresa 3 je pote ulozena do zasobniku a pouzita pro dalsi vypocty
		SPushNeterm(S, E1, value3);
		return SYNTAX_OK;
	}


	else if (strCmpConstStr(&(S->a[index]), "(")==0){
		if (strCmpConstStr(&(S->a[index+1]), "E")==0){ 
			if (strCmpConstStr(&(S->a[index+2]), ")")==0){
			// E-> (E)
				if (error != 2){
					E1 = S-> valueType[index+1];
					value1 = S-> value[index+1];
					//tady instrukce
				}
				SDeleteItem(S, 3);
				SPushNeterm(S, E1, value3);
				return SYNTAX_OK;	
			} 		
		}
	}


		else if (strCmpConstStr(&(S->a[index]), "E")==0){
		
		if (strCmpConstStr(&(S->a[index+1]), "*")==0){
			if (strCmpConstStr(&(S->a[index+2]), "E")==0){
			// E-> E*E
			
				if (error != 2){
					char E2 = S->valueType[index+2];
					double *value2  = S->value[index+2];
				// nezapomen na kontrolu semantiky

					// generuje istrulci ve ktere se vynasoby hodnoty na adresach 1 a 2
					// a vysledek se ulozi na adresu 3 se kterou dale pracujeme
					
				}
				SDeleteItem(S, 3);
				// adresa 3 se ulozi do zasobniku a ceka na dalsi zpracovani
				SPushNeterm(S, E1, value3);
				return SYNTAX_OK;
			}
		} 



		else if (strCmpConstStr(&(S->a[index+1]), "/")==0){
			if (strCmpConstStr(&(S->a[index+2]), "E")==0){
			// E-> E/E
				if (error != 2){
					char E2 = S->valueType[index+2];
					double *value2 = S->value[index+2];
					//seman
					
					
				}	
				SDeleteItem(S, 3);
				SPushNeterm(S, E1, value3);
				return SYNTAX_OK;
			}
		} 


		else if (strCmpConstStr(&(S->a[index+1]), "+")==0){
			if (strCmpConstStr(&(S->a[index+2]), "E")==0){
			// E-> E+E
				if (error != 2){
					char E2 = S->valueType[index+2];
					double *value2  = S->value[index+2];
					//seman
					
					
				}
				SDeleteItem(S, 3);
				SPushNeterm(S, E1, value3);
				return SYNTAX_OK;
			}
		} 


		else if (strCmpConstStr(&(S->a[index+1]), "-")==0){
			if (strCmpConstStr(&(S->a[index+2]), "E")==0){
			// E-> E-E
				if (error != 2){
					char E2 = S->valueType[index+2];
					double *value2  = S->value[index+2];
					
					//seman
					
				}
				SDeleteItem(S, 3);
				SPushNeterm(S, E1, value3);
				return SYNTAX_OK;					
			}
		} 



		else if (strCmpConstStr(&(S->a[index+1]), ">")==0){
			if (strCmpConstStr(&(S->a[index+2]), "E")==0){
			// E-> E>E				
				if (error != 2){
					char E2 = S->valueType[index+2];
					double *value2  = S->value[index+2];
					
					//seman
					
				}
				SDeleteItem(S, 3);
				SPushNeterm(S, E1, value3);
				return SYNTAX_OK;
			}
		} 


		else if (strCmpConstStr(&(S->a[index+1]), ">=")==0){
			if (strCmpConstStr(&(S->a[index+2]), "E")==0){
			// E-> E>=E
				if (error != 2){
					char E2 = S->valueType[index+2];
					double *value2  = S->value[index+2];
					
					//seman
					// viz komentar k nasobeni
					
				}
				SDeleteItem(S, 3);
				SPushNeterm(S, E1, value3);
				return SYNTAX_OK;
			}	
		} 


		else if (strCmpConstStr(&(S->a[index+1]), "<")==0){
			if (strCmpConstStr(&(S->a[index+2]), "E")==0){
			// E-> E<E
				if (error != 2){
					char E2 = S->valueType[index+2];
					double *value2  = S->value[index+2];
					
				//seman
				}
				SDeleteItem(S, 3);
				SPushNeterm(S, E1, value3);
				return SYNTAX_OK;
			}
		} 



		else if (strCmpConstStr(&(S->a[index+1]), "<=")==0){
			if (strCmpConstStr(&(S->a[index+2]), "E")==0){
			// E-> E<=E
				char E2 = S->valueType[index+2];
				double *value2  = S->value[index+2];
				
				//seman
				SDeleteItem(S, 3);
				SPushNeterm(S, E1, value3);
				return SYNTAX_OK;
			}
		}



		 else if (strCmpConstStr(&(S->a[index+1]), "==")==0){
			if (strCmpConstStr(&(S->a[index+2]), "E")==0){
			// E-> E=E
				if (error != 2){
					char E2 = S->valueType[index+2];
					double *value2  = S->value[index+2];
					
					//seman
				}
				SDeleteItem(S, 3);
				SPushNeterm(S, E1, value3);
				return SYNTAX_OK;
			}
		} 



		else if (strCmpConstStr(&(S->a[index+1]), "!=")==0){
			if (strCmpConstStr(&(S->a[index+2]), "E")==0){
			// E-> E<>E
				if (error != 2){
					char E2 = S->valueType[index+2];
					double *value2  = S->value[index+2];
					
					
				//seman
				}
				SDeleteItem(S, 3);
				SPushNeterm(S, E1, value3);
				return SYNTAX_OK;
			}
		
		} 
			
	}
	
	return SYNTAX_ERROR;
	
}

//--------------EXPR---------------------------------------------------------------	
int comp_expr(){
	
	char result;
	int index;
	tStackTN stack;
	string a;

	strInit(&a);			
	StackInit(&stack);
	a = STopTerm (&stack);

	while(strCmpConstStr(&a,"$")!=0  &&(token !=TOK_LEFT_BRACE || token !=TOK_SEMICOLON)){

		result = select_ruler(&top,token);

		switch (result){
			case "=":
				SPushTerm(&stack);
				if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;	
			break;
			
			case "<":
				SAddTerm(&stack, '<');
				SPushTerm(&stack);
				if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
			break;
			
			case ">":
				if ((index=SSearchBracket(&stack))!= -1){
				strDelLastChar(&(stack.a[index]));
				
					// chyba v redukovat
					if (SReduction_expr(&stack, index + 1) == SYNTAX_ERROR){
						SDipose(&stack);
						error=2;
						return SYNTAX_ERROR;			
					}

					// neni co redukovat
					else{
				
							SDipose(&stack);
							error=2;
							return FALSE;
			}
				} 
			
			//prazdne misto v tabulce
			else{
				
				SDipose(&stack);
				error=2;
				return SYNTAX_ERROR;
			}

			break;
		}

		a = STopTerm (&stack);
	}

	*value = stack.value[1]; 	
	strFree(&a);
	SDipose(&stack);
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
	int type_token;
	
	result= type();
	if (result != SYNTAX_OK) return SYNTAX_ERROR;
///
	type_token= token;
	// dalsi token ID
	if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	if (token != TOK_ID) return SYNTAX_ERROR;
///
	if (tableInsert(global_table, &attr, type_token) == 1) return SEM_ERROR;
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
int parse(tSymbolTable *ST, tListOfInstr *instrList){

  int result;
  table = ST;
  list = instrList;
  strInit(&attr);
	

	if((token = getNextToken(&atrr))== LEX_ERROR)
		result=LEX_ERROR;
	else
		result=program();	// volam prvni neterminal 
	
	
	return result;
}