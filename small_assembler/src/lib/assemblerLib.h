#ifndef __ASSEMBLERLIB_H__
#define __ASSEMBLERLIB_H__

#include <string>
#include <map>
#include <list>
#include <stack>
#include <vector>

#define COMMENT       "#;"
#define LABEL         ":"
#define WHITESPACE    " \t\r\n"
#define DIRECTIVE     '.'
#define OPSEP         "\t ,"
#define DIRECTIVE_SEP "\t ,"
#define COMMAND_SEP   " \t\r\n"
#define PSEUDO_SEP    '\n'

/**
 * @brief TEXT area section start address
 */
#define SECTION_TEXT_AREA 0x0
/**
 * @brief DATA area section start address
 */
#define SECTION_DATA_AREA 0x0

/**
 * @brief DATA area indentificator
 */
#define SECTION_DATA "data"
/**
 * @brief TEXT area indentificator
 */
#define SECTION_TEXT "text"

/**
 * @enum Section
 * 
 * @brief available sections type
 */
enum Section {
    S_DATA,
    S_TEXT,
    S_NONE
};

/**
 * @enum Directive
 * 
 * @brief assembler directives
 */
enum Directive {
    D_NONE,
    D_EXTERN,
    D_GLOBL,
    D_ORG,
    D_SPACE,
    D_WORD
};

/**
 * @enum OpType
 * 
 * @brief operation codes type
 */
enum OpType {
    OT_MACHINE,
    OT_PSEUDO,
    OT_MACRO,
    OT_NONE
};

/**
 * @enum InstFormat
 *
 * @brief instruction formats type
 */
enum InstFormat {
    R,
    I,
    J,
    B,
    BZ,
    JR,
    LUI,
    MD,
    MF,
    N,
    S,
    SC,
    SV,
    I_NONE
};

/**
 * @struct OpCode
 *
 * @brief structure for operation code table element
 */
struct OpCode {
    char*      symbol;   /**< operation code symbol in source code */
    OpType     optype;   /**< operation type @see OpType           */
    InstFormat iformat;  /**< operation format @see InstFormat     */
    char       code;     /**< operation code                       */
    char       funct;    /**< operation function                   */
    char       paramNum; /**< number of parameters required        */
    
    /**
     * OpCode constructor
     * initialize each element of the structure
     */
    OpCode(char* sSymbol, OpType oOptype, InstFormat iIformat, char cCode, char fFunct, char pParamNum) :
        symbol(sSymbol),
        optype(oOptype),
        iformat(iIformat),
        code(cCode),
        funct(fFunct),
        paramNum(pParamNum)
		{}
};

/**
 * @struct RelInfo
 *
 * @brief Relocation info
 */
struct RelInfo {
    long byteUsed;  /**< bytes in text segment where the symbol is used */
    char relType;   /**< relocation type                                */

    /**
     * RelInfo constructor
     * initialize each element of the structure
     */
    RelInfo(long bByteUsed, char rRelType) : byteUsed(bByteUsed), relType(rRelType) { }
};

/**
 * @struct Symbol
 *
 * @brief structure for symbol table element
 */
struct Symbol {
    long value; /**< symbol's value */
    Section section; /**< section where the symbol belongs */
    std::list<RelInfo> relList;   /**< relocation info list */

    /**
     * Symbol constructor
     * initialize each element of the structure
     */
    Symbol(long vValue, Section sSection) : value(vValue), section(sSection) {}
};

/**
 * @struct ExternSymbol
 *
 * @brief structure for extern symbol table element
 */
struct ExternSymbol {
    long value; /**< symbol's value */
    std::list<RelInfo> relList; /**< relocation info list */

    /**
     * ExternSymbol constructor
     * initialize each element of the structure
     */
    ExternSymbol(long vValue) : value(vValue) {}
};

/**
 * @struct SourceLine
 *
 * @brief structure for source list element (source line and source line number)
 */
struct SourceLine {
	long lineNumber;
	std::string sourceLine;
};


/**
 * registers map type
 */
typedef std::map<std::string, std::string> Registers;

/**
 * registers pair
 */
typedef std::pair<std::string, std::string> RegistersPair;

/**
 * map type for operation codes table
 */
