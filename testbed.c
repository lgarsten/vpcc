// testbed.c

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"

static char *get_token_name(int sym);
static char *intstr_to_charstr(int *is);

int main() {
	int token = 1;

	init_scanner();

	while(token != -1) {
		token = getSymbol();
		printf("Token: %s (%d)\n", get_token_name(token), token);
		if(token == IDENTIFIER)	printf("\t'%s'\n", intstr_to_charstr(identifier));
		if(token == INTEGER) 		printf("\t%d\n", integer);
	}

	return EXIT_SUCCESS;
}



static char *get_token_name(int sym) {
	if(sym == -1)			return "end of input";
	if(sym == -2)			return "error";

	// other tokens
	if(sym == IDENTIFIER)	return "IDENTIFIER";
	if(sym == INTEGER)		return "INTEGER";

	// keyword tokens
	if(sym == VOID)			return "VOID";
	if(sym == IF)			return "IF";
	if(sym == INT)			return "INT";
	if(sym == WHILE)		return "WHILE";
	if(sym == ELSE)			return "ELSE";
	if(sym == RETURN)		return "RETURN";

	// special character and operator tokens
	if(sym == SEMICOLON)	return "SEMICOLON";
	if(sym == ASTERISK)		return "ASTERISK";
	if(sym == LPARENS)		return "LPARENS";
	if(sym == RPARENS)		return "RPARENS";
	if(sym == LBRACE)		return "LBRACE";
	if(sym == RBRACE)		return "RBRACE";
	if(sym == EQUAL)		return "EQUAL";
	if(sym == PLUS)			return "PLUS";
	if(sym == MINUS)		return "MINUS";
	if(sym == ASSIGN)		return "ASSIGN";
//	if(sym == GT)			return "GT";
	if(sym == GTEQ)			return "GTEQ";
//	if(sym == LT)			return "LT";
	if(sym == LTEQ)			return "LTEQ";
	if(sym == COMMA)		return "COMMA";
//	if(sym == )	return "";
//	if(sym == )	return "";
//	if(sym == )	return "";
//	if(sym == )	return "";
//	if(sym == )	return "";
//	if(sym == )	return "";
//	if(sym == )	return "";

	// FIXME: add new tokens here!

	return "unknown";
}

static char *intstr_to_charstr(int *is) {
	int length = 0;
	char *cs = malloc(maxIdentifierLength);

	while((cs[length] = is[length]) != 0) length++;

	return cs;
}

