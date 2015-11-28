/*
 *	interpret.h
 *	zaverecna interpretace ridiciho programu
 *	Autor: Petr Zufan
 *	
 */

// to do 
//int substring();		//najde podretezec

int interpret(TinstList *LOI); 	//hlavni funkce. prochazi seznam instrukci
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
//jdi na instrukci ilabel pro kterou platí:
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
//funkce v ial.c
int findme(tData *src1, tData *src2, tData *dest);  

//ILENGTH
//dest = length(src1) 
//funkce v inbuild.c
int lengthstring(tData *src1, tData *dest);	

//ICAT
//dest=concat(src1,src2)
//funkce v inbuild.c
int concatenate(tData *src1, tData *src2, tData *dest);

//ISUBSTR
//jeste nedela nic. vrati chybu
int substring(tData *src1, tData *src2, tData *dest);