typedef std::map<std::string, OpCode> OpCodeTable;

/**
 * operation codes table pair
 */
typedef std::pair<std::string, OpCode> OpCodeTablePair;

/**
 * param stack type
 */
typedef std::stack<std::string> ParamStack;

/**
 * source list type
 */
typedef std::list<SourceLine> SourceList;



/**
 * @brief   Converts constant from int or hex string to integer value
 *
 * @param[in]   constant string that should be converted to value
 * @param[out]   isNum    true if string is hex or dec number; false otherwise
 *
 * @return  converted value of constant
 */
long constConv(std::string constant, bool &isNum);

/**
 * @brief   Converts constant from int or hex string to integer value
 *
 * @param[in]   constant string that should be converted to value
 *
 * @return  converted value of constant
 */
long constConv(std::string constant);

/**
 * @brief Gets executable part of the line
 *
 * @param[out]  executable     will hold executable part of the line, if exacutable is found
 * @param[in]   sourceLine      source line to examine
 * @param[in]   addErrorToList if true - check if symbolic instruction
 *                         exists in operation codes table and adds
 *                         error if it doesn't exist
 *                         (case of unrecognized instruction)
 *
 * @return  true if an executable line;
 *          false if not an executable line
 */
bool getExecutable(std::string &executable, SourceLine sourceLine, bool addErrorToList=false);

/**
 * @brief   Returns the part of passed line that represents the command
 *
 * @param[out]  cmd   pointer to pointer to buffer with returned command if found
 * @param[in]   line  the current line to examine
 *		
 * @return  true if command found; false otherwise
 *
 */
bool getCommand(std::string &cmd, std::string line);

/**
 * @brief   Returns found directive;
 *          Adds an error to error list if directive is found, but not recognized
 *
 * @param[in]   sourceLine        the current line to examine
 *
 * @return  D_NONE      - when no directive is found
 *          D_EXTERN    - when .extern is found
 *          D_GLOBL     - when .globl is found
 *          D_ORG       - when .org is found
 *          D_SPACE     - when .space is found
 *          D_WORD      - when .word is found
 */
Directive getDirective(SourceLine sourceLine);

/**
 * @brief   Returns the part of current line that represents symbol, if found
 *
 * @param[out]  symbol        will contain the symbol if one is found in the line
 * @param[in]   sourceLine    the source line to examine
 *		
 * @return  true if symbol found; false otherwise
 */
bool getSymbol(std::string &symbol, SourceLine sourceLine);

/**
 * @brief   Extracts all parameters from current line (after command).
 *
 * @param[out]  params      ParamStack valiable that will hold parameters if they are found
 * @param[in]   sourceLine        the current line to examine
 *
 * @return  true if param found; false otherwise
 */
bool getParams(ParamStack &params, SourceLine sourceLine);

/**
 * @brief   Converts an assembler instruction to machine code.
 *
 * @param[in]   line        the current line to examine
 * @param[in]   lineNumber  the current line number
 * @param[in]   location    the current program location
 *
 * @return 32-bit machine code
 */
unsigned long makeMachineCode(std::string line, long lineNumber, long location);

/**
 * @brief Initialises all members of lib (lists, maps...)
 *
 * @param[in]   machineInstructions     array of machine instructions
 * @param[in]   pseudoInstructions      array of pseudo instructions              
 *
 */
void init(const std::vector<OpCodeTablePair>& machineInstructions, const std::vector<OpCodeTablePair>& pseudoInstructions);

/**
 * @brief   Gets reference to the list with source lines
 *
 * @return  reference to the list.
 */
SourceList& getSourceList();

/**
 * @brief   Adds operation code to list
 *
 * @param[in]   machineCode operation code to add
 */
void pushMachineCode(long machineCode);

/**
 * @brief   Adds instruction location to list
 *
 * @param[in]   instLocation location to add
 */
void pushInstLocation(long instLocation);

/**
 * @brief   Adds instruction to instruction list
 *
 * @param[in]   instruction text of instruction to add
 */
void pushInstruciton(std::string instruction);

/**
 * @brief   Adds a symbol to symbol table
 *
 * @param[in]   symbol      symbol to add (used as key)
 * @param[in]   value       symbol's value
 * @param[in]   section     section in witch the symbol is defined
 */
