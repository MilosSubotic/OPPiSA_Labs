#include "assemblerLib.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "writeElf.h"
#include "registersSet.h"


using namespace std;



/**
* table of symbols map type
*/
typedef map<string, Symbol> SymbolTable;

/**
* generated operation codes list type
*/
typedef list<long> MachineCodeList;

/**
* table of literals pair
*/
typedef pair<long, long> LiteralTablePair;

/**
* table of literals map type
*/
typedef map<long, long> LiteralTable;

/**
* table of extern symbols map type
*/
typedef map<string, ExternSymbol> ExternSymbolTable;

/**
* table of symbols pair
*/
typedef pair<string, Symbol> SymbolPair;

/**
* table of extern symbols pair
*/
typedef pair<string, ExternSymbol> ExternSymbolPair;

/**
* instructions list type
*/
typedef list<char*> InstructionList;

/**
* addresses (locations) of instructions list type
*/
typedef list<long> InstLocationList;

/**
* error list type
*/
typedef list<char*> ErrorList;

/**
* @brief   Calculates the value of passed expression
*
* @param[in/out]   expr        expression to calculate
* @param[in]   lineNumber  current line number
* @param[in]   line        current line
* @param[in]   location    current instruction's location
*/
void exprCalc(char* &expr, long lineNumber, const char* line, long location = 0);

/**
* @brief   Gets instruction (full machineCode) from instruction symbol
*
* @param[in]   instrSymbol symbol of instruction to find
* @param[out]  getInstr    opcode of instruction if found
*
* @return  type of intruction; OT_NONE if instruction not found
*/
OpType getInstr(const char* instrSymbol, const OpCode* &getInstr);

/**
* @brief   Writes machine code and all data in relocatable ELF object file
*
* @param[in]   outFileName  file name of destination ELF file
*
* @return ???
*/
int writeElf(std::string outFileName);

/**
* @brief   Reports an error if output file could not be opened for writing
*
* @param[in]   filename  file name that couldn't be opened
*/
void outFileOpenError(std::string filename);


bool checkEnoughParams(long lineNumber, string line, ParamStack params, int nExpected);


void addError(long lineNumber, string line, string errorText);

void addError(SourceLine sourceLine, std::string errorText) {
	addError(sourceLine.lineNumber, sourceLine.sourceLine, errorText);
}


/**
 * map registersSet
 */
static Registers registersSet;

/**
 * map g_opCodeTable (table of operation codes)
 */
static OpCodeTable g_opCodeTable;

/**
 * error list
 */
static ErrorList g_errorList;

/**
 * source list
 */
static SourceList g_sourceList;

/**
 * symbol table
 */
static SymbolTable g_symbolTable;

/**
 * global symbol table
 */
static SymbolTable g_globalSymbolTable;

/**
 * extern symbols list
 */
static ExternSymbolTable g_externSymbolTable;

/**
 * const data list
 */
static LiteralTable g_literalTable;

/**
 * machine code list
 */
static MachineCodeList g_machineCodeList;

/**
 * instruction location list
 */
static InstLocationList g_instLocationList;

/**
 * instructions list
 */
static InstructionList g_instructionList;

//  previous location in section
static long g_sectionDataLocation = SECTION_DATA_AREA;
static long g_sectionTextLocation = SECTION_TEXT_AREA;


/**
* @brief Trims trailing characters from a string.
*
* @param[in]  string          string to be trimmed
* @param[in]  tc              character to be trimmed from string
*
* @return pointer to the first character in the string to be trimmed
*/
char * strrspn(const char *string, const char* tc){
    char *p;

    p = (char *)string + strlen(string);
    while (p != (char *)string)
        if (!strchr(tc, *--p))
            return (++p);

    return (p);
}

/**
* @brief Returns integer value of int or hex string
*
* @param[in] string    string to convert
* @param[out] isNum     true if string is hex or dec number; false otherwise
*
* @return converted value
*/
long toint(const char* string, bool &isNum){
    long  val;
    char* stringCopy;
    char* end;

    stringCopy = (char*)malloc((strlen(string)+1)*sizeof(char));
    strcpy(stringCopy, string);

    char* lower = _strlwr(stringCopy);
    
    if(lower[0]=='0' && lower[1]=='x'){
        val = strtoul(lower+2, &end, 16);
    }else{
        val = strtol(lower, &end, 10);
    }

    isNum = end==lower+strlen(lower);

    free(stringCopy);
    
    return val;
}

/**
* @brief Returns integer value of int or hex string
*
* @param[in] string    string to convert
*
* @return converted value
*/
long toint(const char* string){
	bool temp;
	return toint(string, temp);
}

long constConv(std::string constant, bool &isNum){
	return toint(constant.c_str(), isNum);
}

long constConv(std::string constant){
	return toint(constant.c_str());
}

bool getExecutable(std::string &executable, SourceLine sourceLine, bool addErrorToList) {
	string line = sourceLine.sourceLine;
	long lineNumber = sourceLine.lineNumber;
    char* pos;
    string cmd;
    char* msg;
    bool ret = false;
	char* freeExec;
	char* exec = freeExec = new char[line.size() + 1];

	strcpy(exec, line.c_str());

    exec[strcspn(exec, COMMENT)] = 0; // Parse line with comments
    
    if(pos = strpbrk(exec, LABEL))
        exec = pos+1;                         // Parse line with labels
    
    exec += strspn(exec, WHITESPACE); // eat whitespaces
        
    if(strlen(exec) && exec[0]!=DIRECTIVE){   // Skip lines with nothing on... or starting with .
        if(getCommand(cmd, exec)){
			if(g_opCodeTable.find(cmd)!=g_opCodeTable.end()){
                ret = true;
            } else {
                if (addErrorToList) {
					msg = (char*) malloc((strlen(cmd.c_str())+25)*sizeof(char));
					sprintf(msg, "Command not recognized: %s", cmd.c_str());
                    addError(lineNumber, exec, msg);
                    free(msg);
                }
            }
        }
    }

	executable.assign(exec);
	delete[] freeExec;
    return ret;
}

bool getCommand(string &cmd, string line) {
    char* pos;
	char* freeCommand;
	char* command = freeCommand = new char[line.size() + 1]; // (char*)malloc((strlen(line.c_str()) + 1) * sizeof(char));

	strcpy(command, line.c_str());

    command[strcspn(command, COMMENT)] = 0;      // Parse line with comments
    
    if(pos = strpbrk(command, LABEL))
        command = pos+1;                       // Parse line with labels
    
    command += strspn(command, WHITESPACE);      // eat whitespaces
        
    if(strlen(command) && command[0]!=DIRECTIVE){         // Skip lines with nothing on... or starting with .
        command[strcspn(command, OPSEP)] = 0;             // throw everything after first op
        command[strcspn(command, COMMAND_SEP)] = 0;       // throw everything after command
        _strlwr(command);
		cmd.assign(command);
		delete[] freeCommand;
        return true;
    }

    cmd = "";
	delete[] freeCommand;
	return false;
}

Directive getDirective(SourceLine sourceLine) {
	std::string line = sourceLine.sourceLine;
	long lineNumber = sourceLine.lineNumber;
    char* pos;

	char* dirMem = (char*) malloc((strlen(line.c_str())+1)*sizeof(char));
    char* dir = dirMem;
    char* msg;

	strcpy(dir, line.c_str());

    dir[strcspn(dir, COMMENT)] = 0;       // Parse dir with comments
    
    if(pos = strpbrk(dir, LABEL))
        dir = pos+1;                        // Parse dir with labels
    
    dir += strspn(dir, WHITESPACE);       // Skip dirs with nothing on... (eat whitespaces)
        
    if(strlen(dir) && dir[0]==DIRECTIVE){ // Skip dirs with nothing on... ommit .
        dir++;
        if( !strncmp(dir, "word", 4) ){
            return D_WORD;
        }else if( !strncmp(dir, "space", 5) ){
            return D_SPACE;
        }else if( !strncmp(dir, "org", 3) ){
            return D_ORG;
        }else if( !strncmp(dir, "globl", 5) ){
            return D_GLOBL;
        }else if( !strncmp(dir, "extern", 6) ){
            return D_EXTERN;
        }else{
            msg = (char*) malloc((strlen(dir)+27)*sizeof(char));
            sprintf(msg, "Directive not recognized: %s", dir);
			addError(lineNumber, line, msg);
            free(msg);
        }
    }

    free(dirMem);

    return D_NONE;
}

