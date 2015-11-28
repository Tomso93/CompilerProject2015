/*
 *	ial.h
 *	vestavene funkce jazyka.
 *	Autor: Petr Zufan
 *	
 */

int find(string *s, string *str);
void FailCreate(char *s, int delka, int fail[]);
string *sort(string *str);
void siftdown(char *str1, int left, int right);
string concat(string s1, string s2);
