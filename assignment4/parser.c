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
int allocateLocalVariable();
void whileStatement();
void ifStatement();
void returnStatement();
void fixup(int codeAddress);
void setParameterCInCode(int codeAdress, int value);
void call();
int setProcedureAddress();
int getOpcodeFromCode(int instruction);
void prologue(loadSize);
void epilogue(paramSize);
void procedure(int have_asterisk);
void variable(int have_asterisk);
void fixlink(int codeAddress);
void statement();
void assignment(int have_asterisk);
void declaration();

void printCodeArray();
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
int ZR; // zero register
//...


// compile-time, stack-based register allocation
int allocatedRegisters;

// bump pointer for heap allocation
int bump_pointer;

// compile-time, bump pointer allocation of global and local variables
int allocatedGlobalVariables;
int allocatedLocalVariables;

// opcodes
int ADD;
int SUB;
int MUL;
int DIV;
int ADDI;
int SUBI;

int LDW;
int SW;
int LUI;
int ORI;

int HLT;

int SLT;	// rd ← (rs < rt) 1 true, 0 false
int BEQ;	// branch on equal
int BGEZ; // branch on greater or equal to zero
int BGTZ;	 // greater zero
int BLEZ;  // less or equal zero
int BLTZ;  // less zero
int BNE;	  // not equal
int BSR;
int BR;	// same as BEQ, but without parameter
int RET;

int J;
int JAL;
int JR;

int PSH;
int POP;
int MFLO; // move LO value into reg
int MFHI; // same with HI

int NOP;	 // addi zr zr 0
int BREAK;
//...

int operating_on_pointer;

// start pointer of global symbol table's linked list.
int* symbolTable;

int* code;
int codeLength;
int maxCodeLength;

int returnBranches;

// current token
int symbol;

// lookahead of one
int lookahead;

// detect pointer
int is_pointer;

void printCodeArray() {
	int *codecursor = code;
	int i = 0;
	
	while (i < codeLength) {
		printf("%d CODEARRAY: %d\n",i, *codecursor);
		codecursor = codecursor +1;
		i = i +1;
	}
}

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
	
	returnBranches = 0;
	
	lookahead = getSymbol();	
}

void emitCode(int op, int a, int b, int c) {

	int *codecursor;
	int instruction = 0;
	
	codecursor = code;
	codecursor = codecursor + codeLength;

	instruction = encodeInstruction(op, a, b, c);
		
	*codecursor = instruction;
	codeLength = codeLength + 1;
}

// calculates 2^x
int m_pow(int times) {

	int i = 0;
	int pow = 2;
	
	while (i < times -1) {
		pow = pow * 2;
		i = i + 1;
	}
	return pow;
}

// convert negative integer to two's complement
int twos(int x, int bit) {

	int max_16bit = 65535;
	int max_26bit = 67108863; 
	int twos_corr = 1;
	
	if (x < 0) {
		if (bit == 16)
			x = max_16bit + x + twos_corr;
		else if (bit == 26)
			x = max_26bit + x + twos_corr;
	}
	return x;
}

int is32bit(int x) {
	if (x > 32767) {
		return 1;
	}
	else if (x < -65535) {
		return 1;
	}
	else {
		return 0;
	}
}

void ADDI32bitImmediate(int op, int a, int b, int c) {
	// immediate values bigger than 16bit is done by: "lui $s0, upper(big)" and "ori $s0, $s0, lower(big)"
	int max = 32767;
	int min = -65535;
	
	if (c > max) {
		emitCode(LUI,a,b,c);
		emitCode(ORI,a,a,c);	
	}
	else if (c < min) {
		emitCode(LUI,a,b,c);
		emitCode(ORI,a,a,c);	
	}
	else {
		emitCode(ADDI, a, b, c);
	}
}

