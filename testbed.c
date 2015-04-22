// testbed.c

#include "mipster.h"
#include "scanner.h"

char *get_token_name(int sym);
char *intstr_to_charstr(int *is);
int putchar(int c);
int print_int(int i);

int main() {
	int token;
	token = 1;
	
	init_scanner();
	
	while(token != -1) {
		token = getSymbol();
		
		// print token name		
		mipster_syscall(4004, 1, (unsigned int)get_token_name(token), 100, 0);
		mipster_syscall(4004, 1, (unsigned int)"\n", 1, 0);
		
		// print identifier
		if(token == IDENTIFIER) {
			int *identifier_ptr;
			identifier_ptr = identifier;
			
			while (*identifier_ptr != 0) {
				mipster_syscall(4004, 1, (unsigned int)identifier_ptr, 100, 0);
				identifier_ptr++;
			}
			
			mipster_syscall(4004, 1, (unsigned int)"\n", 1, 0);
		}
			
		// print integer		
		if(token == INTEGER) {
			print_int(integer);
			mipster_syscall(4004, 1, (unsigned int)"\n", 1, 0);
		}		
	}
	mipster_exit(0);
}

int putchar(int c) {
	// this relies on integers being stored little-endian, i.e. least significant byte first
	// somehow mipster is missing instructions that prevent assigning a char variable from an int value.

	// arguments: syscall number, file handle, pointer to userspace buffer, buffer length, constant 0
	mipster_syscall(4004, 1, (unsigned int)&c, 1, 0); // __NR_write

	return c;
}

int print_int(int i) {
	int sign;
	int pos = 0; // index of next free position in buffer
	int buffer[16];
	if(i < 0) {
		sign = -1; i *= -1;
		putchar(45); // '-'
	}

	if(i == 0) {
		// i = 0: special case because it wouldn't output any digits at all
		buffer[pos] = 48; // '0'
		pos++;
	} else {
		// i > 0: guaranteed to have at least one loop iteration
		while(i != 0) {
			buffer[pos] = 48 + (i % 10);
			pos++;
			i /= 10;
		}
	}

	while(pos > 0) {
		pos--;
		putchar(buffer[pos]);
	}

	return 0;
}

char *get_token_name(int sym) {
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

char *intstr_to_charstr(int *is) {
	int length = 0;
	char *cs = mipster_malloc(maxIdentifierLength * sizeof(char));

	while(is[length] != 0){
		cs[length] = is[length];
		length++;
	} 

	return cs;
}

