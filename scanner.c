#include <stdio.h>
#include <stdlib.h>

int character; // most recently read character
//int symbol; // most recently recognized symbol

// tokens

// keyword tokens
int IDENTIFIER;
int INTEGER;
int VOID;
int IF;
int INT;
int WHILE;
int ELSE;
int RETURN;

// special character and operator tokens
int SEMICOLON;
int ASTERISK;
int LPARENS;
int RPARENS;
int LBRACE;
int RBRACE;
int EQUAL;
int PLUS;
int MINUS;
int ASSIGN;
//int GT;
int GTEQ;
//int LT;
int LTEQ;
int COMMA;
int NOT;
int NOTEQ;
//int ;
//int ;
//int ;
//int ;
//int ;
//int ;

// FIXME: add new tokens here!

//...

int* identifier; // stores scanned identifier
int integer; // stores scanned integer

// maximum number of characters in an identifier
// set as needed
int maxIdentifierLength;
int lineNR;

// prototypes
static char *intstr_to_charstr(int *is);
static char *get_token_name(int sym);

int init_scanner() {
	character = getchar();
	maxIdentifierLength = 64;
	lineNR = 1;

	// other tokens
	IDENTIFIER	= 1;
	INTEGER		= 2;

	// keyword tokens
	VOID			= 3;
	IF			= 5;
	INT			= 6;
	WHILE		= 7;
	ELSE			= 8;
	RETURN		= 9;

	// special character and operator tokens
	SEMICOLON		= 101;
	ASTERISK		= 102;
	LPARENS		= 103;
	RPARENS		= 104;
	LBRACE		= 105;
	RBRACE		= 106;
	EQUAL		= 107;
	PLUS			= 108;
	MINUS		= 109;
	ASSIGN		= 110;
//	GT			= 111;
	GTEQ			= 112; // greater than or equal
//	LT			= 113;
	LTEQ			= 114; // less than or equal
	COMMA		= 115;
     NOT			= 116;
	NOTEQ		= 117;
//			= 118;
//			= 119;
//			= 120;


	// FIXME: add new tokens here!
	// perl -e 'print "//\t\t\t= $_;\n" for(108..120)'


}

int getSymbol() {
	int* identifierCursor;
	int identifierLength;

	if (findNextCharacter() == -1)
		return -1;
	//printf("c: %c (%d)\n", character, character);

	if (isCharacterLetter()) {
		// found identifier or keyword
		identifier = malloc(maxIdentifierLength * 4);

		identifierCursor = identifier;
		identifierLength = 1;

		while (isCharacterLetterOrDigitOrUnderscore()) {
			if (identifierLength >= maxIdentifierLength)
				exit(-1); // identifier too long

			*identifierCursor = character;
			identifierCursor = identifierCursor + 1;
			identifierLength = identifierLength + 1;

			character = getchar();

		}

		*identifierCursor = 0; // null terminated string

		return identifierOrKeyword();

	} else if (isCharacterDigit()) {
		// found integer constant
		integer = 0;

		while (isCharacterDigit()) {
			// caution: overflows remain undetected
			integer = integer * 10 + character - 48;

			character = getchar();
		}

		return INTEGER;
	} else if (character == 59) { // ASCII code 59 = ;
		character = getchar();

		return SEMICOLON;

	} else if (character == 42) { // "*" => 42
		character = getchar();
		return ASTERISK;

	} else if (character == 40) { // "(" => 40
		character = getchar();
		return LPARENS;

	} else if (character == 41) { // ")" => 41
		character = getchar();
		return RPARENS;

	} else if (character == 123) { // "{" => 123
		character = getchar();
		return LBRACE;

	} else if (character == 125) { // "}" => 125
		character = getchar();
		return RBRACE;

	} else if (character == 33) { // "!" => 61
		character = getchar();

		if (character == 61) { // "=" => 61
			character = getchar();
			return NOTEQ;
		} else
			return NOT;
			
	} else if (character == 61) { // "=" => 61
		character = getchar();

		if (character == 61) { // "=" => 61
			character = getchar();
			return EQUAL;
		} else
			return ASSIGN;

	} else if (character == 43) { // "+" => 43
		character = getchar();
		return PLUS;

	} else if (character == 45) { // "=" => 45
		character = getchar();
		return MINUS;

	} else if (character == 62) { // ">" => 62
		character = getchar();

		if (character == 61) { // "=" => 61
			character = getchar();
			return GTEQ;
		} else
			exit(-1); //return GT;

	} else if (character == 60) { // "<" => 60
		character = getchar();

		if (character == 61) { // "=" => 61
			character = getchar();
			return LTEQ;
		} else
			exit(-1); //return LT;


	} else if (character == 44) { // "," => 44
		character = getchar();
		return COMMA;

	//} else if ( ... ) {

	///...

	} else
		exit(-1); // unknown character
}

int getSymbol2() {
	int token;

	token = getSymbol2();

	printf("Token: %s (%d)\n", get_token_name(token), token);
	if(token == IDENTIFIER)	printf("\t'%s'\n", intstr_to_charstr(identifier));
	if(token == INTEGER) 		printf("\t%d\n", integer);

	return token;
}

