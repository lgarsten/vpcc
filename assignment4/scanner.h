#ifndef _SCANNER_H
#define _SCANNER_H 1

int init_scanner();
int getSymbol();
int printf(char *fmt, ...);

void dump_symbol_table(int *symbolTableCursor);

extern int* identifier; // stores scanned identifier
extern int integer; // stores scanned integer
extern int lineNR;

extern int maxIdentifierLength;


// other tokens
extern int IDENTIFIER;
extern int INTEGER;

// keyword tokens
extern int VOID;
extern int IF;
extern int INT;
extern int WHILE;
extern int ELSE;
extern int RETURN;

// special character and operator tokens
extern int SEMICOLON;
extern int ASTERISK;
extern int SLASH;
extern int LPARENS;
extern int RPARENS;
extern int LBRACE;
extern int RBRACE;
extern int EQUAL;
extern int PLUS;
extern int MINUS;
extern int ASSIGN;
extern int GT;
extern int GTEQ;
extern int LT;
extern int LTEQ;
extern int COMMA;
extern int SLASH;
extern int NOT;
extern int NOTEQ;

// error codes
int E_EXPRESSION;
int E_TERM;
int E_WHILE;
int E_PROCEDURE;
int E_CALL;
int E_FACTOR;
int E_CAST;
int E_IF;
int E_DECLARATION;
int E_ASSIGNMENT;
int E_VARIABLE;
int E_STATEMENT;
int E_VOID;
int E_TYPE;
int E_RELATION_EXPRESSION;
int E_RETURN;
int E_ELSE;
int E_VARIABLEORPOCEDURE;

#endif

