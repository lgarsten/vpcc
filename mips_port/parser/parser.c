#include "scanner.h"
#include "mipster.h"

void emitExit();
void cstar();
int* getSymbolTableEntry();
int identifierMatch(int* symbolTableIdentifier);
int relation_expression();
int expression();
void term();
void cast();
void factor();
int getGlobalVariableOffset();
int allocateGlobalVariable();
void whileStatement();
void ifStatement();
void returnStatement();
void fixup(int codeAddress);
void call();
int setProcedureAddress();
void procedure();
void variable();
void fixlink(int codeAddress);
void statement();
void assignment();

void syntaxError(int error_code);
int isSymbolPlusOrMinus();
int isSymbolAsteriskOrSlash();
void variableORprocedure();

//The following variable declarations, initialization code, and code generation facilities should be completed to a full C* compiler in an assignment. Instructions are encoded in 32-bit integers. Code is written to standard output in binary format.

int LINK; // link register
int SP;   // stack pointer
int FP;   // frame pointer
int GP;   // global pointer
int RR;   // return register
//...

int ZR; // zero register

// compile-time, stack-based register allocation
int allocatedRegisters;

// compile-time, bump pointer allocation of global variables
int allocatedGlobalVariables;

// opcodes
int ADD;
int SUB;
//...

int* symbolTable;

int* code;
int codeLength;
int maxCodeLength;

int returnBranches;

// current token
int symbol;

// lookahead of one
int lookahead;

// ERROR 
void syntaxError(int errorcode) {
	printError(errorcode);
}

// and DEBUG
void debug(int errorcode) {
	printDebug(errorcode);
}

void getCurrentSymbol() {
	symbol = lookahead;
	lookahead = getSymbol();
}

void initParser() {
	E_EXPRESSION = 1;
	E_TERM = 2;
	E_WHILE = 3;
	E_PROCEDURE = 4;
	E_CALL = 5;
	E_FACTOR = 6;
	E_CAST = 7;
	E_IF = 8;
	E_DECLARATION = 9;
	E_ASSIGNMENT = 10;
	E_VARIABLE = 11;	
	E_STATEMENT = 12;
	E_VOID = 13;
	E_TYPE = 14;
	E_RELATION_EXPRESSION = 15;
	E_RETURN = 16;
	E_ELSE = 17;
	E_VARIABLEORPOCEDURE = 18;
	
	lookahead = getSymbol();	
}

void emitExit() {
    int* identifierCursor;

    // "exit" is 4 characters plus null termination
    identifier = mipster_malloc(5 * 4);

    identifierCursor = identifier;

    *identifierCursor = 101; // ASCII code 101 = e
    identifierCursor = identifierCursor + 1;
    *identifierCursor = 120; // ASCII code 120 = x
    identifierCursor = identifierCursor + 1;
    *identifierCursor = 105; // ASCII code 105 = i
    identifierCursor = identifierCursor + 1;
    *identifierCursor = 116; // ASCII code 116 = t
    identifierCursor = identifierCursor + 1;
    *identifierCursor = 0; // end of identifier

    // current code length is address of next instruction
    createSymbolTableEntry(codeLength);

    // retrieve error code from stack into register 1
    //emitCode(LDW, 1, SP, 0);

    // halt machine, return error code from register 1
    //emitCode(HLT, 0, 0, 1);
}


int main() {
    // pick some maximum identifier length, e.g. 42 characters
    maxIdentifierLength = 42;

    LINK = 31;
    SP = 30;
    FP = 29;
    GP = 28;
    RR = 27;
    //...

    ZR = 0;

    allocatedRegisters = 0;
    allocatedGlobalVariables = 0;

    ADD = 7; // opcode for ADD
    SUB = 8; // opcode for SUB
    //...

    symbolTable = 0;

    // pick some maximum code length, e.g. 16K * 4 bytes = 64KB
    // increase as needed
    maxCodeLength = 16384;

    // allocate memory for emitting code
    code = mipster_malloc(maxCodeLength * 4);
    codeLength = 0;

    // start executing by branching to main procedure
    // please do not forget: parameter c requires fixup
    // when code location for main procedure is known
    //emitCode(BSR, 0, 0, 0);

    // push exit code in return register onto stack
    //emitCode(PSH, RR, SP, 4);

    // halt machine by invoking exit
    
    //emitCode(BSR, 0, 0, codeLength + 1);

    // emit library code for exit right here
    
    // emitExit();

    // similarly, emit library code for malloc, getchar, and putchar
    //...
    
     // init scanner
    init_scanner();
	// init parser
    initParser();

    // get first symbol from scanner
    getCurrentSymbol();

    // invoke compiler
    cstar();

    // write code to standard output
    //writeBinary();

    return 0;
}
//-----------------------------