// consume input until next character is found. ignores whitespace, comments
// and preprocessor directives. return value of -1 indicates
// error condition (such as end of file) - treat as clean end of input
// condition even if it is an actual error. return value >0 indicates ASCII
// character code also stored in variable "character".
int findNextCharacter() {
	int inComment; // are we scanning a comment?
	inComment = 0;

	while (1) {
		if (inComment) {
			character = getchar();

			if (character == 10) // ASCII code 10 = linefeed
				inComment = 0;
			else if (character == 13) // ASCII code 13 = carriage return
				inComment = 0;
			else if (character == -1) // end of file is represented by -1
				return character;
		} else if (isCharacterWhitespace())
			character = getchar();
		else if (character == 35) { // ACCII code 35 = #
			character = getchar();

			inComment = 1;
		} else if (character == 47) { // ASCII code 47 = /
			character = getchar();

			if (character == 47) { // ASCII code 47 = /
				character = getchar();

				inComment = 1;
			} else
				return character;
		} else
			return character;
	}
}

int isCharacterWhitespace() {
	if (character == 32) // ASCII code 32 = space
		return 1;
	else if (character == 9) // ASCII code 9 = tab
		return 1;
	else if (character == 10) { // ASCII code 10 = linefeed
		lineNR = lineNR + 1;
		return 1;
	}
	else if (character == 13) // ASCII code 13 = carriage return
		return 1;
	else
		return 0;
}

int isCharacterLetter() {
	if (character >= 97)	  // ASCII code 97 = a
		if (character <= 122) // ASCII code 122 = z
			return 1;
		else
			return 0;
	else if (character >= 65) // ASCII code 65 = A
		if (character <= 90)  // ASCII code 90 = Z
			return 1;
		else
			return 0;
	else
		return 0;
}

int identifierOrKeyword() {
	int* identifierCursor;

	identifierCursor = identifier;

//	return IDENTIFIER; // for debugging

	if (*identifierCursor == 119) { // ASCII code 119 = w
		// looking for "while"
		// "while" => 119 104 105 108 101

		identifierCursor = identifierCursor + 1;

		if (*identifierCursor == 104) // ASCII code 104 = h
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;

		if (*identifierCursor == 105) // ASCII code 105 = i
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;

		if (*identifierCursor == 108) // ASCII code 108 = l
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;

		if (*identifierCursor == 101) // ASCII code 109 = e
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;

		if (*identifierCursor == 0) // end of identifier?
			return WHILE;
		else
			return IDENTIFIER;

	} else if (*identifierCursor == 105) { // ASCII code 105 = i
		// looking for "if" or "int"
		identifierCursor = identifierCursor + 1;

		if (*identifierCursor == 102) { // ASCII code 102 = f
			// looking for "if"
			// "if" => 105 102
			
			identifierCursor = identifierCursor + 1;

			if (*identifierCursor == 0) // end of identifier?
				return IF;
			else
				return IDENTIFIER;
		} else if (*identifierCursor == 110) { // ASCII code 110 = n
			// looking for "int"
			// "int" => 105 110 116

			identifierCursor = identifierCursor + 1;

			if (*identifierCursor == 116)
				identifierCursor = identifierCursor + 1;
			else
				return IDENTIFIER;

			if (*identifierCursor == 0) // end of identifier?
				return INT;
			else
				return IDENTIFIER;
		} else
			return IDENTIFIER;
	} else if (*identifierCursor == 101) { // ASCII code 101 = e
		// looking for "else"
		// "else" => 101 108 115 101

		identifierCursor = identifierCursor + 1;


		if (*identifierCursor == 108) // ASCII code 108 = l
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;

		if (*identifierCursor == 115) // ASCII code 115 = s
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;

		if (*identifierCursor == 101) // ASCII code 101 = e
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;

		if (*identifierCursor == 0) // end of identifier?
			return ELSE;
		else
			return IDENTIFIER;

		//...
	} else if (*identifierCursor == 114) { // ASCII code 114 = r
		// looking for "return"
		// "return" => 114 101 116 117 114 110

		identifierCursor = identifierCursor + 1;

		if (*identifierCursor == 101) // ASCII code 101 = e
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;

		if (*identifierCursor == 116) // ASCII code 116 = t
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;

		if (*identifierCursor == 117) // ASCII code 117 = u
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;

		if (*identifierCursor == 114) // ASCII code 114 = r
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;

		if (*identifierCursor == 110) // ASCII code 110 = n
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;

		if (*identifierCursor == 0) // end of identifier?
			return RETURN;
		else
			return IDENTIFIER;

	} else if (*identifierCursor == 118) { // ASCII code 118 = v
		// looking for "void"
		// "void" => 118 111 105 100

		identifierCursor = identifierCursor + 1;

		if (*identifierCursor == 111) // ASCII code 111 = o
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;

		if (*identifierCursor == 105) // ASCII code 105 = i
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;

		if (*identifierCursor == 100) // ASCII code 100 = d
			identifierCursor = identifierCursor + 1;
		else
			return IDENTIFIER;


		if (*identifierCursor == 0) // end of identifier?
			return VOID;
		else
			return IDENTIFIER;

		//...

	} else
		// no keyword found
		return IDENTIFIER;

	return -1; // can't-happen to indicate unhandled case!
}
int isCharacterDigit() {
	if (character <= 47)	  // ASCII code 47 = /
		return 0;
	if (character >= 58)	  // ASCII code 58 = :
		return 0;
	return 1;
}
int isCharacterLetterOrDigit() {
	if(isCharacterLetter()) return 1;
	if(isCharacterLetter()) return 1;
	return 0;
}
int isCharacterLetterOrDigitOrUnderscore() {
	if(isCharacterLetter()) return 1;
	if(isCharacterLetter()) return 1;
	if(character == 95) return 1; // '_' = 95
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

static char *intstr_to_charstr(int *is) {
	int length = 0;
	char *cs = malloc(maxIdentifierLength);

	while((cs[length] = is[length]) != 0) length++;

	return cs;
}
