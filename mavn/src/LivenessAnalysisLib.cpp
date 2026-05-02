#include "Commons.h"
#include "LivenessAnalysisLib.h"
#include "LivenessAnalysis.h"
#include "Util.h"

#include <iostream>

using namespace std;


void doLivenessAnalysis(int mode)
{
	Instructions& instructions = getInstructions();

	buildCfg(instructions);

	setDefsAndUses(instructions);

	if (mode == __DUMPS__)
	{
		cout << endl;
		cout << "==========================================" << endl;
		cout << "Instruction list before LIVENESS ANALYSIS:" << endl;
		cout << "==========================================" << endl;
		printInstructions(instructions);
		cout << "==========================================" << endl;
	}

	livenessAnalysis(instructions);

	if (mode == __DUMPS__)
	{
		cout << endl;
		cout << "==========================================" << endl;
		cout << "Instruction list after LIVENESS ANALYSIS:" << endl;
		cout << "==========================================" << endl;
		printInstructions(instructions);
		cout << "==========================================" << endl;
	}
}