int encodeInstruction(int op, int a, int b, int c) {
	
	// 0x80000000, to fix integer overflow
	int sign_bit = -2147483648;
	
	int unsigned_instuction = 0;
	int encoded_instruction = 0;

	if (op == ADD) {
		encoded_instruction = (b * m_pow(21)) + (c * m_pow(16)) + (a * m_pow(11)) + (0 * m_pow(6)) + op; 
	}
	else if (op == SUB) {
	    	if (integer != -2147483648) {	// FIX integer overflow TODO test..
			encoded_instruction = (b * m_pow(21)) + (c * m_pow(16)) + (a * m_pow(11)) + (0 * m_pow(6)) + op;
		}
	}
	else if (op == MUL) {
		encoded_instruction = (op * m_pow(26)) + (b * m_pow(21)) + (c * m_pow(16)) + (a * m_pow(11)) + (0 * m_pow(6)) + 2;
	}
	else if (op == DIV) {	// stores result in LO and HI special registers
		encoded_instruction = (a * m_pow(21)) + (b * m_pow(16)) + (0 * m_pow(6)) + op;
	}
	else if (op == ADDI) { 
		c = twos(c, 16);
		encoded_instruction = (op * m_pow(26)) + (b * m_pow(21)) + (a * m_pow(16)) + c;
	}
	// SW var[reg], base[reg], offset
	else if (op == SW) {
	
		// fix parameter c 
		c = twos(c, 16);
		
		// remove sign bit 
		unsigned_instuction = (op - m_pow(5)) * m_pow(26) + b * m_pow(21) + a * m_pow(16) + c;
		
		// add sign bit to unsigned instruction, fix overflow
		encoded_instruction = sign_bit + unsigned_instuction;
	}
	else if (op == LDW) {
		c = twos(c, 16);		
		unsigned_instuction = (op - m_pow(5)) * m_pow(26) + b * m_pow(21) + a * m_pow(16) + c;
		encoded_instruction = sign_bit + unsigned_instuction;
	}
	else if (op == SLT) {
		c = twos(c, 16);		
		encoded_instruction = 0 * m_pow(26) + b * m_pow(21) + c * m_pow(16) + a * m_pow(11) + 0 * m_pow(6) + op;
	}
	else if (op == BR) {
		op = 4;
		c = twos(c, 16);		
		encoded_instruction = op * m_pow(26) + 0 * m_pow(21) + 0 * m_pow(16) + c;
	}
	else if (op == BEQ) {
		c = twos(c, 16);
		encoded_instruction = (op * m_pow(26)) + (a * m_pow(21)) + (b * m_pow(16)) + c;
	}
	else if (op == BGEZ) {
		c = twos(c, 16);
		encoded_instruction = (1 * m_pow(26)) + (a * m_pow(21)) + (op * m_pow(16)) + c;
	}
	else if (op == BGTZ) {
		c = twos(c, 16);
		encoded_instruction = (op * m_pow(26)) + (a * m_pow(21)) + (0 * m_pow(16)) + c;
	}
	else if (op == BLTZ) {	
		c = twos(c, 16);
		encoded_instruction = (1 * m_pow(26)) + (a * m_pow(21)) + (op * m_pow(16)) + c;
	}
	else if (op == BLEZ) {
		c = twos(c, 16);
		encoded_instruction = (op * m_pow(26)) + (a * m_pow(21)) + (0 * m_pow(16)) + c;
	}
	else if (op == BNE) {
		c = twos(c, 16);
		encoded_instruction = (op * m_pow(26)) + (a * m_pow(21)) + (b * m_pow(16)) + c;
	}
	else if (op == MFLO) {
		c = twos(c, 16);
		encoded_instruction = (a * m_pow(11)) + op;
	}
	else if (op == MFHI) {
		c = twos(c, 16);
		encoded_instruction = (a * m_pow(11)) + op;
	}
	else if (op == NOP) {		// NOP => ADDI reg[0] reg[0] reg[0]
		encoded_instruction = (ADDI * m_pow(26)) + ZR + ZR + ZR;
	}
	else if (op == LUI) {		// LUI reg[target],unused,immediate 80×(1÷(2^4))
		int shift = m_pow(16);   // 16 bit shift value
		if (c == -2147483648) {		// if 0x80000000
			encoded_instruction = (op * m_pow(26)) + (a * m_pow(16)) + 32768;
		}
		else {
			c = c /shift;			//  shift c in high 32 bit
			encoded_instruction = (op * m_pow(26)) + (a * m_pow(16)) + c;
		}
	}						
	else if (op == ORI) {		// ORI reg[target],reg[high_32_bit],immediate low_16_bit
		int shift = m_pow(16);
		int high_bits = (c / shift) * m_pow(16);	// high 32 bit in last 4 bits
		op = 13;								// correct opcode, else BREAK would be called
		c = c - high_bits; 						// correct to low 16 bits	
		encoded_instruction = (op * m_pow(26)) + (b * m_pow(21)) + (a * m_pow(16)) + c;
	}	
	else if (op == J) {
		c = twos(c, 26);
		encoded_instruction = op * m_pow(26) + c;
	}	
	else if (op == JAL) {
		c = twos(c, 26);
		encoded_instruction = op * m_pow(26) + c;
	}
	else if (op == JR) {
		op = 8;
		encoded_instruction = 0 * m_pow(26) + a * m_pow(21) + op;
	}	
	else if (op == BREAK) {
		encoded_instruction = op;
	}
	else {
		printf("[ERROR] unknown opcode\n");
	}

	return encoded_instruction;
}

// set GP to code length + global variables + 1 (+1 = GP set instruction)
void setGlobalPointer() {
	int save_code_length = codeLength;
	codeLength = 0;
	
	emitCode(ADDI, GP, GP, (allocatedGlobalVariables + save_code_length +1) *4);
	bump_pointer = ((allocatedGlobalVariables + save_code_length +1) * 4) +4;
	
	// bump pointer starts at GP + 4 and is increased by 4 for every allocated memory
	emitCode(ADDI, 1, GP, 4);
	emitCode(SW, 1, GP, 4);
	
	// set RR to zero
	emitCode(ADDI, RR, ZR, ZR);
	
	codeLength = save_code_length;
}

