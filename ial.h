/*
 *	ial.h
 *	Popis: hlavicka vestavenych funkci
 *	Autor: Petr Gibis, Petr Zufan
 *
 */


int length(string s);
string concat(string s1, string s2);
string substr(string s, int pos, int n);

int find(string *s, string *str);
void FailCreate(char *s, int delka, int fail[]);
string sort(string s);
void siftdown(char *str1, int left, int right);

