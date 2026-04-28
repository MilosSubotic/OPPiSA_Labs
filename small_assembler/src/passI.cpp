
#include "passI.h"
#include "assemblerLib.h"

using namespace std;

bool passI() {
	/******************************************************************************************
	* Look up for symbols and constants in the source code and remember their location.
	*******************************************************************************************/
	SourceList& sourceList = getSourceList();

	Section section = S_TEXT;
	long location = 0; // Start postition at 0
	for (SourceLine sourceLine : sourceList) {
		ParamStack params;
		getParams(params, sourceLine);

		Directive directive = getDirective(sourceLine);
		if (directive == D_ORG) {
			if (checkEnoughParams(sourceLine, params, 1)) {
				changeSectionAndLocation(params.top(), section, location);
			}
		}

		string symbol;
		if (getSymbol(symbol, sourceLine)) {
			if (symbolExists(symbol)) {
				string msg = "Symbol already exist: " + symbol;
				addError(sourceLine, msg);
			}
			pushSymbol(symbol, location, section);
		}

		string opSym;
		if (getExecutable(opSym, sourceLine)) {
			location += 4; // Increment location counter if executable line
		} else {
			if (directive == D_WORD) {
				while (!params.empty()) {
					pushLiteral(location, constConv(params.top()));
					params.pop();
					location += 4;
				}
			} else if (directive == D_SPACE) {
				checkEnoughParams(sourceLine, params, 1);
				location += constConv(params.top()) + (4 - constConv(params.top()) % 4) % 4;
			} else if (directive == D_GLOBL) {
				if (checkEnoughParams(sourceLine, params, 1)) {
					if (globalSymbolExists(params.top())) {
						string msg = "Global symbol already exist: " + string(params.top());
						addError(sourceLine, msg);
					}
					pushGlobalSymbol(params.top(), location);
				}
			} else if (directive == D_EXTERN) {
				if (checkEnoughParams(sourceLine, params, 1)) {
					if (externSymbolExists(params.top())) {
						string msg = "Extern symbol already exist: " + string(params.top());
						addError(sourceLine, msg);
					}
					pushExternSymbol(params.top(), location);
				}
			}
		}
	}

	return !errorsFound();
}