bool getSymbol(string &symbol, SourceLine sourceLine) {
	string line = sourceLine.sourceLine;
    char* pos;
	char* freeSym;
    char* sym = freeSym = new char[line.size() + 1]; // (char*)malloc((strlen(line.c_str()) + 1) * sizeof(char));

	strcpy(sym, line.c_str());

    sym[strcspn(sym, COMMENT)] = 0;       // Parse line with comments
    
    if(pos = strpbrk(sym, LABEL)){          // Parse line with labels
        pos[0]=0;                               // end of symbol at the place of :
        sym += strspn(sym, WHITESPACE);   // Skip dirs with nothing on... (eat whitespaces)
		symbol.assign(sym);
		delete[] freeSym;
        return true;
    }

    symbol = "";
	delete[] freeSym;
    return false;
}


/**
* @brief   Extracts all parameters from current line (after command).
*
* @param[out]   params      ParamStack valiable that will hold parameters if they are found
* @param[in]   line        the current line to examine
* @param[in]   lineNumber  the number of current line
* @param[in]   chopDollar  set if you want to chop dollar sign; default value is false
* @param[in]   unWind      set if you want to unwind parameters; default value is false
* @param[in]   location    current instruction's location; default value is 0
*
* @return  true if param found; false otherwise
*/
bool getParams(ParamStack &params, string line, long lineNumber, bool chopDollar, bool unWind, long location = 0) {
    char *pos;
    int  addr;
    char *offset;
    char *memoryOp2;
    char *memoryOp2new;
    char *newParam;
    int  len;
    char *msg;
    bool isNum;
    list<char*> params_list;

	char* param = (char*) malloc((strlen(line.c_str())+1)*sizeof(char));
    char* param0 = param;

	Registers::const_iterator registerSetIter;

	strcpy(param, line.c_str());

    param[strcspn(param, COMMENT)] = 0;           // Parse line with comments
    
    if(pos = strpbrk(param, LABEL))
        param = pos+1;                              // Parse line with labels
    
    param += strspn(param, WHITESPACE);           // eat whitespaces

    while(!params.empty()){                        // clear params stack
        params.pop();
    }

    if(strlen(param)){                              // Skip lines with nothing on
        param += strcspn(param, COMMAND_SEP);     // drop command
        param += strspn(param, WHITESPACE);       // eat whitespaces at the beging

        offset = (char*) malloc((strlen(param)+1)*sizeof(char));

        // special case for lw and sw instructions with a 100($1) parameter...
        memoryOp2 = param + strcspn(param, OPSEP);        // drop first operand
        memoryOp2 += strspn(memoryOp2, OPSEP);
        if( sscanf_s(memoryOp2, "%d($%[^)])", &addr, offset, strlen(param)+1) == 2 ){
            memoryOp2 = (char*) malloc((strlen(param)+1)*sizeof(char));
            memoryOp2new = (char*) malloc((strlen(param)+1)*sizeof(char));
            
            sprintf(memoryOp2, "%d($%s)", addr, offset);    // make original second operand (ie. 100($25))
            sprintf(memoryOp2new, "$%s,%d ", offset, addr); // replace "(" with "," and replace ")" with " " and make reverse order
            
            memcpy(strstr(param, memoryOp2), memoryOp2new, strlen(memoryOp2new)*sizeof(char));  // replace original with new operand
            
            free(memoryOp2);
            free(memoryOp2new);
        }
        
        pos = strrspn(param, WHITESPACE);           // trim trailing whitespaces
        pos[0] = 0;

	    // split with commas, spaces and tabs (operand separators)
        while(strlen(param)){
            len = strcspn(param, OPSEP);
            newParam = (char*)malloc((len+1)*sizeof(char));
            memcpy(newParam, param, len*sizeof(char));
            newParam[len]=0;
            params.push(string(newParam));
            param += len;
            param += strspn(param, OPSEP);
        }

        while (!params.empty()) {
            //newParam = params.top();
			newParam = (char*)malloc((params.top().length()+1)*sizeof(char));
			strcpy(newParam, params.top().c_str());

            params.pop();

            if (unWind) {
                // replace register alias name with real name (e.g. $at->$1)
                if(newParam[0] == '$'){
                    registerSetIter = registersSet.find(newParam);
                    if(registerSetIter != registersSet.end()){
						strcpy(newParam, registerSetIter->second.c_str());
                    }else{
                        if(constConv(newParam+1, isNum)>31 || !isNum){
                            msg = (char*) malloc((strlen(newParam)+59)*sizeof(char));
                            sprintf(msg, "Operand must be register. Register name \"%s\" not recognized.", newParam);
                            addError(lineNumber, line, msg);
                            free(msg);
                        }
                    }
                }

				exprCalc(newParam, lineNumber, line.c_str(), location);
            }
            if (chopDollar) {
                pos = newParam + strcspn(newParam, "$");
                if(pos!=newParam+strlen(newParam)){
                    strcpy(pos, pos+1);                     // Chop $..
                }
            }
            newParam += strspn(newParam, WHITESPACE); // eat whitespaces
            pos = strrspn(newParam, WHITESPACE);      // trim trailing whitespaces
            pos[0] = 0;

            params_list.push_back(newParam);
        }	

        while(!params_list.empty()){
            params.push(string(params_list.front()));
            params_list.pop_front();
        }

        free(offset);

        free(param0);

        if(params.empty())
            return false;
        else
            return true;
    }

	return false;
}


bool getParams(ParamStack &params, SourceLine sourceLine)
{
	return getParams(params, sourceLine.sourceLine, sourceLine.lineNumber, false, false, 0);
}

void exprCalc(char* &expr, long lineNumber, const char* line, long location) {
    char* operand;
    char* rest;
    char* pos;
    char* msg;
    char* dummy;
    long  value;
    bool  isNum;

    if(strlen(expr)){                                  // Skip lines with nothing on
        operand = (char*) malloc((strlen(expr)+1)*sizeof(char));
        rest    = (char*) malloc((strlen(expr)+1)*sizeof(char));
        if( sscanf_s(expr, "%[^(](%[^)])", operand, strlen(expr)+1, rest, strlen(expr)+1) == 2 ){
            operand += strspn(operand, WHITESPACE);   // eat whitespaces
            pos = strrspn(operand, WHITESPACE);       // trim trailing whitespaces
            pos[0] = 0;
            exprCalc(rest, lineNumber, line, location);

            if (!strcmp(operand, "LO")) {
                _ltoa(strtol(rest, &dummy, 10) & 0xffff, expr, 10);
                return;
            }
            else if (!strcmp(operand, "HI")) {
                _ltoa((unsigned long)(strtol(rest, &dummy, 10) & 0xffff0000) >> 16, expr, 10);
                return;
            }
            else if (!strcmp(operand, "ADDR")) {
                if (symbolExists(rest)) {
                    readSymbol(rest, value);
                    _ltoa(value, expr, 10);

                    if(strstr(line, "HI") != NULL)
                        g_symbolTable.find(rest)->second.relList.push_back(RelInfo(location*4, R_MIPS_HI16));
                    else if(strstr(line, "LO") != NULL)
                        g_symbolTable.find(rest)->second.relList.push_back(RelInfo(location*4, R_MIPS_LO16));
                    else
                        g_symbolTable.find(rest)->second.relList.push_back(RelInfo(location*4, R_MIPS_16));
                    
                    return;
                } else if (externSymbolExists(rest)) {
                    readExternSymbol(rest, value);
                    _ltoa(value, expr, 10);
                    
                    if(strstr(line, "HI") != NULL)
                        g_externSymbolTable.find(rest)->second.relList.push_back(RelInfo(location*4, R_MIPS_HI16));
                    else if(strstr(line, "LO") != NULL)
                        g_externSymbolTable.find(rest)->second.relList.push_back(RelInfo(location*4, R_MIPS_LO16));
                    else
                        g_externSymbolTable.find(rest)->second.relList.push_back(RelInfo(location*4, R_MIPS_16));
                    
                    return;
                } else {
                    msg = (char*) malloc((strlen(rest)+19)*sizeof(char));
                    sprintf(msg, "Symbol not found: %s", rest);
                    addError(lineNumber, line, msg);
                    free(msg);
                }
            }
        } else {
            expr += strspn(expr, WHITESPACE);       // eat whitespaces
            pos = strrspn(expr, WHITESPACE);         // trim trailing whitespaces
            pos[0] = 0;

            // Convert parameter to dec if 0xDeadBeef form on parameter
            value = constConv(expr, isNum);
            if(isNum){
                _ltoa(value, expr, 10);
            }
        }
        free(operand);
        free(rest);
    }

    return;
}