//This is a dynamically allocated, list-based implementation of a symbol table in C*, extend as needed.

int createSymbolTableEntry(int data) {
    int* symbolTableCursor;

    // 1 list pointer, 1 identifier pointer, 1 data integer
    symbolTableCursor = mipster_malloc(3 * 4);

    // create entry at head of symbol table
    // cast only works if size of int and int* is equivalent
    *symbolTableCursor = (int) symbolTable;
    symbolTable = symbolTableCursor;

    // store pointer to identifier
    // cast only works if size of int and int* is equivalent
    symbolTableCursor = symbolTable + 1;
    *symbolTableCursor = (int) identifier;

    // store data integer
    symbolTableCursor = symbolTable + 2;
    *symbolTableCursor = data;

    return data;
}

int* getSymbolTableEntry() {
    int* symbolTableCursor;

    symbolTableCursor = symbolTable;

    while (symbolTableCursor != 0) {
        if (identifierMatch(symbolTableCursor + 1)) {
            // return entry
            return symbolTableCursor;
        }
        else {
            // keep looking
            // cast only works if size of int and int* is equivalent
            symbolTableCursor = (int*) *symbolTableCursor;
        }
    }

    // no entry found
    return 0;
}

int identifierMatch(int* symbolTableIdentifier) {
    int* identifierCursor;
    int* symbolTableIdentifierCursor;

    identifierCursor = identifier;
    symbolTableIdentifierCursor = symbolTableIdentifier;

    while (1) {
        if (*identifierCursor == *symbolTableIdentifierCursor) {
            if (*identifierCursor == 0) // end of identifiers: match!
                return 1;
            else {
                identifierCursor = identifierCursor + 1;
                symbolTableIdentifierCursor = symbolTableIdentifierCursor + 1;
            }
        } else
            return 0;
    }
}
//-----------------------------

//Here is the above recursive-descent parser decorated into a single-pass compiler that generates DLX code for arithmetic expressions in C*. Assignments are to be done as part of an assignment.

void cstar() {
	variableORprocedure();
}

void variableORprocedure() {
	debug(E_VARIABLEORPOCEDURE);
	while (symbol != -1) {
		if (symbol == VOID) {			
			getCurrentSymbol();
			if (symbol == ASTERISK) {
				getCurrentSymbol();
			}
			debug(E_VOID);
			procedure();
		}
		else if (type()) {
			if (lookahead == SEMICOLON) {
				variable();		
			}
			else if (lookahead == LPARENS) {
				procedure();
			}
		}
		else {
			syntaxError(E_VARIABLEORPOCEDURE);
			getCurrentSymbol();
		}
	}
}

// local var, function parameter
void declaration() {
	type();
	debug(E_DECLARATION);
	if (symbol == IDENTIFIER) {
		getCurrentSymbol();
	}
	else {
		syntaxError(E_DECLARATION);
	}
}

// global var
void variable() {
	debug(E_VARIABLE);
	if (symbol == IDENTIFIER) {
		getCurrentSymbol();
		if (symbol == SEMICOLON) {
			getCurrentSymbol();
		}
		else {
			syntaxError(E_VARIABLE);
		}
	}
}

int type() {
	if (symbol == INT) {
		debug(E_TYPE);
		getCurrentSymbol();
		
		if (symbol == ASTERISK) {
			getCurrentSymbol();
		}
		return 1;
	}
	else {
		return 0;
	}
}

