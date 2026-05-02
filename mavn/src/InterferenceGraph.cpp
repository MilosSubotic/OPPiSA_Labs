#include "InterferenceGraph.h"

#include <iostream>
#include <vector>

using namespace std;

static InterferenceGraph ig;

InterferenceGraph& buildInterferenceGraph(Instructions& instructions)
{
	int varNum = ig.getIgVariables().size();

	// init interference matrix
	ig.getMatrix().resize(varNum);
	for (auto it = ig.getMatrix().begin(); it != ig.getMatrix().end(); it++)
		(*it).resize(varNum);

	// build interference matrix
	for (auto it = instructions.begin(); it != instructions.end(); it++)
	{
		Instruction* instruction = (*it);
		for(auto defVarIt = instruction->getDef().begin(); defVarIt != instruction->getDef().end(); defVarIt++)
		{
			int defVarIndex = ((RegisterVariable*)(*defVarIt))->getPosition();
			for(auto outVarIt = instruction->getOut().begin(); outVarIt != instruction->getOut().end(); outVarIt++)
			{
				int outVarIndex = ((RegisterVariable*)(*outVarIt))->getPosition();
				if ((*defVarIt)->getName() != (*outVarIt)->getName())
				{
					ig.getMatrix()[defVarIndex][outVarIndex] = __INTERFERENCE__;
					ig.getMatrix()[outVarIndex][defVarIndex] = __INTERFERENCE__;
				}
			}
		}
	}

	return ig;
}


void printInterferenceMatrix()
{
	if (ig.getMatrix().size() == 0)
	{
		cout << "Interference graph is empty!";
		return;
	}
	
	cout << "==========================================" << endl;
	cout << "Interference matrix:" << endl;
	cout << "==========================================" << endl;

	// print existing variables in order to mark rows and columns
	for (auto varIt = ig.getIgVariables().begin(); varIt != ig.getIgVariables().end(); varIt++)
	{
		cout << "\t" << (*varIt)->getName();
	}
	cout << endl;

	auto varIt = ig.getIgVariables().begin();
	for (auto it1 = ig.getMatrix().begin(); it1 != ig.getMatrix().end(); it1++)
	{
		cout << (*varIt++)->getName();
		for (auto it2 = (*it1).begin(); it2 != (*it1).end(); it2++)
		{
			cout << "\t" << *it2;
		}
		cout << endl;
	}
	cout << "==========================================" << endl;
}
