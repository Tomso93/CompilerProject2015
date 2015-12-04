/*
 *	ial.c
 *	Popis: vestavene funkce jazyka
 *	Autor: Petr Gibis, Petr Zufan
 *
 */

#include <string.h>
#include <stdbool.h>
#include "define.h"
#include "str.h"
#include "ial.h"


int length(string s)
//funkce porovnává znaky retezce a urcuje pocet znaku
{

    char* p = s.str;
    int cnt =0;
    int lng = strlen(s.str);
    for(int i = 0;i<lng;i++){
        if((p[i] == '\\')&&(p[i+1] == 'n')){
            cnt--;
        }
        cnt++;
    }
	return cnt;
}

string concat(string s1, string s2)
//funkce spoji retezce s1 a s2
{

    string ccStr;
    strCopyString(&ccStr,&s1);                        //prekopirovani prvniho retezce s1 do ccStr
    strcat(ccStr.str,s2.str);                               //pridani druheho retezce s2 do ccStr
    return ccStr;
}

string substr(string s, int pos, int n)
//funkce vrati podretezec sStr
{
    string sStr;
    char* p = s.str;
    p = p + pos;
    strncpy(sStr.str,p,n);
	return sStr;
}

int find(string *s, string *str1){
//funkce vyhleda podretezec v retezci, uziti KMP algoritmu

  int delkas = strGetLength(s);
  int delkastr1 = strGetLength(str1);
  int fail[delkas];
  FailCreate(s->str, delkas, fail);
  int is = 0;
  int istr1 = 0;

  while ( (istr1 < delkastr1) && (is < delkas) ){
    if (is != -1){
      if (str1->str[istr1] != s->str[is]){
        is = fail[is];
      }else{
        is++;
        istr1++;
      }
    }else{
      is++;
      istr1++;
    }
  }
  if (is >= delkas){
    return (istr1 - delkas);
  }else{
    return istr1;
  }
return 0;
}

void FailCreate(char *s, int delka, int fail[]){
//funkce vektoru FAIL
//urcuje pozici kam se algoritmus musi vratit v porovnavani retezcu

  int k,r = 0;
  fail[0] = 0;

  for (k = 1; k < delka; k++){
    r = fail[k-1];
    if (r > 0){
      while (s[r] != s[k-1]){
        r = fail[r];
      }
    }
    fail[k] = r+1;
  }
}

string sort(string s)
{
  string* str1 = &s;

  char pom;
  int right = (strGetLength(str1) - 1);
  int left = (right % 2);
  for (int i = left; i >= 0; i--){
    siftdown(str1->str, i, right);
  }
  for (right=(strGetLength(str1)-1); right > 0; right--){
    pom = str1->str[0];
    str1->str[0] = str1->str[right];
    str1->str[right] = pom;
    siftdown(str1->str, 0, right-1);
  }
  return *str1;
}

void siftdown(char *str1, int left, int right){
  int i = left;
  int j = (2*i);
  char temp = str1[i];
  bool cont = (j <= right);
  char pom;
  while (cont){
    if (j < right){
      if (str1[j] < str1[j+1]){
        j++;
      }
    }
    if (temp >= str1[j]){
      cont = false;
    }else{
      pom = str1[i];
      str1[i] = str1[j];
      str1[j] = pom;
      i = j;
      j = (2*i);
      cont = (j <= right);
    }
  }
  str1[i] = temp;
}
