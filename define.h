//SCANER - Lex. analyza
/** stavy - cisla od 1 do 100 */
#define STATE_START 1
#define STATE_SLASH 2 // lomitko - /
#define STATE_LINE_COMMENT 3
#define STATE_BLOCK_COMMENT 4
#define STATE_BLOCK_COMMENT_STAR 5
#define STATE_GREATER_THAN 6
#define STATE_ALPHANUM 7
#define STATE_EQUALS 8
#define STATE_DIGIT 9
#define STATE_FLOATING_POINT 10 // STAV = cislo s radovou teckou?
#define STATE_LESS_THAN 11
#define STATE_EXPONENT 12
#define STATE_MORE_IN_EXPONENT 13
#define STATE_MORE_IN_EXPONENT2 15
#define STATE_STRING 16
#define STATE_ESCAPE_SEQUENCE 17
#define STATE_HEXA_CHAR 18
#define STATE_HEXA_CHAR_SECOND 19


/** tokeny - cisla od 101 do n */
//klíèová slova
#define TOK_INT 107         // keyword int
#define TOK_DOUBLE 108      // keyword double
#define TOK_ELSE 115    // key word else
#define TOK_IF 116      // key word if
#define TOK_RETURN 117  // key word return
#define TOK_STRING 118  // key word string
#define TOK_AUTO 119    // key word auto
#define TOK_CIN 120     // key word cin
#define TOK_FOR 121     // key word for
#define TOK_COUT 122   // key word count

//operátory
#define TOK_ADDITION 101        // +
#define TOK_SUBTRACTION 102     // -
#define TOK_MULTIPLICATION 103  // *
#define TOK_DIVISION 104        // /
#define TOK_GREATER_THAN 105            // >
#define TOK_GREATER_THAN_OR_EQUAL 106   // >=
#define TOK_LESS_THAN_OR_EQUAL 127 //<=
#define TOK_LESS_THAN 128 // <
#define TOK_COMPARISON 111  // ==
#define TOK_EQUALS 112      // =

//identifikátor
#define TOK_ID 109          // ID

//øetìzecová konstanta
#define TOK_STR 134

//reálná konstanta
#define TOK_REAL_NUMBER 133 // realnumber

//celoèíselná konstanta
#define TOK_DECIMAL_NUMBER 113 // cele cislo (rozsah C-int)

//double
#define TOK_FLOATING_POINT_NUMBER 114 // necele cislo (0.1, 5.889, ...)

//jednotlivé znaky
#define TOK_LEFT_BRACKET 123    // (
#define TOK_RIGHT_BRACKET 124    // )
#define TOK_DOT 125 // .
#define TOK_COMMA 125 // ,
#define TOK_LEFT_BRACE 129 // {
#define TOK_RIGHT_BRACE 130 // }
#define TOK_SEMICOLON 110   // ;
#define TOK_DOUBLE_ARROW_RIGHT 131 // >>
#define TOK_DOUBLE_ARROW_LEFT 132 // <<

//spec. znak
#define TOK_END_OF_FILE 126 // EOF



/** ErrorCodes - cisla od  do n */
#define LEX_ERROR 1 // chyba lex. analyzy

