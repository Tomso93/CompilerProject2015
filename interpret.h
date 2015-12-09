/*
 *	interpret.h
 *	zaverecna interpretace ridiciho programu
 *	Autor: Petr Zufan
 *	
 */

// to do 
//int substring();		//najde podretezec

int interpret(glabal *GTS); 	//hlavni funkce. prochazi seznam instrukci
       				//a vola prislusne funkce

//IMOV
//dest = src1
int move(tData *src1, tData *dest);			   

//IADD
//dest = src1 + src2
int addition(tData *src1, tData *src2, tData *dest);	  

//ISUB
//dest = src1 - src2
int substraction(tData *src1, tData *src2, tData *dest);

//IMUL
//dest = src1 * src2
int multiplication(tData *src1, tData *src2, tData *dest); 

//IDIV
//dest = src1 / src2
int division(tData *src1, tData *src2, tData *dest);

//IEQUAL
//dest = (src1 == src2)
int equal(tData *src1, tData *src2, tData *dest);

//ISMALL
//dest = (src1 < src2)
int smaller(tData *src1, tData *src2, tData *dest);

//IBIG
//dest = (src1 > src2)
int bigger(tData *src1, tData *src2, tData *dest);

//IEQSM
//dest = (src1 <= src2)
int equalsmaller(tData *src1, tData *src2, tData *dest);

//IEQBG
//dest = (src1 >= src2)
int equalbigger(tData *src1, tData *src2, tData *dest);

//INOTEQ
//dest = (src1 != src2)
int notequal(tData *src1, tData *src2, tData *dest);

//INOT
//dest = !(src1)
int negation(tData *src1, tData *dest);	

//IGOTO
//jdi na instrukci ilabel pro kterou plati:
//igoto dest == ilabel src1
int jump(tData *dest, TinstList *LOI);			  

//IIFGOTO
//kdyz plati src1
//jdi na instrukci ilabel pro kterou plati
//iifgoto dest == ilabel src1
int jumpif(tData *src1, tData *dest, TinstList *LOI);	  

//IREAD
//ze stdin cte do dest
int read(tData *dest);		

//IREADI
//ze stdin nacte int do dest
int readint(tData *dest);		


//IREADD
//ze stdin nacte double do dest
int readdouble(tData *dest);		

//IREADS
//ze stdin nacte string do dest
int readstring(tData *dest);		

//IWRITE
//zapise src1 na stdout
int write(tData *src1);			

//ISORT
//dest = sort(src1) funkce v ial.c
int sortme(tData *src1, tData *dest);	

//IFIND
//dest = find(src1, src2)
//vestavena funkce
int findme(tData *src1, tData *src2, tData *dest);  

//ILENGTH
//dest = length(src1) 
//vestavena funkce
int lengthstring(tData *src1, tData *dest);	

//ICAT
//dest=concat(src1,src2)
//vestavena funkce
int concatenate(tData *src1, tData *src2, tData *dest);

//ISUBSTR
//dest=substr(src1, src2, src3)
//vestavena funkce
int substring(tData *src1, tData *src2, tData *src3, tData *dest);

//IPRECALL
//vytvori novy ramec promennych z lokalni tabulky symbolu funkce
int precall(string *funcName, globalTS *GTS, struct Frame *newF);

//IPAR
//inicializuje parametr v source1 urceny cotacem cnt hodnotou v source2,
//vlozi ho do ramce promennych
int parametr(tGData *source1, tLData *source2, struct Frame *newF, int cnt);

//ICALL
//vlozi ramec do zasobniku
//vlozi instrukce do seznamu 
int call(tGData *source1, struct Frame *newF, TinstList *LOI, Tstackframe *S);

//IRET
//odstrani ramec z vrcholu zasobniku
//navratovou hodnotu funkce ulozi do promenne #RETURN
int ret(tLData *source1, Tstackframe *S);
