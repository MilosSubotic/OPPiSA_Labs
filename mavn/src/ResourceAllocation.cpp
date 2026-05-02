#include "ResourceAllocation.h"


bool doAllocation(SimplificationStack& ss, InterferenceGraph& ig)
{
	std::list<Regs> regList;
	Variables coloredVars;

	Regs reg;
	RegisterVariable* v;

	while (!ss.empty())
	{
		regList.push_back(t0);
		regList.push_back(t1);
		regList.push_back(t2);
		regList.push_back(t3);
		regList.push_back(t4);

		v = (RegisterVariable*)ss.top();
		ss.pop();

		// check interference with other variables
		if (!coloredVars.empty())
		{
			for (auto iter = coloredVars.begin(); iter != coloredVars.end(); iter++)
			{
				RegisterVariable* coloredVar = (RegisterVariable*)*iter;
				if (ig.getMatrix()[v->getPosition()][coloredVar->getPosition()] == __INTERFERENCE__)
					regList.remove(coloredVar->getAssignment()); // remove used register
			}
		}
		coloredVars.push_back(v);

		if (regList.empty())
		{
			return false; // all regs allocated
		}
		else
		{
			reg = regList.front();
			regList.clear();
			v->setAssignment(reg); // assign reg
		}

	}

	coloredVars.clear();
	return true;
}
