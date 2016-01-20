#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include "str.h"
#include "define.h"
#include "lex.h"

FILE * sourceFile;

void set_source_file(FILE *f){
    sourceFile = f;
}

int getNextToken(string *attr){
    char c;
    int stav = STATE_START;
    strClear(attr);

    while(1){
        c = getc(sourceFile);

        switch(stav){
            case STATE_START:
                if(isspace(c)){
                    stav = STATE_START;
                }
                else if(isalpha(c) || c == '_'){ // znaky: a-zA-Z nebo _
                    strAddChar(attr, c);
                    stav = STATE_ALPHANUM;
                }
                else if(isdigit(c)){ // cisla
                    strAddChar(attr, c);
                    stav = STATE_DIGIT;
                }
                else if(c == 34) { // "
                    //printf("%c", c);
                   // strAddChar(attr, c);
					stav = STATE_STRING;
                }
                else {
                    switch(c){
                        case '+':
                            return TOK_ADDITION;
                            break;
                        case '-':
                            return TOK_SUBTRACTION;
                            break;
                        case '*':
                            return TOK_MULTIPLICATION;
                            break;
                        case '/':
                            stav = STATE_SLASH;
                            break;
                        case '<':
                            stav = STATE_LESS_THAN;
                            break;
                        case '>':
                            stav = STATE_GREATER_THAN;
                            break;
                        case '=':
                            stav = STATE_EQUALS;
                            break;
                        case '!':
                            stav = STATE_INEQUALITY;
                            break;
                        case ';':
                            return TOK_SEMICOLON;
                            break;
                        case '(':
                            return TOK_LEFT_BRACKET;
                            break;
                        case ')':
                            return TOK_RIGHT_BRACKET;
                            break;
                        case '.':
                            return TOK_DOT;
                            break;
                        case ',':
                            return TOK_COMMA;
                            break;
                        case EOF :
                            return TOK_END_OF_FILE;
                            break;
                        case '{' :
                            return TOK_LEFT_BRACE;
                            break;
                        case '}' :
                            return TOK_RIGHT_BRACE;
                            break;
                        default:
                            printf("defualt %c", c);
                            break;
                    }
                }
                break;
            case STATE_INEQUALITY:
                if(c == '='){
                    return TOK_INEQUALITY;
                } else {
                    ungetc (c,sourceFile);
                    return LEX_ERROR;
                }
                break;
            case STATE_STRING:
                if(c == '"'){
 //                   strAddChar(attr, c);
                    return TOK_STR;
                } else if(c == 92){ // ASCII 92 je zpetne lomitko
 //                   strAddChar(attr, c);
                    stav = STATE_ESCAPE_SEQUENCE;
                } else if (c == EOF){
                    ungetc(c, sourceFile);
                    return LEX_ERROR;
                } else if(c != 34 && c > 31){
                    strAddChar(attr, c);
                } else {
                    ungetc(c, sourceFile);
                    return LEX_ERROR;
                }
                break;
            case STATE_ESCAPE_SEQUENCE:
                if(c == 'n'){
         	    strAddChar(attr, '\n');
                    stav = STATE_STRING;
		} else if (c == 92){
		    strAddChar(attr, '\\');
                    stav = STATE_STRING;
		} else if (c == 't'){
		    strAddChar(attr, '\t');
                    stav = STATE_STRING;
		} else if (c == '"'){
//                    strAddChar(attr, c);
                    stav = STATE_STRING;
                } else if (c == 'x'){
                    strAddChar(attr, 92);
                    strAddChar(attr, c);
                    stav = STATE_HEXA_CHAR;
                } else {
                    ungetc(c, sourceFile);
                    return LEX_ERROR;
                }
                break;
            case STATE_HEXA_CHAR:
                if((c >= 48 && c <= 57) || (c >= 65 && c <= 70) || (c >= 97 && c <= 102)){
                    strAddChar(attr, c);
                    stav = STATE_HEXA_CHAR_SECOND;
                } else {
                    ungetc(c, sourceFile);
                    return LEX_ERROR;
                }
                break;
            case STATE_HEXA_CHAR_SECOND:
                if((c >= 48 && c <= 57) || (c >= 65 && c <= 70) || (c >= 97 && c <= 102)){
                    strAddChar(attr, c);
                    stav = STATE_STRING;
                } else {
                    ungetc(c, sourceFile);
                    return LEX_ERROR;
                }
                break;
            case STATE_DIGIT:
                if(isdigit(c)){ // prislo dalsi cislo
                    strAddChar(attr, c);
                }
                else if(c == '.'){
                    strAddChar(attr, c);
                    stav = STATE_FLOATING_POINT;
                }
                else if(c == 'E' || c == 'e'){
                    strAddChar(attr, c);
                    stav = STATE_EXPONENT;
                }
                else {
                    ungetc (c,sourceFile);
                    return TOK_DECIMAL_NUMBER;
                }
                break;
            case STATE_FLOATING_POINT:
                if(isdigit(c)){
                    strAddChar(attr, c);
                }
                else if(c == 'E' || c == 'e'){
                    strAddChar(attr, c);
                    stav = STATE_EXPONENT;
                }
                else {
                    ungetc (c,sourceFile);
                    return TOK_FLOATING_POINT_NUMBER;
                }
                break;
            case STATE_EXPONENT://
                if(isdigit(c) != 0) {
                    strAddChar(attr, c);
                    stav = STATE_MORE_IN_EXPONENT;
                }
                else if(c == '+' || c == '-') {
                    strAddChar(attr, c);
                    stav = STATE_MORE_IN_EXPONENT;
                }
                else {
                    ungetc(c, sourceFile);
                    return LEX_ERROR;
                }
                break;

            case STATE_MORE_IN_EXPONENT:
                if(isdigit(c) != 0) {
                    strAddChar(attr, c);
                    stav = STATE_MORE_IN_EXPONENT2;
                } else {
                    ungetc(c, sourceFile);
                    return LEX_ERROR;
                }
                break;
            case STATE_MORE_IN_EXPONENT2:
                if(isdigit(c) != 0) {
                    strAddChar(attr, c);
                }
                else if(c == ' ' || c == ';' || c == ')' || c == '+' || c == '-' || c == '/' || c == '*'){
                    ungetc(c, sourceFile);
                    return TOK_FLOATING_POINT_NUMBER;
                }
                else{
                    ungetc(c, sourceFile);
                    return LEX_ERROR;
                }
                break;
            case STATE_EQUALS:
                if(c == '='){
                    return TOK_COMPARISON;
                } else {
                    ungetc (c,sourceFile);
                    return TOK_EQUALS;
                    break;
                }
                break;
            case STATE_ALPHANUM:
                if(!(isalnum(c) || c == '_')){ // kdyz neni toto: a-zA-Z0-9 nebo _
                    //printf("STATE_ALPHANUM: %s", attr->str);
                    ungetc(c, sourceFile);
                    if(0 == strCmpConstStr(attr, "int")){
                        return TOK_INT;
                    }
                    else if(0 == strCmpConstStr(attr, "double")){
                        return TOK_DOUBLE;
                    }
                    else if(0 == strCmpConstStr(attr, "else")){
                        return TOK_ELSE;
                    }
                    else if(0 == strCmpConstStr(attr, "if")){
                        return TOK_IF;
                    }
                    else if(0 == strCmpConstStr(attr, "return")){
                        return TOK_RETURN;
                    }
                    else if(0 == strCmpConstStr(attr, "string")){
                        return TOK_STRING;
                    }
                    else if(0 == strCmpConstStr(attr, "auto")){
                        return TOK_AUTO;
                    }
                    else if(0 == strCmpConstStr(attr, "cin")){
                        return TOK_CIN;
                    }
                    else if(0 == strCmpConstStr(attr, "for")){
                        return TOK_FOR;
                    }
                    else if(0 == strCmpConstStr(attr, "cout")){
                        return TOK_COUT;
                    }
                    else {
                        return TOK_ID;
                    }
                } else {
                    strAddChar(attr, c);
                }
                break;
            case STATE_GREATER_THAN:
                switch(c){
                    case '=':
                        return TOK_GREATER_THAN_OR_EQUAL;
                        break;
                    case '>':
                        return TOK_DOUBLE_ARROW_RIGHT;
                        break;
                    default:
                        ungetc (c,sourceFile);
                        return TOK_GREATER_THAN;
                        break;
                }
                break;
            case STATE_LESS_THAN:
                switch(c){
                    case '=':
                        return TOK_LESS_THAN_OR_EQUAL;
                        break;
                    case '<':
                        return TOK_DOUBLE_ARROW_LEFT;
                        break;
                    default:
                        ungetc (c,sourceFile);
                        return TOK_LESS_THAN;
                        break;
                }
                break;
            case STATE_SLASH:
                switch(c){
                    case '/':
                        stav = STATE_LINE_COMMENT;
                        break;
                    case '*':
                        stav = STATE_BLOCK_COMMENT;
                        break;
                    default:
                        ungetc (c,sourceFile);
                        return TOK_DIVISION;
                        break;
                }
            case STATE_LINE_COMMENT:
                if(c == '\n'){
                    stav = STATE_START;
                }
                break;
            case STATE_BLOCK_COMMENT:
                if(c == '*'){
                    stav = STATE_BLOCK_COMMENT_STAR;
                }
                break;
            case STATE_BLOCK_COMMENT_STAR:
                if(c == '/'){
                    stav = STATE_START;
                }
                break;
            default:
                break;
        }
    }
    return LEX_ERROR;
}