unsigned long makeMachineCode(std::string line, long lineNumber, long location){
    string       cmd;
    ParamStack params;
    string param0;
    string param1;
    string param2;
    char*       dummy;
	char* msg;
    bool        isNum;
    long        true_location = location;
    long        branchTarget;
    long        param_tmp;

    const       OpCode* instr;
    unsigned long machineCode = 0;

	if( getCommand(cmd, line) && getInstr(cmd.c_str(), instr)!=OT_NONE ){

        switch(instr->iformat){
            case(R):            // reg op
                getParams(params, line, lineNumber, false, true);
                if(checkEnoughParams(lineNumber, line, params, instr->paramNum)){
                    param0 = params.top();
                    params.pop();
                    param1 = params.top();
                    params.pop();
                    param2 = params.top();
                    params.pop();
                    // all operands for R instruction type must be registers
                    if(param0[0]!='$' || param1[0]!='$' || param2[0]!='$'){
                        addError(lineNumber, line, "All operands must be registers for this instruction");
                    }else{
                        param0.erase(param0.begin());
                        param1.erase(param1.begin());
                        param2.erase(param2.begin());
                        machineCode = (instr->code<<26) + (constConv(param1, isNum)<<21) + (constConv(param2, isNum)<<16) + (constConv(param0, isNum)<<11) + instr->funct;
                    }
                }
                break;
            case(JR):    // Special case for jr
                getParams(params, line, lineNumber, false, true);
                if(checkEnoughParams(lineNumber, line, params, instr->paramNum)){
                    param0 = params.top();
                    params.pop();
                    // operands for R instruction type must be register
                    if(param0[0]!='$'){
                        addError(lineNumber, line, "All operands must be registers for this instruction");
                    }else{
                        param0.erase(param0.begin());
                        machineCode = (instr->code<<26) + (constConv(param0, isNum)<<21) + instr->funct;
                    }
                }
                break;
            case(MF):    // Special case move from LO/HI
                getParams(params, line, lineNumber, false, true);
                if(checkEnoughParams(lineNumber, line, params, instr->paramNum)){
                    param0 = params.top();
                    params.pop();
                    if(param0[0]!='$'){
                        addError(lineNumber, line, "All operands must be registers for this instruction");
                    }else{
                        param0.erase(param0.begin());
                        machineCode = (instr->code<<26) + (constConv(param0, isNum)<<11) + instr->funct;
                    }
                }
                break;
            case(I):    // immediate op
                getParams(params, line, lineNumber, false, true, location);
                if(checkEnoughParams(lineNumber, line, params, instr->paramNum)){
                    param0 = params.top();
                    params.pop();
                    param1 = params.top();
                    params.pop();
                    param2 = params.top();
                    params.pop();
                    /*
                    if(symbolExists(param2)){
                        readSymbol(param2, true_location);
                    } else if(externSymbolExists(param2)){
                        readExternSymbol(param2, true_location);
                    } else {
                        true_location = constConv(param2, isNum);
                    }
                    */
                    true_location = constConv(param2, isNum);
                    if(param0[0]!='$' || param1[0]!='$' || !isNum){
                        addError(lineNumber, line, "First and second operands must be registers and third must be number");
                    }else{
                        param0.erase(param0.begin());
                        param1.erase(param1.begin());
                        machineCode = (instr->code<<26) + (constConv(param1, isNum)<<21) + (constConv(param0, isNum)<<16) + (true_location & 0xffff);
                    }
                }
                break;
            case(LUI):    // immediate op sp case
                getParams(params, line, lineNumber, false, true, location);
                if(checkEnoughParams(lineNumber, line, params, instr->paramNum)){
                    param0 = params.top();
                    params.pop();
                    param1 = params.top();
                    params.pop();
                    true_location = constConv(param1, isNum);
                    if(param0[0]!='$' || !isNum){
                        addError(lineNumber, line, "First operand must be register and second must be number");
                    }else{
                        param0.erase(param0.begin());
                        machineCode = (instr->code<<26) + (constConv(param0, isNum)<<16) + (true_location & 0xffff);
                    }
                }
                break;
            case(S):    // shift op
                getParams(params, line, lineNumber, false, true);
                if(checkEnoughParams(lineNumber, line, params, instr->paramNum)){
                    param0 = params.top();
                    params.pop();
                    param1 = params.top();
                    params.pop();
                    param2 = params.top();
                    params.pop();
                    // First two operands must be registers and third must not for this instruction
                    if(param0[0]!='$' || param1[0]!='$' || param2[0]=='$'){
                        addError(lineNumber, line, "First two operands must be registers and third must not for this instruction");
                    }else{
                        param0.erase(param0.begin());
                        param1.erase(param1.begin());
                        param2.erase(param2.begin());
                        machineCode = (instr->code<<26) + (constConv(param1, isNum)<<16) + (constConv(param0, isNum)<<11) + ((constConv(param2, isNum)&0x1f)<<6) + instr->funct;
                    }
                }
                break;
            case(SV):    // variable shift op
                getParams(params, line, lineNumber, false, true);
                if(checkEnoughParams(lineNumber, line, params, instr->paramNum)){
                    param0 = params.top();
                    params.pop();
                    param1 = params.top();
                    params.pop();
                    param2 = params.top();
                    params.pop();
                    // all operands for R instruction type must be registers
                    if(param0[0]!='$' || param1[0]!='$' || param2[0]!='$'){
                        addError(lineNumber, line, "All operands must be registers for this instruction");
                    }else{
                        param0.erase(param0.begin());
                        param1.erase(param1.begin());
                        param2.erase(param2.begin());
                        machineCode = (instr->code<<26) + (constConv(param2, isNum)<<21) + (constConv(param1, isNum)<<16) + (constConv(param0, isNum)<<11) + instr->funct;
                    }
                }
                break;
            case(B):     // branch op
                getParams(params, line, lineNumber, false, true);
                if(checkEnoughParams(lineNumber, line, params, instr->paramNum)){
                    param0 = params.top();
                    params.pop();
                    param1 = params.top();
                    params.pop();
                    param2 = params.top();
                    params.pop();

                    // First two operands must be registers
                    if(param0[0]!='$' || param1[0]!='$'){
                        addError(lineNumber, line, "First two operands must be registers.");
                    }else{
                        param0.erase(param0.begin());
                        param1.erase(param1.begin());

                        param_tmp = strtol(param2.c_str(), &dummy, 10);
                        if(symbolExists(param2) || externSymbolExists(param2) || dummy[0]==0){
                            if(dummy[0]==0){
                                branchTarget = param_tmp;
                            } else if (readSymbol(param2, branchTarget)) {
                                branchTarget = branchTarget/4 - (location + 1);
                            } else if (readExternSymbol(param2, branchTarget)) {
                                branchTarget = branchTarget/4 - (location + 1);
                                g_externSymbolTable.find(param2)->second.relList.push_back(RelInfo(location*4, R_MIPS_PC16));
                            }
                            machineCode = (instr->code<<26) + (constConv(param0, isNum)<<21) + (constConv(param1, isNum)<<16) + (branchTarget & 0xffff);
                        } else {
                            msg = (char*) malloc((param2.length()+19)*sizeof(char));
                            sprintf(msg, "Symbol not found: %s", param2.c_str());
                            addError(lineNumber, line, msg);
                            free(msg);
                        }
                    }
                }
                break;
            case(BZ):     // branch sp case op (zero)
                getParams(params, line, lineNumber, false, true);
                if(checkEnoughParams(lineNumber, line, params, instr->paramNum)){
                    param0 = params.top();
                    params.pop();
                    param1 = params.top();
                    params.pop();

                    // First operand must be register
                    if(param0[0]!='$'){
                        addError(lineNumber, line, "First operand must be register.");
                    }else{
                        param0.erase(param0.begin());

                        param_tmp = strtol(param1.c_str(), &dummy, 10);
                        if(symbolExists(param1) || externSymbolExists(param1) || dummy[0]==0){
                            if(dummy[0]==0){
                                branchTarget = param_tmp;
                            } else if (readSymbol(param1, branchTarget)) {
                                branchTarget = branchTarget/4 - (location + 1);
                            } else if (readExternSymbol(param1, branchTarget)) {
                                branchTarget = branchTarget/4 - (location + 1);
                                g_externSymbolTable.find(param1)->second.relList.push_back(RelInfo(location*4, R_MIPS_PC16));
                            }
                            machineCode = (instr->code<<26) + (constConv(param0, isNum)<<21) + (instr->funct<<16) + (branchTarget & 0xffff);
                        } else {
                            msg = (char*) malloc((param1.length()+19)*sizeof(char));
                            sprintf(msg, "Symbol not found: %s", param1.c_str());
                            addError(lineNumber, line, msg);
                            free(msg);
                        }
                    }
                }
                break;
            case(J):     // jump op
                getParams(params, line, lineNumber, false, true);
                if(checkEnoughParams(lineNumber, line, params, instr->paramNum)){
                    param0 = params.top().c_str();
                    params.pop();

                    if(readSymbol(param0, branchTarget)){
                        machineCode = (instr->code<<26) + ((branchTarget/4)&0x3ffffff);
                        g_symbolTable.find(param0)->second.relList.push_back(RelInfo(location*4, R_MIPS_26));
                    } else if (readExternSymbol(param0, branchTarget)) {
                        machineCode = (instr->code<<26) + ((branchTarget/4)&0x3ffffff);
                        g_externSymbolTable.find(param0)->second.relList.push_back(RelInfo(location*4, R_MIPS_26));
                    } else {
                        msg = (char*) malloc((param0.length()+19)*sizeof(char));
                        sprintf(msg, "Symbol not found: %s", param0.c_str());
                        addError(lineNumber, line, msg);
                        free(msg);
                    }
                }
                break;
            case(MD):    // multiple and division op
                getParams(params, line, lineNumber, false, true);
                if(checkEnoughParams(lineNumber, line, params, instr->paramNum)){
                    param0 = params.top().c_str();
                    params.pop();
                    param1 = params.top().c_str();
                    params.pop();
                    // all operands for R instruction type must be registers
                    if(param0[0]!='$' || param1[0]!='$'){
                        addError(lineNumber, line, "Both operands must be registers for this instruction");
                    }else{
                        param0.erase(param0.begin());
                        param1.erase(param1.begin());
                        machineCode = (instr->code<<26) + (constConv(param0, isNum)<<21) + (constConv(param1, isNum)<<16) + instr->funct;
                    }
                }
                break;
            case(SC):    // syscall
                getParams(params, line, lineNumber, true, true);
                if(checkEnoughParams(lineNumber, line, params, instr->paramNum)){
                    machineCode = (instr->code<<26) + instr->funct;
                }
                break;
            case(N):    // nop
                getParams(params, line, lineNumber, true, true);
                if(checkEnoughParams(lineNumber, line, params, instr->paramNum)){
                    machineCode = 0;
                }
                break;
            default:
                addError(lineNumber, line, "Command not recognized: "s + line);
                break;
        }

    } else {
        addError(lineNumber, line, "Command not recognized: "s + line);
    }

    return machineCode;
}

