/*
 *	ial.c
 *	vestavene funkce jazyka.
 *	Autor: Petr Zufan
 *	
 */

#include <string.h>
#include <stdbool.h>
#include "define.h"
#include "str.h"
#include "ial.h"

//-------------vyhledavani-----------------------------------------------------

int find(string *s, string *str1){
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
}

void FailCreate(char *s, int delka, int fail[]){
  int k,r;
//  int fail[delka];
  
  fail[0] = 0;
  for (k = 1; k < delka; k++){
    r = fail[k-1];
    if (r > 0){
      while (s[r] != s[k]){
        r = fail[r-1];
      }
    }
    fail[k] = r+1;
  }
}

//------------------rezeni-----------------------------------------------------

int sort(string *str0, string *str1){

  strCopyString(str1, str0);

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
  return 0;
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