void statement() {
	debug(E_STATEMENT);

	if (symbol == WHILE) {
		whileStatement();
	}
	else if (symbol == IF) {
		ifStatement();
	}
	else if (symbol == RETURN) {
		returnStatement();
		getCurrentSymbol();
	}
	else if (symbol == ASTERISK) {
		getCurrentSymbol();
		if (symbol == IDENTIFIER) {
			if (lookahead == ASSIGN) {
				assignment();
				getCurrentSymbol();
			}
			else {
				call();
				getCurrentSymbol();
			}
		}
	}
	else if (symbol == IDENTIFIER) {
		if (lookahead == ASSIGN) {
			assignment();
			getCurrentSymbol();
		}
		else {
			call();
			getCurrentSymbol();
		}
	}
	// Local variables dont have to be declared on top of the function (TODO mb remove because not in EBNF)
	else if (symbol == INT) {
		declaration();
		getCurrentSymbol();
	}
	else {
		syntaxError(E_STATEMENT);
	}
}

void assignment() {
	debug(E_ASSIGNMENT);
	if (symbol == ASTERISK) {
		getCurrentSymbol();
	}
	if (symbol == IDENTIFIER) {
		getCurrentSymbol();
		if (symbol == ASSIGN) {
			getCurrentSymbol();
			expression();
		}
	}
	else {
		syntaxError(E_ASSIGNMENT);
	}
}

int relation_expression() {
	debug(E_RELATION_EXPRESSION);
	
	expression();
	// TODO add LT GT
	if (symbol == EQUAL) {
		getCurrentSymbol();
		expression();
	}
	else if (symbol == NOTEQ) {
		getCurrentSymbol();
		expression();
	}
	else if (symbol == GTEQ) {
		getCurrentSymbol();
		expression();
	}
	else if (symbol == LTEQ) {
		getCurrentSymbol();
		expression();
	}
	
	return 0;
}

int expression() {
   debug(E_EXPRESSION);
    // assert: n = allocatedRegisters

    // have we parsed a minus sign?
    // must be local variable to work for nested expressions
    int sign;

    // remember operator symbol
    // must be local variable to work for nested expressions
    int operatorSymbol;

    if (symbol == MINUS) {
        sign = 1;
        getCurrentSymbol();
    } else {
        sign = 0;
    }

    term();

    // assert: allocatedRegisters == n + 1

    if (sign) {
        //emitCode(SUB, allocatedRegisters, ZR, allocatedRegisters);
    }

    while (isSymbolPlusOrMinus()) {
        operatorSymbol = symbol;
        getCurrentSymbol();

        term();

        if (operatorSymbol == PLUS) {
            //emitCode(ADD, allocatedRegisters - 1, allocatedRegisters - 1, allocatedRegisters);
        }
        else {
            ////emitCode(SUB, allocatedRegisters - 1, allocatedRegisters - 1, allocatedRegisters);
		}

        allocatedRegisters = allocatedRegisters - 1;
    }

    // assert: allocatedRegisters == n + 1

    // enhance to return BEQ, BNE, etc.
    // depending on parsed comparison operator
    return 0;
}

void term() {
    debug(E_TERM);	
    // assert: n = allocatedRegisters

    // remember operator symbol
    // must be local variable to work for nested expressions
    int operatorSymbol;

    factor();

    // assert: allocatedRegisters == n + 1

    while (isSymbolAsteriskOrSlash()) {
        operatorSymbol = symbol;
        getCurrentSymbol();

        term();

       // if (operatorSymbol == ASTERISK)
         //   emitCode(MUL, allocatedRegisters - 1, allocatedRegisters - 1, allocatedRegisters);
       // else
         //   emitCode(DIV, allocatedRegisters - 1, allocatedRegisters - 1, allocatedRegisters);

        allocatedRegisters = allocatedRegisters - 1;
    }

    // assert: allocatedRegisters == n + 1
}