void init(const vector<OpCodeTablePair>& machineInstructions, const vector<OpCodeTablePair>& pseudoInstructions){

	g_opCodeTable.clear();

    // OT_MACHINE
    for (OpCodeTablePair it : machineInstructions){
        g_opCodeTable.insert(it);
    }

    // OT_PSEUDO
    for (OpCodeTablePair it : pseudoInstructions ){
        g_opCodeTable.insert(it);
    }

	g_errorList.clear();

	g_sourceList.clear();

	g_symbolTable.clear();

	g_globalSymbolTable.clear();

	g_externSymbolTable.clear();

	g_literalTable.clear();

	g_machineCodeList.clear();

	g_instLocationList.clear();

	g_instructionList.clear();

	g_sectionDataLocation = SECTION_DATA_AREA;
	g_sectionTextLocation = SECTION_TEXT_AREA;

    registersSet.clear();
    // REGISTERS SET ALIASES
    for (RegistersPair it : registerSet){
        registersSet.insert(it);
    }

    return;
}


OpType getInstr(const char* instrSymbol, const OpCode * &getInstr){
    OpCodeTable :: const_iterator tOpCodeIter;
    tOpCodeIter = g_opCodeTable.find(string(instrSymbol));
    if( tOpCodeIter != g_opCodeTable.end() ){
        getInstr = &tOpCodeIter->second;
        return tOpCodeIter->second.optype;
    } else {
        return OT_NONE;
    }
}

SourceList& getSourceList(){
    return g_sourceList;
}

void pushMachineCode(long machineCode){
    g_machineCodeList.push_back(machineCode);
}

void pushInstLocation(long instLocation){
	g_instLocationList.push_back(instLocation);
}

void pushInstruciton(std::string instruction) {
	char *line;
	if (instruction.empty() == false) {
		line = (char*)malloc((instruction.length() + 1) * sizeof(char));
		strcpy(line, instruction.c_str());
		g_instructionList.push_back(line);
	}
}

void pushSymbol(string symbol, long location, Section section){
	if(!symbol.empty()){
        g_symbolTable.insert(SymbolPair(symbol, Symbol(location, section)));
        if(globalSymbolExists(symbol)){
			updateGlobalSymbol(symbol, location);
        }
    }
}

bool readSymbol(string symbol, long &value){
    SymbolTable  :: const_iterator tableSymbolIter;
    tableSymbolIter = g_symbolTable.find(symbol);
    if( tableSymbolIter != g_symbolTable.end() ){
        value = tableSymbolIter->second.value;
        return true;
    } else {
        return false;
    }
}

bool symbolExists(string symbol){
    if( g_symbolTable.find(symbol) != g_symbolTable.end() ){
        return true;
    } else {
        return false;
    }
}

void pushGlobalSymbol(string symbol, long location){
	if(!symbol.empty()){
        g_globalSymbolTable.insert(SymbolPair(symbol, Symbol(location, S_NONE)));
    }
}