void setStartAddessToMain() {
	int main_address;
	int *symbolTableCursor;
	int save_code_length = codeLength;

     int* identifierCursor;
   
     identifierCursor = identifier; 	
     
     *identifierCursor = 109; // ASCII code 109 = m
     identifierCursor = identifierCursor + 1;
     *identifierCursor = 97; // ASCII code 120 = a
     identifierCursor = identifierCursor + 1;	
     *identifierCursor = 105; // ASCII code 105 = i
     identifierCursor = identifierCursor + 1;	
     *identifierCursor = 110; // ASCII code 110 = n
     identifierCursor = identifierCursor + 1;	
	
	symbolTableCursor = getSymbolTableEntry();
	symbolTableCursor = symbolTableCursor + 2;
	
	main_address = *symbolTableCursor;
	
	codeLength = 26;
	
	emitCode(ADDI, LINK, ZR, save_code_length * 4);
	emitCode(J, 0, 0, main_address);
		
	codeLength = save_code_length;
}

void emitMalloc() {

     int* identifierCursor;
   
     identifierCursor = identifier; 

     *identifierCursor = 109; // ASCII code 109 = m
     identifierCursor = identifierCursor + 1;
     *identifierCursor = 97; // ASCII code 120 = a
     identifierCursor = identifierCursor + 1;
     *identifierCursor = 108; // ASCII code 105 = l
     identifierCursor = identifierCursor + 1;
     *identifierCursor = 108; // ASCII code 116 = l
     identifierCursor = identifierCursor + 1;
     *identifierCursor = 111; // ASCII code 116 = o
     identifierCursor = identifierCursor + 1;
     *identifierCursor = 99; // ASCII code 116 = c
     identifierCursor = identifierCursor + 1;
     *identifierCursor = 0; // end of identifier

     createSymbolTableEntry(codeLength);

	prologue(0);	// 0 local vars

	// load requested mem_size from stack (arg) (1 arg, 8 bytes are LINK and FP)
	emitCode(LDW, 2, FP, 8);

	// load bump_pointer
	emitCode(LDW, 1, GP, 4);
	
	// save pointer to RR
	emitCode(ADDI, RR, 1, 4);
	
	// increase bump_pointer (load current)
	emitCode(LDW, 1, GP, 4);
	
	// add requested mem_size bump_pointer
	emitCode(ADD, 1, 1, 2);
	
	// save increased bump_pointer
	emitCode(SW, 1, GP, 4);
	
	epilogue(1 * 4);	// 1 arg: mem_size
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
    FP = 30;
    SP = 29;
    GP = 28;
    RR = 27;
    //...

    ZR = 0;

    allocatedRegisters = 0;
    allocatedGlobalVariables = 0;
    allocatedLocalVariables = -1;

    ADD = 32; // opcode for ADD
    SUB = 34; // opcode for SUB
    MUL = 28;
    DIV = 26;
    SW = 43;
    ADDI = 8;
    LDW = 35;
    LUI = 15;
    ORI = 97;
    SLT = 42;
    MFLO = 18;
    MFHI = 16;
    BGEZ = 1;
    BGTZ = 7;
    BLEZ = 6;
    BLTZ = 0;
    BNE = 5;
    BEQ = 4;
    J = 2;
    JAL = 3;
    JR = 96;   // opcode is 8 but ADDI is using it
    BSR = JAL;
    BR = 99;	// own opcode for mapping
    NOP = 98;	// same
    BREAK = 13;
    //...

    symbolTable = 0;

    // pick some maximum code length, e.g. 16K * 4 bytes = 64KB
    // increase as needed
    maxCodeLength = 16384;

    // allocate memory for emitting code
    code = mipster_malloc(maxCodeLength * 4);
    codeLength = 0;
    is_pointer = 0;

    // start executing by branching to main procedure
    // please do not forget: parameter c requires fixup
    // when code location for main procedure is known
    //emitCode(BSR, 0, 0, 0);

    // push exit code in return register onto stack
    //emitCode(PSH, RR, SP, 4);

    // halt machine by invoking exit
    
    //emitCode(BSR, 0, 0, codeLength + 1);

    // emit library code for exit right here
    emitExit();
   
    // similarly, emit library code for malloc, getchar, and putchar
    //...
    
    // init scanner
    init_scanner();
    // init parser
    initParser();
       
    // reserve first instruction for GP fixup and second and third for heap_pointer 4ed for RR
    emitCode(NOP,0,0,0);   
    emitCode(NOP,0,0,0);
    emitCode(NOP,0,0,0);
    emitCode(NOP,0,0,0);
    
    // jump to end of malloc lib
    emitCode(J, 0, 0, 26);	// 26 = 4 reserved + j + delay slot + malloc code length
    emitCode(NOP, 0, 0, 0);
   
    // load malloc
    emitMalloc();
          
    // reserved for jump to main()
    emitCode(NOP, 0, 0, 0);	// set LINK to BREAK 0 0 0 (end of code)
    emitCode(NOP, 0, 0, 0);	// jump to main()
    emitCode(NOP, 0, 0, 0);   // delay slot
    
    // get first symbol from scanner
    getCurrentSymbol(); 
     
    // invoke compiler
    cstar();
          
    // set GP     
    setGlobalPointer();
    
    // jump to main()
    setStartAddessToMain();		
		
    // stop code execution
    emitCode(BREAK, 0, 0, 0);
     
    printf("CODE Length %d\n", codeLength);
    printCodeArray();
	
    // write code to standard output
    // writeBinary();
  
    char* name = "a.mipster";
     
    mipster_dump_raw_args_t args;
    args.code_addr = (int)code;
    args.code_size = 4 * codeLength;
    args.name_addr = (int)name;
    args.name_size = 10; 

    mipster_dump_raw(args);  
	 
    return 0;
}
//-----------------------------

