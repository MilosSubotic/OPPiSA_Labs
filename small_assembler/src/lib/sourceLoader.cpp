#include "sourceLoader.h"

#include "assemblerLib.h"

#include <cstring>

using namespace std;


/**
* @brief   Gets instruction (full machineCode) from instruction symbol
*
* @param[in]   instrSymbol symbol of instruction to find
* @param[out]  getInstr    opcode of instruction if found
*
* @return  type of intruction; OT_NONE if instruction not found
*/
OpType getInstr(const char* instrSymbol, const OpCode* &getInstr);

void inFileOpenError(string filename) {
	if (!filename.empty())
		printf("Error: Could not open input file (%s) for reading.\n", filename.c_str());
	else
		printf("Error: Input file name not defined.\n");
}

void sourceListPushBack(SourceList& sourceList, const char* sourceLine, long lineNumber) {
	if (sourceLine) {
		sourceList.push_back(SourceLine{ lineNumber,std::string(sourceLine) });
	}
}

/**
* @brief Replaces given string (find) in string (source) with new string (replace).
*
* @param[out] dest       destination for processed string
* @param[in] source     string being processed
* @param[in] find       string to replace
* @param[in] replace    string to replace with
* @param[in] replaceAll true  - replace all occurences of find
*                   false - replace only first occ. of find
*
* @return true if replace successful (stringFind found and replaced);
*         false otherwise
*/
bool replaceString(char* &dest, const char* source, const char* find, const char* replace, bool replaceAll) {
	const char* pos;
	bool ret = false;
	char* sourceCopy;

	if (!(dest && source && find && replace))
		return false;

	sourceCopy = (char*)malloc((strlen(source) + 1) * sizeof(char));
	strcpy(sourceCopy, source);

	do {
		if (pos = strstr(sourceCopy, find)) {
			memcpy(dest, sourceCopy, strlen(sourceCopy) - strlen(pos));
			memcpy(dest + strlen(sourceCopy) - strlen(pos), replace, strlen(replace));
			memcpy(dest + strlen(sourceCopy) - strlen(pos) + strlen(replace), pos + strlen(find), strlen(pos) + 1 - strlen(find));
			ret = true;
			free(sourceCopy);
			sourceCopy = (char*)malloc((strlen(dest) + 1) * sizeof(char));
			strcpy(sourceCopy, dest);
		}
	} while (pos && replaceAll);

	free(sourceCopy);

	return ret;
}


bool loadSourceRepPseudo(string input_file){

    FILE*         inFile;
    char*         buffer;
    long          buffLen;
    long          lineNum;
    char*         mem1;
    char*         op;
	string         op1;
    string         symbol;
    const OpCode* replace;
    ParamStack   params;
    char*         mem2;
    long          buffPos;
    char*         replaceLine;
    char*         sourceLine;
    char*         pos = NULL;
    int           ret;
    /******************************************************************************************
    * read source code to buffer
    *******************************************************************************************/

	SourceList& sourceList = getSourceList();

    // open input file
	if(!(inFile = fopen(input_file.c_str(), "rb"))){
        inFileOpenError(input_file);
        return false;
    }

    // get input file size
    fseek(inFile, 0, SEEK_END);
    buffLen = ftell(inFile);
    fseek(inFile, 0, SEEK_SET);
    
    // read input file to buffer
    buffer = (char *) malloc((buffLen+1) * sizeof(char));
    fread(buffer, sizeof(char), buffLen, inFile);
    
    buffer[buffLen] = 0;

    // close input file
    fclose(inFile);

    /******************************************************************************************
    * read lines to buffer, replace pseudo instructions,
    * store lines to source list, and line numbers to line
    * numbers list
    *******************************************************************************************/

    mem2 = (char*) malloc(buffLen*sizeof(char));
    buffPos = 0;

    mem1 = (char*) malloc(buffLen*sizeof(char));

    lineNum = 1;

    while(buffPos < buffLen){
        ret = sscanf(buffer+buffPos,"%[^\r\n]", mem2, buffLen);
        if(ret==1){ // skip empty lines
            if(!(getCommand(op1,mem2))){
                op = mem1;
                // Try to find directive...
	            strcpy(op, mem2);
                op[strcspn(op, COMMENT)] = 0;       // Parse dir with comments
                if(pos = strpbrk(op, LABEL)){
                    op = pos+1;                        // Parse dir with labels
                }
                op += strspn(op, WHITESPACE);       // Skip dirs with nothing on... (eat whitespaces)
                if(strlen(op) && op[0]==DIRECTIVE){ // Skip dirs with nothing on... ommit .
                    op++;
                }
                op[strcspn(op, DIRECTIVE_SEP)] = 0;// throw everything after cmd or first op
                op1 = op;
		    }

			if(getInstr(op1.c_str(), replace)==OT_PSEUDO){

				getParams(params, SourceLine{ lineNum, std::string(mem2) });

				checkEnoughParams(SourceLine{ lineNum, std::string(mem2) }, params, replace->paramNum);

                replaceLine = (char*) malloc((strlen(mem2)+strlen(replace->symbol)+2)*sizeof(char));
                strcpy(replaceLine, replace->symbol);

                sourceLine = NULL;

                if(!params.empty()){
                    sourceLine  = (char*) malloc((strlen(replace->symbol)+1)*sizeof(char));
                    strcpy(sourceLine, replace->symbol);
					replaceString(replaceLine, sourceLine, "%1", params.top().c_str(), true);
                    params.pop();
                    free(sourceLine);
                }

                if(!params.empty()){
                    sourceLine  = (char*) malloc((strlen(replaceLine)+1)*sizeof(char));
                    strcpy(sourceLine, replaceLine);
                    replaceString(replaceLine, sourceLine, "%2", params.top().c_str(), true);
                    params.pop();
                    free(sourceLine);
                }

                if(!params.empty()){
                    sourceLine  = (char*) malloc((strlen(replaceLine)+1)*sizeof(char));
                    strcpy(sourceLine, replaceLine);
                    replaceString(replaceLine, sourceLine, "%3", params.top().c_str(), true);
                    params.pop();
                    free(sourceLine);
                }

                sourceLine  = (char*) malloc((strlen(replaceLine)+1)*sizeof(char));
                strcpy(sourceLine, replaceLine);
                replaceString(replaceLine, sourceLine, "$SECTION_TEXT", SECTION_TEXT, true);
                free(sourceLine);

                sourceLine  = (char*) malloc((strlen(replaceLine)+1)*sizeof(char));
                strcpy(sourceLine, replaceLine);
                replaceString(replaceLine, sourceLine, "$SECTION_DATA", SECTION_DATA, true);
                free(sourceLine);

				if (getSymbol(symbol, SourceLine{ lineNum, std::string(mem2) })) {
					sourceListPushBack(sourceList, symbol.c_str(), lineNum);
                }

                // add every replace line to sourceList and lineNumber to lineNumberList
                sourceLine = replaceLine;
                do{
                    pos=strchr(sourceLine, PSEUDO_SEP);
                    if(pos)
                        pos[0] = 0;
                    sourceListPushBack(sourceList, sourceLine, lineNum);
                    sourceLine = pos+1;
                }while(pos);

                if(replaceLine)
                    free(replaceLine);

            } else {
                sourceListPushBack(sourceList, mem2, lineNum);
            }
        } else {
            mem2[0] = 0;
        }
        buffPos += strlen(mem2)+2;
        lineNum++;
    }

    //release taken memory resources
    free(mem1);
    free(mem2);
    free(buffer);

    return !errorsFound();
}