void updateGlobalSymbol(string symbol, long location){
    SymbolTable  :: iterator tableGlobalSymbolIter;
    tableGlobalSymbolIter = g_globalSymbolTable.find(symbol);
    if( tableGlobalSymbolIter != g_globalSymbolTable.end() ){
        tableGlobalSymbolIter->second.value = location;
    }
}

bool readGlobalSymbol(string symbol, long &val){
    SymbolTable  :: const_iterator tableGlobalSymbolIter;
    tableGlobalSymbolIter = g_globalSymbolTable.find(symbol);
    if( tableGlobalSymbolIter != g_globalSymbolTable.end() ){
        val = tableGlobalSymbolIter->second.value;
        return true;
    } else {
        return false;
    }
}

bool globalSymbolExists(string symbol){
	if( g_globalSymbolTable.find(symbol.c_str()) != g_globalSymbolTable.end() ){
        return true;
    } else {
        return false;
    }
}

void pushExternSymbol(string symbol, long location){
	if(!symbol.empty()){
        g_externSymbolTable.insert(ExternSymbolPair(symbol, ExternSymbol(location)));
    }
}

bool readExternSymbol(string symbol, long &val){
    ExternSymbolTable  :: const_iterator tableExternSymbolIter;
    tableExternSymbolIter = g_externSymbolTable.find(symbol);
    if( tableExternSymbolIter != g_externSymbolTable.end() ){
        val = tableExternSymbolIter->second.value;
        return true;
    } else {
        return false;
    }
}

bool externSymbolExists(string symbol){
    if( g_externSymbolTable.find(symbol) != g_externSymbolTable.end() ){
        return true;
    } else {
        return false;
    }
}

void pushLiteral(long location, long value){
    g_literalTable.insert(LiteralTablePair(location, value));
}

long sizeTableLiteral(){
    return g_literalTable.size();
}

bool checkEnoughParams(long lineNumber, string line, ParamStack params, int nExpected){

    bool ret = true;
    char msg[30];

    sprintf(msg, "Expected %d parameters", nExpected);

    if (nExpected != params.size()) {
        ret = false;
        addError(lineNumber, line, msg);
    }

    return ret;
}

bool checkEnoughParams(SourceLine sourceLine, ParamStack params, int nExpected) {
	return checkEnoughParams(sourceLine.lineNumber, sourceLine.sourceLine, params, nExpected);
}

void addError(long lineNumber, string line, string errorText){
    char *pos;
    char *errorMsg;
	pos = strrspn(line.c_str(), WHITESPACE);           // trim trailing whitespaces
    pos[0] = 0;
	errorMsg = (char*) malloc((strlen(line.c_str())+1+strlen(errorText.c_str())+1+30)*sizeof(char));
	sprintf(errorMsg, "Line %d: %s (%s)", lineNumber, errorText.c_str(), line.c_str());
    g_errorList.push_back(errorMsg);
}

bool errorsFound() {
    
    if (!g_errorList.empty()) {
        printf("Errors found:\n");
        while(!g_errorList.empty()) {
            printf("\n%s\n", g_errorList.front());
            g_errorList.pop_front();
        }
/*        
        printf("\n\nSymbols found:\n\n");
        while(!g_symbolTable.empty()) {
            printf("%s = %d\n", g_symbolTable.begin()->first, g_symbolTable.begin()->second);
            g_symbolTable.erase(g_symbolTable.begin());
        }
*/        
        return true; 
    }

    return false;
}

void writeDataToFile(std::string outFileNameBin, std::string outFileNameTxt){
    long  location;
    long  literalPos;
    long  literalVal;
    char* instr;
    long  machineCode;
    bool  isNum;
    long  iters;

    FILE *outFileBin;
    FILE *outFileTxt;

    LiteralTable::iterator tableLiteralIter;
    list<RelInfo>::iterator externSymbolRelListIter;
    list<RelInfo>::iterator symbolRelListIter;
    ExternSymbolTable::iterator tableExternSymbolIter;
    SymbolTable::iterator tableSymbolIter;
    MachineCodeList::iterator machineCodeListIter;
	InstLocationList::iterator instLocationListIter;
    InstructionList::iterator instructionListIter;

    if((outFileTxt = fopen(outFileNameTxt.c_str(), "wt"))==NULL){
        outFileOpenError(outFileNameTxt);
    }

    // TEXT SEGMENT

    location = SECTION_TEXT_AREA/4;

    if(outFileTxt){
        fprintf(outFileTxt, ".text\n");
    }else{
        printf("\n.text\n");
    }

    machineCodeListIter = g_machineCodeList.begin();
	instLocationListIter = g_instLocationList.begin();
    instructionListIter = g_instructionList.begin();
    iters = g_instructionList.size();

    for(long i=0; i<iters; i++){
        machineCode = *machineCodeListIter;
        instr = *instructionListIter;

        if(outFileTxt){
            fprintf(outFileTxt, "%08x --%08x %s\n", machineCode, (*instLocationListIter)*4, instr);
        }else{
            printf("%08x --%08x %s\n", machineCode, (*instLocationListIter)*4, instr);
        }
        location++;

        machineCodeListIter++;
		instLocationListIter++;
        instructionListIter++;
    }


    // DATA SEGMENT

    iters = sizeTableLiteral();

    if(outFileTxt){
        fprintf(outFileTxt, ".data\n");
    }else{
        printf(".data\n");
    }

    location = 0;

    machineCode = 0;

    tableLiteralIter = g_literalTable.begin();

    for(long i=0; i<iters; i++){
        literalPos = tableLiteralIter->first;
        literalVal = tableLiteralIter->second;
        if(literalPos != location){
            if(outFileTxt){
                fprintf(outFileTxt, "address %d\n", literalPos/4);
            }else{
                printf("address %d\n", literalPos/4);
            }
            location = literalPos + 4;
        } else {
            location += 4;
        }

        if(outFileTxt){
            fprintf(outFileTxt, "%08x --%08x\n", literalVal, literalPos);
        }else{
            printf("%08x --%08x\n", literalVal, literalPos);
        }

        tableLiteralIter++;
    }


    // GLOBAL'S SEGMENT

    iters = g_globalSymbolTable.size();

    if(outFileTxt){
        fprintf(outFileTxt, ".globl\n");
    }else{
        printf(".globl\n");
    }

    tableSymbolIter = g_globalSymbolTable.begin();

    for(long i=0; i<iters; i++){
        if(outFileTxt){
            fprintf(outFileTxt, "%08x --%s\n", tableSymbolIter->second.value, tableSymbolIter->first.c_str());
        }else{
            printf("%08x --%s\n", tableSymbolIter->second.value, tableSymbolIter->first.c_str());
        }

        tableSymbolIter++;
    }


    // RELOCATION SEGMENT
    if(outFileTxt){
        fprintf(outFileTxt, ".rel_data\n");
    }else{
        printf(".rel_data\n");
    }

    iters = g_symbolTable.size();

    tableSymbolIter = g_symbolTable.begin();

    for(long i=0; i<iters; i++){
        if(tableSymbolIter->second.relList.size()){
            symbolRelListIter = tableSymbolIter->second.relList.begin();
            if(outFileTxt){
                fprintf(outFileTxt, "%s", tableSymbolIter->first.c_str());
                for(long j=0; j<tableSymbolIter->second.relList.size(); j++){
                    fprintf(outFileTxt, " --%08x, ++%d", symbolRelListIter->byteUsed, symbolRelListIter->relType);
                    symbolRelListIter++;
                }
                fprintf(outFileTxt, "\n");
            }else{
                printf("%s", tableSymbolIter->first.c_str());
                for(long j=0; j<tableSymbolIter->second.relList.size(); j++){
                    printf(" --%08x, ++%d", symbolRelListIter->byteUsed, symbolRelListIter->relType);
                    symbolRelListIter++;
                }
                printf("\n");
            }
        }
        tableSymbolIter++;
    }

    iters = g_externSymbolTable.size();

    tableExternSymbolIter = g_externSymbolTable.begin();

    for(long i=0; i<iters; i++){
        externSymbolRelListIter = tableExternSymbolIter->second.relList.begin();
        if(outFileTxt){
            fprintf(outFileTxt, "%s", tableExternSymbolIter->first.c_str());
            for(long j=0; j<tableExternSymbolIter->second.relList.size(); j++){
                fprintf(outFileTxt, " --%08x, ++%d", externSymbolRelListIter->byteUsed, externSymbolRelListIter->relType);
                externSymbolRelListIter++;
            }
            fprintf(outFileTxt, "\n");
        }else{
            printf("%s", tableExternSymbolIter->first.c_str());
            for(long j=0; j<tableExternSymbolIter->second.relList.size(); j++){
                printf(" --%08x ++%d", externSymbolRelListIter->byteUsed, externSymbolRelListIter->relType);
                externSymbolRelListIter++;
            }
            printf("\n");
        }

        tableExternSymbolIter++;
    }

    // SYMBOLS

    iters = g_symbolTable.size();

    if(outFileTxt){
        fprintf(outFileTxt, "--symbols--\n");
    }else{
        printf("--symbols--\n");
    }

    tableSymbolIter = g_symbolTable.begin();

    for(long i=0; i<iters; i++){
        if(outFileTxt){
            fprintf(outFileTxt, "%d %08x %s\n", tableSymbolIter->second.section, tableSymbolIter->second.value, tableSymbolIter->first.c_str());
        }else{
            printf("%d %08x %s\n", tableSymbolIter->second.section, tableSymbolIter->second.value, tableSymbolIter->first.c_str());
        }
        tableSymbolIter++;
    }


    if(outFileTxt){
        printf("Text file \"%s\" created.\n", outFileNameTxt.c_str());
        
        // close output text file
        fclose(outFileTxt);
    }

	char tmpBuff[501];
	strcpy(tmpBuff, outFileNameBin.c_str());
    writeElf(tmpBuff);
}