void cast() {
    if (symbol == LPARENS) {
    
       if (lookahead == INT) {
		 debug(E_CAST);
		 getCurrentSymbol();
		   
		 type();
		   
		 if (symbol == RPARENS) {
		     getCurrentSymbol();
		 }
		 else {
		     syntaxError(E_CAST); // right parenthesis expected!
		 }
	   }   
    }
}

void factor() {
    debug(E_FACTOR);
    // assert: n = allocatedRegisters

    // have we parsed an asterisk sign?
    // must be local variable to work for nested expressions
    int dereference;

    cast();

    if (symbol == ASTERISK) {
        dereference = 1;

        getCurrentSymbol();
    } 
    else {
        dereference = 0;
    }

    if (symbol == IDENTIFIER) {
    	   // call
    	   if (lookahead == LPARENS) {
    	        call();
    	   }
    	   // variable
    	   else {
		   allocatedRegisters = allocatedRegisters + 1;

		   //emitCode(LDW, allocatedRegisters, GP, getGlobalVariableOffset());
		   getCurrentSymbol();
        }
    } 
    else if (symbol == INTEGER) {
        allocatedRegisters = allocatedRegisters + 1;

        //emitCode(ADDI, allocatedRegisters, ZR, integer);

        getCurrentSymbol();
    } 
    else if (symbol == LPARENS) {
        getCurrentSymbol();
        expression();

        if (symbol == RPARENS) {
            getCurrentSymbol();
        }
        else {
            syntaxError(E_FACTOR); // right parenthesis expected!
        }
    } 
    else {
        syntaxError(E_FACTOR); // identifier, integer, or left parenthesis expected!
    }

    //if (dereference)
        //emitCode(LDW, allocatedRegisters, allocatedRegisters, 0);

    // assert: allocatedRegisters == n + 1
}
//-----------------------------

//Store global variable offsets in symbol table.

int getGlobalVariableOffset() {
    int* symbolTableCursor;

    symbolTableCursor = getSymbolTableEntry();

    if (symbolTableCursor != 0) {
        symbolTableCursor = symbolTableCursor + 2;

        return *symbolTableCursor;
    } else {
        // variable not found but who cares,
        // just create entry in symbol table,
        // allocate space for global variable,
        // and return its offset
        return createSymbolTableEntry(allocateGlobalVariable());
    }
}

int allocateGlobalVariable() {
    allocatedGlobalVariables = allocatedGlobalVariables + 1;

    // each variable needs 4 bytes, global variable offsets are negative
    return -4 * allocatedGlobalVariables;
}
//-----------------------------

//While statements are compiled as follows. Conditional statements are analogous and should be done as part of an assignment.

void whileStatement() {
	debug(E_WHILE);
    // assert: allocatedRegisters == 0

    // both must be local variables to work for nested while statements
    int branchBackwardsToWhile;
    int branchForwardToEndOfWhile;

    // may be global variable but is anyway nicer like that
    int branchInstruction;

    // remember address of next instruction to loop back to below
    branchBackwardsToWhile = codeLength;

    if (symbol == WHILE) {
        getCurrentSymbol();
    }
    else {
        syntaxError(E_WHILE); // while expected!
    }

    if (symbol == LPARENS) {
        // enhance expression() to return BEQ, BNE, etc.
        // depending on parsed comparison operator
        // hint 1: instruction must be the negation!
        // hint 2: if there is no comparison use BEQ
        getCurrentSymbol();
        
        //branchInstruction = expression(); // EDIT
	   branchInstruction = relation_expression();	
		
        // remember address of next instruction which
        // conditionally branches forward to end of while
        // if parsed comparison evaluates to false!
        branchForwardToEndOfWhile = codeLength;

        // assert: allocatedRegisters == 1

        // target address unknown, so just say 0 here, fixup is done below
        //emit(branchInstruction, allocatedRegisters, 0, 0);

        // do not need the register for comparison anymore
        allocatedRegisters = allocatedRegisters - 1;

        if (symbol == RPARENS) {
            getCurrentSymbol();
        }
        else {
            syntaxError(E_WHILE); // right parenthesis expected!
        }
    } else {
        syntaxError(E_WHILE); // left parenthesis expected!
    }

    // assert: allocatedRegisters == 0

    if (symbol == LBRACE) {
        getCurrentSymbol();

        while (symbol != RBRACE) {
            statement();
        }

        getCurrentSymbol();
    } else {
        statement();
    }

    // assert: allocatedRegisters == 0

    // unconditional branch backwards to while
    //emit(BR, 0, 0, branchBackwardsToWhile - codeLength);

    // here will be the first instruction after the loop
    // so point the conditional branch instruction from above here
    fixup(branchForwardToEndOfWhile);
}