//This is a dynamically allocated, list-based implementation of a symbol table in C*, extend as needed.

int createSymbolTableEntry(int data) {
	// add current identifier as symbol into symbol table
    int* symbolTableCursor;

    // 1 list pointer, 1 identifier pointer, 1 data integer, 1 unused field
    symbolTableCursor = mipster_malloc(4 * 4);

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
    
    symbolTableCursor = symbolTable + 3;
    *symbolTableCursor = is_pointer;  

    return data;
}

int* getSymbolTableEntry() {
	// look up current identifier in symbol table. if found, return pointer to list element. return 0 otherwise
    int* symbolTableCursor;

    symbolTableCursor = symbolTable;

    while (symbolTableCursor != 0) {
        if (identifierMatch( *(symbolTableCursor + 1) )) {
            // return entry
			print_symbol_table_entry(symbolTableCursor);
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
	// do strcmp(symbolTableIdentifier, identifier); return 1 if strings match, 0 otherwise.
    int* identifierCursor;
    int* symbolTableIdentifierCursor;

    identifierCursor = identifier;
    symbolTableIdentifierCursor = symbolTableIdentifier;

    while (1) {
        if (*identifierCursor == *symbolTableIdentifierCursor) {
			// characters at current position are equal
            if (*identifierCursor == 0) {
				// end of identifiers: match!
                return 1;
			} else {
				// continue matching
                identifierCursor = identifierCursor + 1;
                symbolTableIdentifierCursor = symbolTableIdentifierCursor + 1;
            }
        } else {
			// mismatch found - terminating
            return 0;
		}
    }
}


//-----------------------------

//Here is the above recursive-descent parser decorated into a single-pass compiler that generates DLX code for arithmetic expressions in C*. Assignments are to be done as part of an assignment.

void cstar() {
	while (symbol != -1) {
		variableORprocedure();
	}
}

void variableORprocedure() {
	int have_asterisk;
	debug(E_VARIABLEORPOCEDURE);

	have_asterisk = 0;
	is_pointer = 0;
	
	if (symbol == ASTERISK) {
		have_asterisk = 1;
	}

	if (symbol == VOID) {
		getCurrentSymbol();
		if (symbol == ASTERISK) {
			getCurrentSymbol();
		}
		debug(E_VOID);
		procedure(have_asterisk);
	}
	else if (type()) {
		if (lookahead == SEMICOLON) {
			variable(have_asterisk);		
		}
		else if (lookahead == LPARENS) {
			procedure(have_asterisk);
		}
	}
	else {
		syntaxError(E_VARIABLEORPOCEDURE);
		getCurrentSymbol();
	}
}

// local var, function parameter
void declaration() {
	type();
	debug(E_DECLARATION);
	if (symbol == IDENTIFIER) {
	
		createSymbolTableEntry(allocateLocalVariable());
		dump_symbol_table(symbolTable);
		
		getCurrentSymbol();
	}
	else {
		syntaxError(E_DECLARATION);
	}
}

// global var
void variable(int have_asterisk) {
	debug(E_VARIABLE);
	
	if (symbol == IDENTIFIER) {
		createSymbolTableEntry(allocateGlobalVariable());

		dump_symbol_table(symbolTable);

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
			is_pointer = 1;
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
	
	int have_asterisk;
	have_asterisk = 0;

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
		have_asterisk = 1;
		getCurrentSymbol();
		if (symbol == IDENTIFIER) {
			if (lookahead == ASSIGN) {
				assignment(have_asterisk);
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
			assignment(have_asterisk);
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

void assignment(int have_asterisk) {
	debug(E_ASSIGNMENT);
		
	if (symbol == IDENTIFIER) {

		int *tmp = getSymbolTableEntry();
				
		getCurrentSymbol();
		if (symbol == ASSIGN) {
			getCurrentSymbol();
			
			expression();
			
			// is pointer
			if (tmp[3] == 1) {
				if (have_asterisk == 1) {	// dereference
					if (tmp[2] < 0) {
						emitCode(LDW, allocatedRegisters +1, GP, tmp[2]);
						emitCode(SW, allocatedRegisters, allocatedRegisters +1, 0);
					} 
					else {
						emitCode(LDW, allocatedRegisters +1, FP, tmp[2]);
						emitCode(SW, allocatedRegisters, allocatedRegisters +1, 0);					
					}
				}
				else {					// ref
					if (tmp[2] < 0) {
						emitCode(SW, allocatedRegisters, GP, tmp[2]);
					}
					else {
						emitCode(SW, allocatedRegisters, FP, tmp[2]);
					}
				}
			}
			else {
				if (tmp[2] < 0) {
					emitCode(SW, allocatedRegisters, GP, tmp[2]);
				}
				else {
					emitCode(SW, allocatedRegisters, FP, tmp[2]);
				}
			}
			
			allocatedRegisters = allocatedRegisters -1;
		}
	}
	else {
		syntaxError(E_ASSIGNMENT);
	}
}

int relation_expression() {
	debug(E_RELATION_EXPRESSION);

	// flag if there is no comparison sign
	int no_comparison = 0;	
	int cmp_symbol = 0;	
		
	expression();

	if (symbol == EQUAL) {
		cmp_symbol = BNE;
		getCurrentSymbol();
		expression();	
	}
	else if (symbol == NOTEQ) {
		cmp_symbol = BEQ;
		getCurrentSymbol();
		expression();
	}
	else if (symbol == GT) {
		cmp_symbol = BLEZ;
		getCurrentSymbol();
		expression();
	}
	else if (symbol == GTEQ) {
		cmp_symbol = BLTZ;
		getCurrentSymbol();
		expression();
	}
	else if (symbol == LT) { 
		cmp_symbol = BGEZ;
		getCurrentSymbol();
		expression();
	}
	else if (symbol == LTEQ) { 
		cmp_symbol = BGTZ;
		getCurrentSymbol();
		expression();
	}
	else {	// no comparison: 0 == false, else true
		no_comparison = 1;
		cmp_symbol = BEQ;
	}
	
	if (no_comparison != 1) {
		emitCode(SUB, allocatedRegisters - 1, allocatedRegisters - 1, allocatedRegisters);
		allocatedRegisters = allocatedRegisters - 1;
	}
	
	return cmp_symbol;
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
    
    int type_prev;
    int type_curr;

    if (symbol == MINUS) {
        sign = 1;
        getCurrentSymbol();
    } else {
        sign = 0;
    }

    operating_on_pointer = 0;
    term();
    type_prev = operating_on_pointer;

    // assert: allocatedRegisters == n + 1

    if (sign) {
        emitCode(SUB, allocatedRegisters, ZR, allocatedRegisters);
    }

    while (isSymbolPlusOrMinus()) {
        operatorSymbol = symbol;
        getCurrentSymbol();

	   operating_on_pointer = 0;
        term();
        type_curr = operating_on_pointer;

	   if(type_prev == 1) {
		     if(type_curr == 1) {
				// current and previous are pointers - do nothing
			} else {
				// previous is pointer - multiply current
				// temporarily allocate one new register to hold the constant (because there is no MULI)
				    emitCode(ADDI, allocatedRegisters + 1, ZR, 4);
	        		    emitCode(MUL, allocatedRegisters, allocatedRegisters, allocatedRegisters + 1);
			}
	   } else {
			if(type_curr == 1) {
				// current is pointers - multiply previous
				// temporarily allocate one new register to hold the constant (because there is no MULI)
				    emitCode(ADDI, allocatedRegisters + 1, ZR, 4);
	        		    emitCode(MUL, allocatedRegisters-1, allocatedRegisters-1, allocatedRegisters + 1);
			} else {
				// no pointers involved - do nothing
			}
 	   }

	   type_prev = type_curr;

        if (operatorSymbol == PLUS) {
            emitCode(ADD, allocatedRegisters - 1, allocatedRegisters - 1, allocatedRegisters);
        }
        else {
            emitCode(SUB, allocatedRegisters - 1, allocatedRegisters - 1, allocatedRegisters);
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

        if (operatorSymbol == ASTERISK) {
            emitCode(MUL, allocatedRegisters - 1, allocatedRegisters - 1, allocatedRegisters);
        }
        else {
            emitCode(DIV, allocatedRegisters - 1, allocatedRegisters , 0);	// TODO fix LO, HI
            emitCode(MFLO,allocatedRegisters -1, 0, 0);
            //emitCode(MFHI,allocatedRegisters -1, 0, 0);
        }

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
    int var_offset = getGlobalVariableOffset();
    int* symbolTableCursor;
    
    cast();

    if (symbol == ASTERISK) {
        dereference = 1;

        getCurrentSymbol();
    } 
    else {
        dereference = 0;
    }

	// TODO local variables!!
    if (symbol == IDENTIFIER) {
    	   // call
    	   if (lookahead == LPARENS) {
    	        call();
    	   }
    	   // variable
    	   else {
    	   	   if (var_offset < 0) {
			   allocatedRegisters = allocatedRegisters + 1;
			   
			   emitCode(LDW, allocatedRegisters, GP, var_offset);
			      
			   symbolTableCursor = getSymbolTableEntry();
			   symbolTableCursor = symbolTableCursor + 3; // type field
		        if(*symbolTableCursor == 1) {
			   	operating_on_pointer = 1;
			   }
			   
			   getCurrentSymbol();
		   }
		   else {
		   		// TODO local pointer arith like above
		   	   allocatedRegisters = allocatedRegisters + 1;
		   	   
		   	   emitCode(LDW, allocatedRegisters, FP, var_offset);
			   
			   getCurrentSymbol();
		   }
        }
    } 
    else if (symbol == INTEGER) {
        allocatedRegisters = allocatedRegisters + 1;
	   ADDI32bitImmediate(ADDI, allocatedRegisters, ZR, integer);
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

    if (dereference) {
        emitCode(LDW, allocatedRegisters, allocatedRegisters, 0);
        operating_on_pointer = 0;
    }

    // assert: allocatedRegisters == n + 1
}
//-----------------------------

//Store global variable offsets in symbol table.

int getGlobalVariableOffset() {
	// looks up current identifier in symbol table; if found, returns "data integer" field of table entry. otherwise allocates new global variable first.
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

int allocateLocalVariable() {
	if(allocatedLocalVariables == -1) {
		// FIXME: this is a parser error - trying to allocate a local variable when no local context exists
		printf("[PARSER] ERROR: trying to allocate a local variable when no local context exists - allocating as global instead!\n");
		return allocateGlobalVariable();
	}
    allocatedLocalVariables = allocatedLocalVariables + 1;

    // each variable needs 4 bytes, local variable offsets are positive
    return 4 * allocatedLocalVariables;
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
    branchBackwardsToWhile = codeLength -1;

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
        
	   branchInstruction = relation_expression();	
		
        // remember address of next instruction which
        // conditionally branches forward to end of while
        // if parsed comparison evaluates to false!
        branchForwardToEndOfWhile = codeLength;

        // assert: allocatedRegisters == 1

        // target address unknown, so just say 0 here, fixup is done below
 	   
        emitCode(branchInstruction, allocatedRegisters, 0, 0);
        emitCode(NOP,0,0,0);


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
    emitCode(BR, 0, 0, branchBackwardsToWhile - codeLength);
    emitCode(NOP,0,0,0);

    // here will be the first instruction after the loop
    // so point the conditional branch instruction from above here
   
    fixup(branchForwardToEndOfWhile);
}

void ifStatement() {
	debug(E_IF);

    int branchForwardToElse;
    int branchForwardToEndOfIf;
    int branchForwardToEndOfIfInElse;
    
    int branchInstruction;

    if (symbol == IF) {
        getCurrentSymbol();
    }
    else {
        syntaxError(IF); // if expected!
    }

    if (symbol == LPARENS) {
        getCurrentSymbol();
        
	   branchInstruction = relation_expression();
	   	
	   // jump address to else or endif
	   branchForwardToElse = codeLength;
	   
	   // branch to endif or if there is a else to else body. 
	   emitCode(branchInstruction, allocatedRegisters, 0, 0);
	   emitCode(NOP,0, 0, 0);
	   
	   allocatedRegisters = allocatedRegisters - 1;

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
    
    // jump address to endif
    branchForwardToEndOfIf = codeLength; 
    
    // branch to endif after if body
    emitCode(BR, 0, 0, 0);
    emitCode(NOP,0, 0, 0);
    
    if (symbol == ELSE) {
    	   debug(E_ELSE);
    	   
    	   // branch address to endif in else
    	   branchForwardToEndOfIfInElse = codeLength; 
    	   
    	   // branch to endif in else
    	   emitCode(BR, 0, 0, 0);
    	   emitCode(NOP,0, 0, 0);
    	   
    	   // fixup else branch to else body if there is a else
    	   fixup(branchForwardToElse);
    	   
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
        // jump to endif before else body
        fixup(branchForwardToEndOfIfInElse);       
    } 
    else {
    	   // fixup else branch to endif if there is no else 
        fixup(branchForwardToElse);
    }
    // fixup branch to endif
    fixup(branchForwardToEndOfIf);         
}


void fixup(int codeAddress) {
	// assert: -2^15 <= codeLength - codeAddress <= 2^15 - 1

	// branch from instruction at codeAddress to instruction at codeLength
	setParameterCInCode(codeAddress, codeLength - codeAddress);
}

void setParameterCInCode(int codeAdress, int value) {
	int *codecursor;
	
	codecursor = code + codeAdress;
	*codecursor = *codecursor + value -1;
}
//-----------------------------

// Procedure calls are (in principle) done as follows. Note that (in reality)
// the procedure identifier needs to be parsed before invoking call and then
// passed into call (by factor and statement) because the grammar (for factor
// and statement) is not fully left-factored. In other words, the parser can
// only know if it is dealing with a variable access or a procedure call after
// it has seen the symbol that appears after the identifier, i.e., if the
// parser then sees a left parenthesis (call) or not (identifier). This is
// called a lookahead of two (which can nevertheless be reduced to one by
// left factoring).

void call() {
	debug(E_CALL);
    // assert: n = allocatedRegisters

    // both must be local variables to work for nested expressions
    int* procedureIdentifier;
    int savedAllocatedRegisters;
    int procedureAddress;

    int *restoreIdentifier;

    if (symbol == IDENTIFIER) {
        // save procedure identifier for symbol table lookup below
        procedureIdentifier = identifier;

        getCurrentSymbol();

        savedAllocatedRegisters = allocatedRegisters;

        // save allocated registers on stack
        while (allocatedRegisters > 0) {
            //emit(PSH, allocatedRegisters, SP, 4);
		  emitCode(ADDI, SP, SP, -4);
	       emitCode(SW, allocatedRegisters, SP, 0);
	       
            allocatedRegisters = allocatedRegisters - 1;
        }

        // assert: allocatedRegisters == 0

        if (symbol == LPARENS) {
            getCurrentSymbol();

            if (symbol != RPARENS) {
                expression();

                // push value of expression (actual parameter) onto stack
                //emit(PSH, allocatedRegisters, SP, 4);
		  	 emitCode(ADDI, SP, SP, -4);
	      	 emitCode(SW, allocatedRegisters, SP, 0);
	      	 
                // register for value of expression is not needed anymore
                allocatedRegisters = allocatedRegisters - 1;

                // assert: allocatedRegisters == 0

                while (symbol == COMMA) {
                    getCurrentSymbol();
                    expression();

                    // push value of expression (actual parameter) onto stack
                    //emit(PSH, allocatedRegisters, SP, 4);
		  	 	emitCode(ADDI, SP, SP, -4);
	      	 	emitCode(SW, allocatedRegisters, SP, 0);
	      	 
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

	   // save current identifier
	   restoreIdentifier = identifier;
        // restore procedure identifier for symbol table lookup
        identifier = procedureIdentifier;

        procedureAddress = setProcedureAddress();
	
	  // function call but declaration is missing 
        if (procedureAddress == codeLength) {
            // create a new fixup chain
            //emit(BSR, 0, 0, 0);
            emitCode(JAL, 0, 0, 0);
        } 
        else if (getOpcodeFromCode(procedureAddress) == BSR) {
            // link to the head of an existing fixup chain
            //emit(BSR, 0, 0, procedureAddress);
        }
        else {
            // branch to subroutine to invoke procedure
            //emit(BSR, 0, 0, procedureAddress - codeLength);
            emitCode(JAL, 0, 0, procedureAddress);
            emitCode(NOP, 0, 0, 0);
            // TODO mb use BR and set LINK instead of JAL to use a relative branch instead of a jump instruction
        }
		
        // assert: allocatedRegisters == 0
        // restore allocated registers from stack
      	while (allocatedRegisters < savedAllocatedRegisters) {
          	allocatedRegisters = allocatedRegisters + 1;
			
      	     //emit(POP, allocatedRegisters, SP, 4);
      	     emitCode(LDW, allocatedRegisters, SP, 0);
			emitCode(ADDI, SP, SP, 4);
          }
          
          // save return value into current register
          allocatedRegisters = allocatedRegisters +1;
          emitCode(ADD, allocatedRegisters, ZR, RR);
          
    } else {
        syntaxError(E_CALL); // identifier expected!
    }
    // ? assert: allocatedRegisters == n ?
    identifier = restoreIdentifier;
}
//-----------------------------

//Store procedure addresses in symbol table and create fixup chains for forward declarations.

int setProcedureAddress() {
	int* symbolTableCursor;
	int savedAddress;

	printf("[PARSER] looking up procedure '%ls'\n", identifier);

	// try to look up current identifier
	symbolTableCursor = getSymbolTableEntry();

	if (symbolTableCursor != 0) {
		// found a symbol table entry - do weird shit with it.
		printf("\t -> found at %d\n", symbolTableCursor);
		symbolTableCursor = symbolTableCursor + 2;

		savedAddress = *symbolTableCursor;

		if (getOpcodeFromCode(savedAddress) == BSR) {
			// save address of next instruction which may point to
			// the new head of an existing fixup chain
			// or the beginning of a procedure body
			*symbolTableCursor = codeLength;
		}

		return savedAddress;
	} else {
		printf("\t -> not found");
		printf("\n");
		// procedure not found but who cares,
		// just create entry in symbol table,
		// and save address of next instruction
		return createSymbolTableEntry(codeLength);
	}
}

int getOpcodeFromCode(int address) {
	int shift;
	int opcode;
	int *codecursor;
	
	codecursor = code;
	codecursor = codecursor + address;
		
	opcode = *codecursor;
	// check last 5 bits of the instruction for JAL
	shift = m_pow(26);
	opcode = opcode / shift;
	
	return opcode;
}

//-----------------------------

//Procedure definitions are next. Here the construction of the local symbol table containing the parameters and local variables of a procedure is still to be done as part of an assignment. Also, the access of parameters and local variables still needs to be implemented (in factor and assignment).

void procedure(int have_asterisk) {
	debug(E_PROCEDURE);
    // assert: allocatedRegisters == 0

    // may be global variables but are anyway nicer like that
    int callBranches;
    int parameters;
    int localVariables;

	int *global_symbol_table;

	// back up start of global symbol table so appending shit adds to the local part
	global_symbol_table = 0; // not yet, only after symbol of the procedure itself is added

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

		// create symbol table entry for procedure
		callBranches = setProcedureAddress();

		// enter a local variable context
		allocatedLocalVariables = 0;
		global_symbol_table = symbolTable;
		printf("[PARSER] last entry of global symbol table: %d\n", global_symbol_table);

		dump_symbol_table(symbolTable);

		getCurrentSymbol();

		printf("CALL BRANCHES CODE L: %d %d\n", callBranches, codeLength);

          if (callBranches != codeLength) {
             if (getOpcodeFromCode(callBranches) == BSR) {
                // fixlink(callBranches);
             }
          }
          else {
             // procedure defined more than once!
             //declarationError(PROCEDURE);
             printf("ERROR procedure defined more than once!\n\n");
          }

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
		  prologue(localVariables * 4);

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

		  printf("\n[PARSER] ===== begin procedure statement block =====\n\n");
		  
            while (symbol != RBRACE) {
            	statement();
            }
            
		  printf("\n[PARSER] ===== end procedure statement block =====\n\n");
		  
            getCurrentSymbol();

		  // set return address
            fixlink(returnBranches);

            // procedure epilogue
		  epilogue(parameters * 4);

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

	// restore previous symbol table state
	if(global_symbol_table != 0 ) {
		dump_symbol_table(symbolTable);
		symbolTable = global_symbol_table;
		allocatedLocalVariables = -1;
		printf("[PARSER] clearing local symbol table entries (reverting to %d)\n", symbolTable);
		dump_symbol_table(symbolTable);
	}
	printf("[PARSER] end of procedure\n");

    // assert: allocatedRegisters == 0
}

void prologue(int loadSize) {
	// push LINK
	emitCode(ADDI, SP, SP, -4);
	emitCode(SW, LINK, SP, 0);
	// push FP
	emitCode(ADDI, SP, SP, -4);
	emitCode(SW, FP, SP, 0);
	// set FP to SP
	emitCode(ADD, FP, 0, SP);
	// move SP (loadsize = local variables)
	emitCode(ADDI, SP, SP, loadSize * -1);
}

void epilogue(int paramSize) {
	emitCode(ADD, SP, 0, FP);
	// pop FP
	emitCode(LDW, FP, SP, 0);
	emitCode(ADDI, SP, SP, 4);
	// pop SP
	emitCode(LDW, LINK, SP, 0);
	emitCode(ADDI, SP, SP, 4);
	
	// deallocate paramsize
	emitCode(ADDI, SP, SP, paramSize);
	
	// return 
	//emitCode(BR, 0, 0, LINK);
	emitCode(JR, LINK, 0, 0);
	emitCode(NOP, 0, 0, 0);
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
	if (symbol == SLASH) {
		return 1;
	}
	return 0;
}

//-----------------------------

//Fixup a whole chain of branch instructions.

int getParameterCFromCode(int codeAddress) {
	int *codecursor;
	int shift;
	int high_bits;
	int parameterC;

	codecursor = code;
	codecursor = codecursor + codeAddress;
		
	parameterC = *codecursor;
	
	shift = m_pow(26);
	high_bits = (parameterC / shift) * m_pow(26);								
	parameterC = parameterC - high_bits;
	
	return parameterC; 						
}

void fixlink(int codeAddress) {
    int previousCodeAddress;

    while (codeAddress != 0) {
        previousCodeAddress = getParameterCFromCode(codeAddress);

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
        emitCode(ADD, RR, ZR, allocatedRegisters);

        // register for value of expression is not needed anymore
        allocatedRegisters = allocatedRegisters - 1;
    }

    // unconditional branch to procedure epilogue
    // maintain fixup chain for later fixup
    emitCode(BR, 0, 0, returnBranches);
    emitCode(NOP, 0, 0, 0);

    // new head of fixup chain (-2 = BR + NOP)
    returnBranches = codeLength - 2;

    // assert: allocatedRegisters == 0
}
//-----------------------------

//Here is the complete C* syntax in EBNF form.

//type = "int" [ "*" ] .

//relation_expression = expression [ ( "<=" | "==" | ">=" | "<" | ">"  | "!=" ) expression ]                        
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