void outFileOpenError(string filename){
	if(!filename.empty())
        printf("Error: Could not open output file (%s) for writing.\n", filename.c_str());
    else
        printf("Info: Output file name not defined.\n", filename.c_str());
}

//----------------------------------------------------------------------------

void err(int s, const char *f, ...){
    va_list args;

    printf("\n");

    va_start(args, f);
    vprintf(f, args);
    va_end(args);

    printf("\n");

    exit(s);
}

void errx(int s, const char *f, ...){
    va_list args;

    printf("\n");

    va_start(args, f);
    vprintf(f, args);
    va_end(args);

    printf("\n");

    exit(s);
}

int writeElf(string outFileName)
{
	int FileDes;
	Elf *pElf;
	Elf32_Ehdr *pEhdr;
	Elf32_Shdr *pShdr;
	Elf_Scn *pScn;
	Elf_Data *pData;

	// Create the ELF header
		if (elf_version(EV_CURRENT) == EV_NONE) // It must appear before "elf_begin()"
			errx(EX_SOFTWARE, "ELF library initialization failed: %s", elf_errmsg(-1));

		if ((FileDes = open(outFileName.c_str(), O_CREAT | O_WRONLY | O_TRUNC | O_BINARY, 0777)) < 0)
			errx(EX_OSERR, "open \"%s\" failed", outFileName.c_str());

		if ((pElf = elf_begin(FileDes, ELF_C_WRITE, NULL)) == NULL)  // 3rd argument is ignored for "ELF_C_WRITE"
			errx(EX_SOFTWARE, "elf_begin() failed: %s.", elf_errmsg(-1));

		if ((pEhdr = elf32_newehdr(pElf)) == NULL)
			errx(EX_SOFTWARE, "elf32_newehdr() failed: %s", elf_errmsg(-1));

		pEhdr->e_ident[EI_CLASS] = ELFCLASS32;  // Defined by Intel architecture
        pEhdr->e_ident[EI_DATA] = ELFDATA2MSB;  // Defined by Intel architecture
		pEhdr->e_machine = EM_MIPS;  // Intel architecture
		pEhdr->e_type = ET_REL;   // Relocatable file (object file)
		pEhdr->e_shstrndx = _shstrtab;    // Point to the shstrtab section

	// Create the section "default section header string table (.shstrtab)"
		if ((pScn = elf_newscn(pElf)) == NULL)
			errx(EX_SOFTWARE, "elf_newdata() failed: %s.", elf_errmsg(-1));
		if ((pData = elf_newdata(pScn)) == NULL)
			errx(EX_SOFTWARE, "elf_newdata() failed: %s.", elf_errmsg(-1));

		pData->d_align = 1;
		pData->d_buf = (void *) defaultStrTable;
		pData->d_type = ELF_T_BYTE;
		pData->d_size = defaultStrTableLen;

		if ((pShdr = elf32_getshdr(pScn)) == NULL)
			errx(EX_SOFTWARE, "elf32_etshdr() failed: %s.", elf_errmsg(-1));

		pShdr->sh_name = _shstrtab_offset;  // Point to the name of the section
		pShdr->sh_type = SHT_STRTAB;
		pShdr->sh_flags = 0;
	// End of section "String table"

	// Create the section ".strtab"
		if ((pScn = elf_newscn(pElf)) == NULL)
			errx(EX_SOFTWARE, "elf_newdata() failed: %s.", elf_errmsg(-1));
		if ((pData = elf_newdata(pScn)) == NULL)
			errx(EX_SOFTWARE, "elf_newdata() failed: %s.", elf_errmsg(-1));

    // gen. generic output file name
        char *genFileName, *extPos;
		const char* slashPos;
        genFileName = (char*) malloc((strlen(outFileName.c_str())+3)*sizeof(char));
        if(slashPos = strrchr(outFileName.c_str(), '/'))
            strcpy(genFileName, slashPos+1);
        else
            strcpy(genFileName, outFileName.c_str());
        if(extPos = strrchr(genFileName, '.'))
            strcpy(extPos, ".x");

    // calc length for strtab ('0' + genfilename + all symbols + all extern symbols)

        long strtabLen = 1 + strlen(genFileName) + 1;

        SymbolTable::iterator tableSymbolIter = g_symbolTable.begin();

        for(long i=0; i<g_symbolTable.size(); i++){
			strtabLen += (strlen(tableSymbolIter->first.c_str())+1);
            tableSymbolIter++;
        }

        ExternSymbolTable::iterator tableExternSymbolIter = g_externSymbolTable.begin();

        for(long i=0; i<g_externSymbolTable.size(); i++){
			strtabLen += (strlen(tableExternSymbolIter->first.c_str())+1);
            tableExternSymbolIter++;
        }

    // make strtab ('0' + genfilename + all symbols + all extern symbols)

        char *strtab;
        long strtabPos = 0;
        strtab = (char*) malloc(strtabLen*sizeof(char));

        strtab[strtabPos++] = 0;
        strcpy(strtab+strtabPos, genFileName);
        strtabPos += (strlen(genFileName) + 1);

    // make symtab along with strtab

        Elf32_Sym *x;

        x = (Elf32_Sym*) malloc (sizeof(Elf32_Sym)*(4+g_symbolTable.size()+g_externSymbolTable.size()));

        long xPos = 0;
        long xLocal = 0;
        long xExtern = 0;
		long xGlobal = 0;


	// Definition of the undefined section (this must be the first item by the definition of TIS ELF)
		x[  xPos].st_name = 0;
		x[  xPos].st_value = 0;
		x[  xPos].st_size = 0;
		x[  xPos].st_info = 0;
		x[  xPos].st_other = 0;
		x[xPos++].st_shndx = SHN_UNDEF;

	// Definition of the name of the source file (this must be the second item by the definition in TIS ELF)
		x[  xPos].st_name = 1;
		x[  xPos].st_value = 0;
		x[  xPos].st_size = 0;
		x[  xPos].st_info = ELF32_ST_INFO(STB_LOCAL, STT_FILE); // This is the value that st_info must have (because of TIS ELF)
		x[  xPos].st_other = 0;
		x[xPos++].st_shndx = SHN_ABS;  // The section where the symbol is

	// Definition of the ".text" section as a section in the ".symtab" section
		x[  xPos].st_name = 0;
		x[  xPos].st_value = 0;
		x[  xPos].st_size = 0;
		x[  xPos].st_info = ELF32_ST_INFO(STB_LOCAL, STT_SECTION);
		x[  xPos].st_other = 0;
		x[xPos++].st_shndx = _text;  // The section where the symbol is

	// Definition of the ".data" section as a section in the ".symtab" section
		x[  xPos].st_name = 0;
		x[  xPos].st_value = 0;
		x[  xPos].st_size = 0;
		x[  xPos].st_info = ELF32_ST_INFO(STB_LOCAL, STT_SECTION);
		x[  xPos].st_other = 0;
		x[xPos++].st_shndx = _data;  // The section where the symbol is

	// Definition of local symbols in .data and .text section
		xLocal = xPos;
		map<std::string, int> xLocalSymbolIndexMap;

        tableSymbolIter = g_symbolTable.begin();

        for(long i=0; i<g_symbolTable.size(); i++){
			if(!globalSymbolExists(tableSymbolIter->first)){
				strcpy(strtab+strtabPos, tableSymbolIter->first.c_str());
				xLocalSymbolIndexMap[tableSymbolIter->first] = xPos;
                //if(g_literalTable.find(tableSymbolIter->second) != g_literalTable.end() && g_literalTable.find(tableSymbolIter->second)->second.symbol!=NULL && !strcmp(g_literalTable.find(tableSymbolIter->second)->second.symbol,tableSymbolIter->first)){
                if(tableSymbolIter->second.section==S_DATA){
                    // .data
		            x[  xPos].st_name = strtabPos;  // Offset in the "strtab" section where the name start
                    x[  xPos].st_value = tableSymbolIter->second.value;
		            x[  xPos].st_size = 0;
		            x[  xPos].st_info = ELF32_ST_INFO(STB_LOCAL, STT_OBJECT);
		            x[  xPos].st_other = 0;
		            x[xPos++].st_shndx = _data;  // The section where the symbol is
                } else if(tableSymbolIter->second.section==S_TEXT){
                    // .text
		            x[  xPos].st_name = strtabPos;  // Offset in the "strtab" section where the name start
		            x[  xPos].st_value = tableSymbolIter->second.value;
		            x[  xPos].st_size = 0;
		            x[  xPos].st_info = ELF32_ST_INFO(STB_LOCAL, STT_FUNC);
		            x[  xPos].st_other = 0;
		            x[xPos++].st_shndx = _text;  // The section where the symbol is
                }

				strtabPos += (strlen(tableSymbolIter->first.c_str()) + 1);
            }
            tableSymbolIter++;
        }

		xGlobal = xPos;
		map<std::string, int> xGlobalSymbolIndexMap;

	// Definition of global symbols in .data and .text section
        tableSymbolIter = g_symbolTable.begin();

        for(long i=0; i<g_symbolTable.size(); i++){
			if(globalSymbolExists(tableSymbolIter->first)){
				strcpy(strtab+strtabPos, tableSymbolIter->first.c_str());
				xGlobalSymbolIndexMap[tableSymbolIter->first] = xPos;
                //if(g_literalTable.find(tableSymbolIter->second) != g_literalTable.end() && g_literalTable.find(tableSymbolIter->second)->second.symbol!=NULL && !strcmp(g_literalTable.find(tableSymbolIter->second)->second.symbol,tableSymbolIter->first)){
                if(tableSymbolIter->second.section==S_DATA){
                    // .data
		            x[  xPos].st_name = strtabPos;  // Offset in the "strtab" section where the name start
                    x[  xPos].st_value = tableSymbolIter->second.value;
		            x[  xPos].st_size = 0;
                    x[  xPos].st_info = ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT);
		            x[  xPos].st_other = 0;
		            x[xPos++].st_shndx = _data;  // The section where the symbol is
                } else if(tableSymbolIter->second.section==S_TEXT){
                    // .text
		            x[  xPos].st_name = strtabPos;  // Offset in the "strtab" section where the name start
		            x[  xPos].st_value = tableSymbolIter->second.value;
		            x[  xPos].st_size = 0;
		            x[  xPos].st_info = ELF32_ST_INFO(STB_GLOBAL, STT_FUNC);
		            x[  xPos].st_other = 0;
		            x[xPos++].st_shndx = _text;  // The section where the symbol is
                }

				strtabPos += (strlen(tableSymbolIter->first.c_str()) + 1);
            }
            tableSymbolIter++;
        }

	// Definition of extern symbols
        xExtern = xPos;     // position of first extern symbol in symtab

        tableExternSymbolIter = g_externSymbolTable.begin();

        for(long i=0; i<g_externSymbolTable.size(); i++){
			strcpy(strtab+strtabPos, tableExternSymbolIter->first.c_str());

	        x[  xPos].st_name = strtabPos;  // Offset in the "strtab" section where the name start
	        x[  xPos].st_value = 0;
	        x[  xPos].st_size = 0;
	        x[  xPos].st_info = ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE);
	        x[  xPos].st_other = 0;
	        x[xPos++].st_shndx = SHN_UNDEF;  // The section where the symbol is. Any external symbol has an undefined index because it reside outside the current object file

			strtabPos += (strlen(tableExternSymbolIter->first.c_str()) + 1);
            tableExternSymbolIter++;
        }


		pData->d_align = 1;
		pData->d_buf = (void *) &(strtab[0]);
		pData->d_type = ELF_T_BYTE;
		pData->d_size = strtabLen*sizeof(char);

		if ((pShdr = elf32_getshdr(pScn)) == NULL)
			errx(EX_SOFTWARE, "elf32_etshdr() failed: %s.", elf_errmsg(-1));

		pShdr->sh_name = _strtab_offset;
		pShdr->sh_type = SHT_STRTAB;
		pShdr->sh_flags = 0;
	// End of section ".strtab"

	// Create the section ".symtab"
		if ((pScn = elf_newscn(pElf)) == NULL)
			errx(EX_SOFTWARE, "elf_newdata() failed: %s.", elf_errmsg(-1));
		if ((pData = elf_newdata(pScn)) == NULL)
			errx(EX_SOFTWARE, "elf_newdata() failed: %s.", elf_errmsg(-1));

		pData->d_align = 4;
		pData->d_buf = (void *) x;
		pData->d_type = ELF_T_SYM;
		pData->d_size = sizeof(Elf32_Sym)*xPos;

		if ((pShdr = elf32_getshdr(pScn)) == NULL)
			errx(EX_SOFTWARE, "elf32_etshdr() failed: %s.", elf_errmsg(-1));

		pShdr->sh_name = _symtab_offset;  // Point to the name of the section
		pShdr->sh_type = SHT_SYMTAB;
		pShdr->sh_flags = 0;
		pShdr->sh_link = _strtab;  // point to the section .strtab (the section that contain the strings)
		pShdr->sh_info = ELF32_ST_INFO(STB_LOCAL, xLocal);  // the second argument is beause of TIS ELF (One greater than the symbol table index of the last local symbol (binding STB_LOCAL))
	// End of section ".symtab"

	// Create the section ".text"
		if ((pScn = elf_newscn(pElf)) == NULL)
			errx(EX_SOFTWARE, "elf_newdata() failed: %s.", elf_errmsg(-1));
		if ((pData = elf_newdata(pScn)) == NULL)
			errx(EX_SOFTWARE, "elf_newdata() failed: %s.", elf_errmsg(-1));

		pData->d_align = 4;
		pData->d_off = 0;
    
    // make assemblerinstructions buffer from g_machineCodeList
        uint32_t *assemblerinstructions;
        long assemblerinstructionsPos = 0;

        assemblerinstructions = (uint32_t*) malloc(g_machineCodeList.size()*sizeof(uint32_t));

        MachineCodeList::iterator machineCodeListIter = g_machineCodeList.begin();

        for(long i=0; i<g_machineCodeList.size(); i++){
            memcpy(assemblerinstructions+assemblerinstructionsPos, &(*machineCodeListIter), sizeof(uint32_t));
            assemblerinstructionsPos++;
            machineCodeListIter++;
        }

		pData->d_buf = (void *) assemblerinstructions;
		pData->d_type = ELF_T_WORD;
		pData->d_size = sizeof(uint32_t)*assemblerinstructionsPos;

		if ((pShdr = elf32_getshdr(pScn)) == NULL)
			errx(EX_SOFTWARE, "elf32_etshdr() failed: %s.", elf_errmsg(-1));

		pShdr->sh_name = _text_offset;
		pShdr->sh_type = SHT_PROGBITS;
		pShdr->sh_flags = SHF_ALLOC | SHF_EXECINSTR;
	// End of section ".text"

	// Create the section ".data"
		if ((pScn = elf_newscn(pElf)) == NULL)
			errx(EX_SOFTWARE, "elf_newdata() failed: %s.", elf_errmsg(-1));
		if ((pData = elf_newdata(pScn)) == NULL)
			errx(EX_SOFTWARE, "elf_newdata() failed: %s.", elf_errmsg(-1));

		pData->d_align = 4;

    // make datacontent buffer from g_literalTable

        uint32_t *datacontent;
        long datacontentPos = 0;

        datacontent = (uint32_t*) malloc(g_literalTable.size()*sizeof(uint32_t));

        LiteralTable::iterator tableLiteralIter = g_literalTable.begin();

        for(long i=0; i<g_literalTable.size(); i++){
            memcpy(datacontent+datacontentPos, &tableLiteralIter->second, sizeof(uint32_t));
            datacontentPos++;
            tableLiteralIter++;
        }

        pData->d_buf = (void *) datacontent;
		pData->d_type = ELF_T_WORD;
		pData->d_size = sizeof(uint32_t)*datacontentPos;
		pData->d_version = EV_CURRENT;

		if ((pShdr = elf32_getshdr(pScn)) == NULL)
			errx(EX_SOFTWARE, "elf32_etshdr() failed: %s.", elf_errmsg(-1));

		pShdr->sh_name = _data_offset;
		pShdr->sh_type = SHT_PROGBITS;
		pShdr->sh_flags = SHF_ALLOC | SHF_WRITE;
	// End of section ".data"

	// Create the section ".rel.text"
		if ((pScn = elf_newscn(pElf)) == NULL)
			errx(EX_SOFTWARE, "elf_newdata() failed: %s.", elf_errmsg(-1));
		if ((pData = elf_newdata(pScn)) == NULL)
			errx(EX_SOFTWARE, "elf_newdata() failed: %s.", elf_errmsg(-1));

		pData->d_align = 4;

    // make rel table from symbols in .data and .text section and extern symbols

        Elf32_Rel *rel;
        long relPos = 0;
        long relNum = 0;

    // count rel symbols and allocate memory

        tableSymbolIter = g_symbolTable.begin();

        for(long i=0; i<g_symbolTable.size(); i++){
            relNum += tableSymbolIter->second.relList.size();
            tableSymbolIter++;
        }

        tableExternSymbolIter = g_externSymbolTable.begin();

        for(long i=0; i<g_externSymbolTable.size(); i++){
            relNum += tableExternSymbolIter->second.relList.size();
            tableExternSymbolIter++;
        }

        rel = (Elf32_Rel*) malloc(relNum*sizeof(Elf32_Rel));

    // initialize rel table

        tableSymbolIter = g_symbolTable.begin();

        list<RelInfo>::iterator symbolRelListIter;

        for(long i=0; i<g_symbolTable.size(); i++){
            if (tableSymbolIter->second.relList.size() > 0) {
				if(!globalSymbolExists(tableSymbolIter->first)) {
					symbolRelListIter = tableSymbolIter->second.relList.begin();
                    for(long j=0; j<tableSymbolIter->second.relList.size(); j++){
                        rel[  relPos].r_offset = symbolRelListIter->byteUsed;
						rel[relPos++].r_info = ELF32_R_INFO(xLocalSymbolIndexMap[tableSymbolIter->first], symbolRelListIter->relType);
                        symbolRelListIter++;
                    }
				}
				else {
					symbolRelListIter = tableSymbolIter->second.relList.begin();
                    for(long j=0; j<tableSymbolIter->second.relList.size(); j++){
                        rel[  relPos].r_offset = symbolRelListIter->byteUsed;
						rel[relPos++].r_info = ELF32_R_INFO(xGlobalSymbolIndexMap[tableSymbolIter->first], symbolRelListIter->relType);
                        symbolRelListIter++;
                    }
				}
            }
            tableSymbolIter++;
        }

        tableExternSymbolIter = g_externSymbolTable.begin();

        list<RelInfo>::iterator externSymbolRelListIter;

        for(long i=0; i<g_externSymbolTable.size(); i++){
            externSymbolRelListIter = tableExternSymbolIter->second.relList.begin();
            for(long j=0; j<tableExternSymbolIter->second.relList.size(); j++){
                rel[  relPos].r_offset = externSymbolRelListIter->byteUsed;
                rel[relPos++].r_info = ELF32_R_INFO(xExtern, externSymbolRelListIter->relType);  // 3 is the index in the symbol table where the symbol is (.data section)
                externSymbolRelListIter++;
            }
            xExtern++;
            tableExternSymbolIter++;
        }


		pData->d_buf = (void *) rel;
        pData->d_type = ELF_T_REL; // The type of each relocation is specified in the relocation itself
		pData->d_size = sizeof(Elf32_Rel)*relNum;

		if ((pShdr = elf32_getshdr(pScn)) == NULL)
			errx(EX_SOFTWARE, "elf32_etshdr() failed: %s.", elf_errmsg(-1));

		pShdr->sh_name = _rel_text_offset;
		pShdr->sh_type = SHT_REL;
		pShdr->sh_flags = 0;
		pShdr->sh_link = _symtab;
		pShdr->sh_info = _text;
	// End of section ".rel.text"

	// Update the sections internally
		if (elf_update(pElf, ELF_C_NULL) < 0)
			errx(EX_SOFTWARE, "elf_update(NULL) failed: %s.", elf_errmsg(-1));

	// Write the object file
		if (elf_update(pElf, ELF_C_WRITE) < 0)
			errx(EX_SOFTWARE, "elf_update() failed: %s.", elf_errmsg(-1));

	// Close all handles
		elf_end(pElf);
		close(FileDes);

    // report file created
        printf("ELF file \"%s\" created.\n", outFileName.c_str());


	return 0;
}

bool changeSectionAndLocation(string sectionID, Section &section, long &location){
    char* msg;

	if(!strcmp(sectionID.c_str(), SECTION_DATA)){
        if(section == S_TEXT)
			g_sectionTextLocation = location;
        section = S_DATA;
        location = g_sectionDataLocation;
        return true;
	}else if(!strcmp(sectionID.c_str(), SECTION_TEXT)){
        if(section == S_DATA)
			g_sectionDataLocation = location;
        section = S_TEXT;
        location = g_sectionTextLocation;
        return true;
    }else{
		msg = (char*) malloc((strlen(sectionID.c_str())+40)*sizeof(char));
		sprintf(msg, "Section identificator not recognized: %s", sectionID.c_str());
        addError(0, "", msg);
        free(msg);
        return false;
    }
}



