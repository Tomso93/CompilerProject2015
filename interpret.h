/*
 *	interpret.h
 *	zaverecna interpretace ridiciho programu
 *	Autor: Petr Zufan
 *	
 */

// to do 
//int substr();		//najde podretezec

int interpret(TinstList *LOI); 	//hlavni funkce. prochazi seznam instrukci
       				//a vola prislusne funkce

//IMOV
//dest = src1
int move(Tdata *src1, Tdata *dest);			   

//IADD
//dest = src1 + src2
int addition(Tdata *src1, Tdata *src2, Tdata *dest);	  

//ISUB
//dest = src1 - src2
int substraction(Tdata *src1, Tdata *src2, Tdata *dest);

//IMUL
//dest = src1 * src2
int multiplication(Tdata *src1, Tdata *src2, Tdata *dest); 

//IDIV
//dest = src1 / src2
int division(Tdata *src1, Tdata *src2, Tdata *dest);

//IEQUAL
//dest = (src1 == src2)
int equal(Tdata *src1, Tdata *src2, Tdata *dest);

//ISMALL
//dest = (src1 < src2)
int smaller(Tdata *src1, Tdata *src2, Tdata *dest);

//IBIG
//dest = (src1 > src2)
int bigger(Tdata *src1, Tdata *src2, Tdata *dest);

//IEQSM
//dest = (src1 <= src2)
int equalsmaller(Tdata *src1, Tdata *src2, Tdata *dest);

//IEQBG
//dest = (src1 >= src2)
int equalbigger(Tdata *src1, Tdata *src2, Tdata *dest);

//INOTEQ
//dest = (src1 != src2)
int notequal(Tdata *src1, Tdata *src2, Tdata *dest);

//INOT
//dest = !(src1)
int negation(Tdata *src1, Tdata *dest);	

//IGOTO
//jdi na instrukci ilabel pro kterou platí:
//igoto dest == ilabel src1
int jump(Tdata *dest);			  

//IIFGOTO
//kdyz plati src1
//jdi na instrukci ilabel pro kterou plati
//iifgoto dest == ilabel src1
int jumpif(Tdata *src1, Tdata *dest);	  

//IREAD
//ze stdin cte do dest
int read(Tdata *dest);		

//IREADI
//ze stdin nacte int do dest
int readint(Tdata *dest);		


//IREADD
//ze stdin nacte double do dest
int readdouble(Tdata *dest);		

//IREADS
//ze stdin nacte string do dest
int readstring(Tdata *dest);		

//IWRITE
//zapise src1 na stdout
int write(Tdata *src1);			

//ISORT
//dest = sort(src1) funkce v ial.c
int sortme(Tdata *src1, Tdata *dest);	

//IFIND
//dest = find(src1, src2)
//funkce v ial.c
int findme(Tdata *src1, Tdata *src2, Tdata *dest);  

//ILENGTH
//dest = length(src1) 
//funkce v inbuild.c
int lengthstring(Tdata *src1, Tdata *dest);	

//ICAT
//dest=concat(src1,src2)
//funkce v inbuild.c
int concatenate(Tdata *src1, Tdata *src2, Tdata *dest);