void ifStatement() {
	debug(E_IF);

    if (symbol == IF) {
        getCurrentSymbol();
    }
    else {
        syntaxError(IF); // if expected!
    }

    if (symbol == LPARENS) {
        getCurrentSymbol();
        
	   relation_expression();	

        if (symbol == RPARENS) {
            getCurrentSymbol();
        }
        else {
            syntaxError(E_IF); // right parenthesis expected!
        }
    } else {
        syntaxError(E_IF); // left parenthesis expected!
    }

    if (symbol == LBRACE) {
        getCurrentSymbol();

        while (symbol != RBRACE) {
            statement();
        }

        getCurrentSymbol();
    } else {
        statement();
    }

    if (symbol == ELSE) {
    	   debug(E_ELSE);
        getCurrentSymbol();
        
        if (symbol == LBRACE) {
            getCurrentSymbol();
            
            while (symbol != RBRACE) {
            	statement();
            } 
            getCurrentSymbol();
        }
        else {
           statement();
        }
    }
    
}

void fixup(int codeAddress) {
    // assert: -2^15 <= codeLength - codeAddress <= 2^15 - 1

    // branch from instruction at codeAddress to instruction at codeLength
   // setParameterCInCode(codeAddress, codeLength - codeAddress);
}
//-----------------------------

//Procedure calls are (in principle) done as follows. Note that (in reality) the procedure identifier needs to be parsed before invoking call and then passed into call (by factor and statement) because the grammar (for factor and statement) is not fully left-factored. In other words, the parser can only know if it is dealing with a variable access or a procedure call after it has seen the symbol that appears after the identifier, i.e., if the parser then sees a left parenthesis (call) or not (identifier). This is called a lookahead of two (which can nevertheless be reduced to one by left factoring).

void call() {
	debug(E_CALL);
    // assert: n = allocatedRegisters

    // both must be local variables to work for nested expressions
    int* procedureIdentifier;
    int savedAllocatedRegisters;

    if (symbol == IDENTIFIER) {
        // save procedure identifier for symbol table lookup below
        procedureIdentifier = identifier;

        getCurrentSymbol();

        savedAllocatedRegisters = allocatedRegisters;

        // save allocated registers on stack
      //  while (allocatedRegisters > 0) {
      //      emit(PSH, allocatedRegisters, SP, 4);

      //      allocatedRegisters = allocatedRegisters - 1;
      //  }

        // assert: allocatedRegisters == 0

        if (symbol == LPARENS) {
            getCurrentSymbol();

            if (symbol != RPARENS) {
                expression();

                // push value of expression (actual parameter) onto stack
                //emit(PSH, allocatedRegisters, SP, 4);

                // register for value of expression is not needed anymore
                allocatedRegisters = allocatedRegisters - 1;

                // assert: allocatedRegisters == 0

                while (symbol == COMMA) {
                    getCurrentSymbol();
                    expression();

                    // push value of expression (actual parameter) onto stack
                    //emit(PSH, allocatedRegisters, SP, 4);

                    // register for value of expression is not needed anymore
                    allocatedRegisters = allocatedRegisters - 1;

                    // assert: allocatedRegisters == 0
                }

                if (symbol == RPARENS) {
                    getCurrentSymbol();
                }
                else {
                    syntaxError(E_CALL); // right parenthesis expected!
                }
            } else {
                getCurrentSymbol();
            }
        } else {
            syntaxError(E_CALL); // left parenthesis expected!
        }

        // restore procedure identifier for symbol table lookup
        identifier = procedureIdentifier;

        //procedureAddress = setProcedureAddress();

       // if (procedureAddress == codeLength)
            // create a new fixup chain
            //emit(BSR, 0, 0, 0);
       // else if (getOpcodeFromCode(procedureAddress) == BSR)
            // link to the head of an existing fixup chain
            //emit(BSR, 0, 0, procedureAddress);
       // else
            // branch to subroutine to invoke procedure
            //emit(BSR, 0, 0, procedureAddress - codeLength);
		
        // assert: allocatedRegisters == 0

        // restore allocated registers from stack
      //  while (allocatedRegisters < savedAllocatedRegisters) {
      //      allocatedRegisters = allocatedRegisters + 1;

      //      emit(POP, allocatedRegisters, SP, 4);
      //  }
    } else {
        syntaxError(E_CALL); // identifier expected!
    }
    // assert: allocatedRegisters == n
}
//-----------------------------