void pushSymbol(std::string symbol, long value, Section section);

/**
 * @brief   Reads symbol and its postion from symbol table
 *
 * @param[in]   symbol  symbol to read, used as key
 * @param[out]  value   where symbol value will be stored
 *
 * @return  true if symbol found; false otherwise
 */
bool readSymbol(std::string symbol, long &value);

/**
 * @brief   Verifies if symbol already exists in symbol table
 *
 * @param[in]   symbol  symbol to verify, used as key
 *
 * @return  true if symbol found; false otherwise
 */
bool symbolExists(std::string symbol);

/**
 * @brief   Adds a symbol to global symbol table
 *
 * @param[in]   symbol   symbol to add (used as key)
 * @param[in]   value    symbol's value
 */
void pushGlobalSymbol(std::string symbol, long value);

/**
 * @brief   Updates symbol's value in global symbol table
 *
 * @param[in]   symbol   symbol to update (used as key)
 * @param[in]   value    new symbol's value
 */
void updateGlobalSymbol(std::string symbol, long value);

/**
 * @brief   Reads symbol and its postion from global symbol table
 *
 * @param[in]   symbol  symbol to read, used as key
 * @param[out]  value   where the symbol value will be stored
 *
 * @return  true if symbol found; false otherwise
 */
bool readGlobalSymbol(std::string symbol, long &value);

/**
 * @brief   Verifies if symbol already exists in global symbol table
 *
 * @param[in]   symbol  symbol to verify, used as key
 *
 * @returns true if symbol found; false otherwise
 */
bool globalSymbolExists(std::string symbol);

/**
 * @brief   Adds a symbol to extern symbol table
 *
 * @param[in]   symbol   symbol to add (used as key)
 * @param[in]   value    symbol's value
 */
void pushExternSymbol(std::string symbol, long value);

/**
 * @brief   Reads symbol value from extern symbol table
 *
 * @param[in]   symbol  symbol to read, used as key
 * @param[out]  value   where the symbol value will be stored
 *
 * @return  true if symbol found; false otherwise
 */
bool readExternSymbol(std::string symbol, long &value);

/**
 * @brief   Verifies if symbol already exists in extern symbol table
 *
 * @param[in]   symbol  symbol to verify, used as key
 *
 * @return  true if symbol found; false otherwise
 */
bool externSymbolExists(std::string symbol);

/**
 * @brief   Adds a value to location in literal table
 *
 * @param[in]   location    data location (used as a key)
 * @param[in]   value       data value
 */
void pushLiteral(long location, long value);

/**
 * @brief   Adds error to error list if actual number of parameters differs from
 *          expected.
 *
 * @param[in]   sourceLine  sourceLine to check
 * @param[in]   params      parameter stack
 * @param[in]   nExpected   expected number of parameters
 *
 * @return  true if right number; false otherwise
 */
bool checkEnoughParams(SourceLine sourceLine, ParamStack params, int nExpected);

/**
 * @brief   Adds errors to errors list
 *
 * @param[in]   sourceLine  current source line
 * @param[in]   errorText   error text
 */
void addError(SourceLine sourceLine, std::string errorText);

/**
 * @brief   If errors were found, print them and exit.
 *
 * @return  true if errors found; false otherwise
 */
bool errorsFound();

/**
 * @brief   Writes binary data to binary file if defined
 *          Writes text data to textual file if defined, to stream output otherwise
 *
 * @param[in]   outFileNameBin  binary file name
 * @param[in]   outFileNameTxt  textual file name
 */
void writeDataToFile(std::string outFileNameBin, std::string outFileNameTxt);

/**
 * @brief   Updates current section and location
 *
 * @param[in]   sectionID   new section identificator (string)
 *                          valid identificators - S (extracted from ".org S" directive):
 *                              text
 *                              data
 * @param[out]   section     identificator of the previously active section
 * @param[out]   location    counter of memory locations for the new section
 *
 * @return  true  - if section identificator is recognized
 *          false - otherwise
 */
bool changeSectionAndLocation(std::string sectionId, Section &section, long &location);

#endif  // __ASSEMBLERLIB_H__
