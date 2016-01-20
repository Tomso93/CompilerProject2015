/*
 *	interpret.h
 *	zaverecna interpretace ridiciho programu
 *	Autor: Petr Zufan
 *	
 */

// to do 
//int substring();		//najde podretezec

int interpret(globalTS *GTS); 	//hlavni funkce. prochazi seznam instrukci
       				//a vola prislusne funkce

//IMOV
//dest = src1
int move(tLData *src1, tLData *dest);			   

//IADD
//dest = src1 + src2
int addition(tLData *src1, tLData *src2, tLData *dest);	  

//ISUB
//dest = src1 - src2
int substraction(tLData *src1, tLData *src2, tLData *dest);

//IMUL
//dest = src1 * src2
int multiplication(tLData *src1, tLData *src2, tLData *dest); 

//IDIV
//dest = src1 / src2
int division(tLData *src1, tLData *src2, tLData *dest);

//IEQUAL
//dest = (src1 == src2)
int equal(tLData *src1, tLData *src2, tLData *dest);

//ISMALL
//dest = (src1 < src2)
int smaller(tLData *src1, tLData *src2, tLData *dest);

//IBIG
//dest = (src1 > src2)
int bigger(tLData *src1, tLData *src2, tLData *dest);

//IEQSM
//dest = (src1 <= src2)
int equalsmaller(tLData *src1, tLData *src2, tLData *dest);

//IEQBG
//dest = (src1 >= src2)
int equalbigger(tLData *src1, tLData *src2, tLData *dest);

//INOTEQ
//dest = (src1 != src2)
int notequal(tLData *src1, tLData *src2, tLData *dest);

//INOT
//dest = !(src1)
int negation(tLData *src1, tLData *dest);	

//IGOTO
//jdi na instrukci ilabel pro kterou plati:
//igoto dest == ilabel src1
int jump(string *dest, TinstList *LOI);			  

//IIFGOTO
//kdyz plati src1
//jdi na instrukci ilabel pro kterou plati
//iifgoto dest == ilabel src1
int jumpif(tLData *src1, string *dest, TinstList *LOI);	  

//IREAD
//ze stdin cte do dest
int read(tLData *dest);		

//IREADI
//ze stdin nacte int do dest
int readint(tLData *dest);		


//IREADD
//ze stdin nacte double do dest
int readdouble(tLData *dest);		

//IREADS
//ze stdin nacte string do dest
int readstring(tLData *dest);		

//IWRITE
//zapise src1 na stdout
int write(tLData *src1);			

//ISORT
//dest = sort(src1) funkce v ial.c
int sortme(tLData *src1, tLData *dest);	

//IFIND
//dest = find(src1, src2)
//vestavena funkce
int findme(tLData *src1, tLData *src2, tLData *dest);  

//ILENGTH
//dest = length(src1) 
//vestavena funkce
int lengthstring(tLData *src1, tLData *dest);	

//ICAT
//dest=concat(src1,src2)
//vestavena funkce
int concatenate(tLData *src1, tLData *src2, tLData *dest);

//ISUBSTR
//dest=substr(src1, src2, src3)
//vestavena funkce
int substring(tLData *src1, tLData *src2, tLData *src3, tLData *dest);

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