//Store procedure addresses in symbol table and create fixup chains for forward declarations.

//int setProcedureAddress() {
//    int* symbolTableCursor;
//    int savedAddress;

//    symbolTableCursor = getSymbolTableEntry();

//    if (symbolTableCursor != 0) {
//        symbolTableCursor = symbolTableCursor + 2;

//        savedAddress = *symbolTableCursor;

//        if (getOpcodeFromCode(savedAddress) == BSR)
            // save address of next instruction which may point to
            // the new head of an existing fixup chain
            // or the beginning of a procedure body
//            *symbolTableCursor = codeLength;

//        return savedAddress;
//    } else
        // procedure not found but who cares,
        // just create entry in symbol table,
        // and save address of next instruction
//        return createSymbolTableEntry(codeLength);
//}

//-----------------------------

//Procedure definitions are next. Here the construction of the local symbol table containing the parameters and local variables of a procedure is still to be done as part of an assignment. Also, the access of parameters and local variables still needs to be implemented (in factor and assignment).

void procedure() {
	debug(E_PROCEDURE);
    // assert: allocatedRegisters == 0

    // may be global variables but are anyway nicer like that
    int callBranches;
    int parameters;
    int localVariables;

  //  if (symbol == INTEGER) {
  //      getCurrentSymbol();
  //
  //      if (symbol == ASTERISK) {
  //          getCurrentSymbol();
  //      }
  //  } 
  //  else if (symbol == VOID) {
  //      getCurrentSymbol();
  //  }
  //  else {
  //      syntaxError(PROCEDURE); // int expected!
  //  }

	if (symbol == IDENTIFIER) {
       // callBranches = setProcedureAddress();
		getCurrentSymbol();
		

        //if (callBranches != codeLength) {
        //    if (getOpcodeFromCode(callBranches) == BSR)
        //        fixlink(callBranches);
        //    }
        //    else {
                // procedure defined more than once!
        //        declarationError(PROCEDURE);
        //    }

		if (symbol == LPARENS) {
			
			getCurrentSymbol();

			parameters = 0;

            if (symbol != RPARENS) {
                declaration();

                parameters = 1;

                while (symbol == COMMA) {
                    getCurrentSymbol();

                    declaration();

                    parameters = parameters + 1;
                }

                // parameter offsets are only known here
                // when all parameters have been parsed!
                // thus enter offsets in local table now:

                //...

                if (symbol == RPARENS) {
                    getCurrentSymbol();
                }
                else {
                    syntaxError(E_PROCEDURE); // right parenthesis expected!
                }
            } else {  	 
                getCurrentSymbol();
            }
        } else {
            syntaxError(E_PROCEDURE); // left parenthesis expected!
        }

        if (symbol == SEMICOLON) {
            getCurrentSymbol();
        }
        else if (symbol == LBRACE) {
        	
            getCurrentSymbol();

            localVariables = 0;

            while (symbol == INT) {

                declaration();

                localVariables = localVariables + 1;

                if (symbol == SEMICOLON) {
                    getCurrentSymbol();
                }
                else {
                    syntaxError(E_PROCEDURE); // semicolon expected!
                }
            }

            // procedure prologue

            // save return address
            //emit(PSH, LINK, SP, 4);

            // save caller's frame
            //emit(PSH, FP, SP, 4);

            // allocate callee's frame
            //emit(ADD, FP, ZR, SP);

            // allocate callee's local variables
            //emit(SUBI, SP, SP, localVariables * 4);

            // create a fixup chain for return statements
            returnBranches = 0;

            while (symbol != RBRACE) {
            	statement();
            }
		               
            getCurrentSymbol();

            // procedure epilogue

            //fixlink(returnBranches);

            // deallocate callee's frame and local variables
            //emit(ADD, SP, ZR, FP);

            // restore caller's frame
            //emit(POP, FP, SP, 4);

            // restore return address and deallocate parameters
            //emit(POP, LINK, SP, parameters * 4 + 4);

            // return
            //emit(RET, 0, 0, LINK);
           
        } else {
            syntaxError(E_PROCEDURE); // semicolon or left braces expected!
        }
    } else {
        syntaxError(E_PROCEDURE); // identifier expected!
    }

    // assert: allocatedRegisters == 0
}

