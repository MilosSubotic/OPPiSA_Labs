#include "ResourceAllocationLib.h"
#include "Constants.h"
#include "Commons.h"
#include "Simplification.h"
#include "ResourceAllocation.h"

#include <exception>

using namespace std;

bool doResourceAllocation()
{
	Instructions& instructions = getInstructions();
	InterferenceGraph& ig = buildInterferenceGraph(instructions);
	
	printInterferenceMatrix();
	
	SimplificationStack& ss = doSimplification(ig, __REG_NUMBER__);
	
	if (ss.size() != getRegVariables().size())
		throw runtime_error("Simplification of interference graph is impossible!");

	if (!doAllocation(ss, ig))
	{
		throw runtime_error("Allocation impossible!");
	}
	else
	{
		if (checkResourceAllocation(ig))
		{
			return true;
		}
		else
		{
			throw runtime_error("Allocation not valid!");
		}
	}
}


bool checkResourceAllocation(InterferenceGraph& ig)
{
	for (auto iter1 = ig.getIgVariables().begin(); iter1 != ig.getIgVariables().end(); iter1++)
	{
		for (auto iter2 = ig.getIgVariables().begin(); iter2 != ig.getIgVariables().end(); iter2++)
		{
			if ((*iter1) != (*iter2))
			{
				RegisterVariable* var1 = (RegisterVariable*)(*iter1);
				RegisterVariable* var2 = (RegisterVariable*)(*iter2);
				if (var1->getAssignment() == var2->getAssignment())
				{
					if (ig.getMatrix()[var1->getPosition()][var2->getPosition()] == __INTERFERENCE__)
					{
						return false;
					}
					else
					{
						// nothing
					}
				}
				else
				{
					// nothing
				}
			}
		}
	}
	return true;
}
