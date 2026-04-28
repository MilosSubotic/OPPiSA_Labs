
#include "passII.h"
#include "assemblerLib.h"

using namespace std;

bool passII(){

    long          lineNum;
    string         sourceLine;
    long          location;
    string         instr;
    long          opCode;
    SourceList::iterator sourceListIter;
  	 Section section = S_TEXT;

	SourceList& sourceList = getSourceList();

    // reset section counters
    section = S_TEXT;
    location = SECTION_DATA_AREA;
    changeSectionAndLocation(SECTION_DATA, section, location);
    section = S_DATA;
    location = SECTION_TEXT_AREA;
    changeSectionAndLocation(SECTION_TEXT, section, location);

    for (sourceListIter = sourceList.begin(); sourceListIter != sourceList.end(); ++sourceListIter){
        sourceLine = sourceListIter->sourceLine;
        lineNum = sourceListIter->lineNumber;

		Directive directive = getDirective(*sourceListIter);
		if (directive == D_ORG)
      {
			ParamStack params;
			getParams(params, *sourceListIter);
			changeSectionAndLocation(params.top(), section, location);
      }
      else if (directive == D_SPACE)
		{
			ParamStack params;
			getParams(params, *sourceListIter);
			location += constConv(params.top()) + (4 - constConv(params.top()) % 4) % 4;
		}
		else if (directive == D_WORD)
		{
			ParamStack params;
			getParams(params, *sourceListIter);
			location += params.size() * 4;
		}
		else if(getExecutable(instr, *sourceListIter, true)){
			opCode = makeMachineCode(instr.c_str(), lineNum, location/4);
            pushMachineCode(opCode);
			pushInstruciton(instr);
			pushInstLocation(location/4);
            location+=4;
        }
    }

    return !errorsFound();
}