int isSymbolPlusOrMinus() {
	if (symbol == PLUS) {
		return 1;
	}
	if (symbol == MINUS) {
		return 1;
	}
	return 0;
}

int isSymbolAsteriskOrSlash() {
	if (symbol == ASTERISK) {
		return 1;
	}
	// not scanned yet!
	//if (symbol == SLASH) {
	//	return 1;
	//}
	return 0;
}


//-----------------------------

//Fixup a whole chain of branch instructions.

void fixlink(int codeAddress) {
    int previousCodeAddress;

    while (codeAddress != 0) {
        //previousCodeAddress = getParameterCFromCode(codeAddress);

        fixup(codeAddress);

        codeAddress = previousCodeAddress;
    }
}
//-----------------------------

//Return statements only need to make sure that the return value of procedures are stored in RR before returning.

void returnStatement() {
    debug(E_RETURN);
    // assert: allocatedRegisters == 0

    if (symbol == RETURN) {
        getCurrentSymbol();
    }
    else {
        syntaxError(RETURN); // return expected;
    }

    if (symbol != SEMICOLON) {
        expression();

        // save value of expression in return register
        //emit(ADD, RR, ZR, allocatedRegisters);

        // register for value of expression is not needed anymore
        allocatedRegisters = allocatedRegisters - 1;
    }

    // unconditional branch to procedure epilogue
    // maintain fixup chain for later fixup
    //emit(BR, 0, 0, returnBranches);

    // new head of fixup chain
    returnBranches = codeLength - 1;

    // assert: allocatedRegisters == 0
}
//-----------------------------

//Here is the complete C* syntax in EBNF form.

//type = "int" [ "*" ] .

//relation_expression = expression [ ( "<=" | "==" | ">=" | "!=" ) expression ]                        
//expression = simpleExpression [ ( "==" | "!=" | "<" | ">" | "<=" | ">=" ) simpleExpression ] .
//simpleExpression = [ "-" ] term { ( "+" | "-" ) term } .
//term = factor { ( "*" | "/" | "%" ) factor } .
//cast = "(" type ")" .
//factor = cast [ "*" ]  ( identifier | integer | "(" expression ")" | call ) .

//assignment = [ "*" ] identifier "=" expression .
//while = "while" "(" relation_expression ")" ( statement | "{" { statement } "}" ) .
//if = "if" "(" relation_expression ")" ( statement | "{" { statement } "}" ) [ "else" ( statement | "{" { statement } "}" ) ] .
//call = identifier "(" [ expression { "," expression } ] ")" .
//return = "return" [ expression ] .
//statement = assignment ";" | while | if | call ";" | return ";" .

//declaration = type identifier .
//variable = identifier ";" .
//procedure = identifier "(" [ variable { "," variable } ] ")" ( ";" | "{" { variable ";" } { statement } "}" ) .
//variableORprocedure = ( "void" | type ) LOOKAHEAD -> ( ";" variable | "(" procedure) .

//cstar = { variableORprocedure } .
