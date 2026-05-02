#include "LivenessAnalysis.h"
#include "Util.h"


void buildCfg(Instructions& instructions)
{
	Instruction* targetInstr = NULL;
	// branches not supported
	auto prevIt = instructions.begin();
	for (auto it = instructions.begin(); it != instructions.end(); it ++)
	{
		auto itNext = ++it;
		it--;
		if (it == instructions.begin() && itNext == instructions.end())
		{
			break;
		}
		else if (it == instructions.begin() && itNext != instructions.end())
		{
			if ((*it)->getType() == I_B || (*it)->getType() == I_BLTZ)
			{
				targetInstr = getInstructionByPosition((*it)->getTargetLabel()->getPosition(), instructions);
				(*it)->getSucc().push_back(targetInstr);
				targetInstr->getPred().push_back((*it));
			}
			(*it)->getSucc().push_back(*itNext);
		}
		else if (it != instructions.begin() && itNext != instructions.end())
		{
			if ((*it)->getType() == I_B || (*it)->getType() == I_BLTZ)
			{
				targetInstr = getInstructionByPosition((*it)->getTargetLabel()->getPosition(), instructions);
				(*it)->getSucc().push_back(targetInstr);
				targetInstr->getPred().push_back((*it));
			}
			(*it)->getPred().push_back(*prevIt);
			(*it)->getSucc().push_back(*itNext);
		}
		else
		{
			(*it)->getPred().push_back(*prevIt);
		}
		prevIt = it;
	}
}


void setDefsAndUses(Instructions& instructions)
{
	for (auto it = instructions.begin(); it != instructions.end(); it ++)
	{
		for (auto dstIt = (*it)->getDst().begin(); dstIt != (*it)->getDst().end(); dstIt++)
		{
			if ((*dstIt)->getType() == Variable::REG_VAR)
				(*it)->getDef().push_back(*dstIt);
		}
		for (auto srcIt = (*it)->getSrc().begin(); srcIt != (*it)->getSrc().end(); srcIt++)
		{
			if ((*srcIt)->getType() == Variable::REG_VAR)
				(*it)->getUse().push_back(*srcIt);
		}
	}
}


void livenessAnalysis(Instructions& instructions)
{
	bool doItAgain = true;
	while (doItAgain)
	{
		doItAgain = false;

		Instructions::reverse_iterator rit;
		for (rit = instructions.rbegin(); rit != instructions.rend(); rit++)
		{
			Variables& out = (*rit)->getOut();
			Variables& in = (*rit)->getIn();
			Variables newOut;
			Variables newIn;

			// 1.
			// newOut = U succIn;
			Instructions& successors = (*rit)->getSucc();
			Instructions::iterator succIt;
			for (succIt = successors.begin(); succIt != successors.end(); succIt++)
			{
				Variables& succIn = (*succIt)->getIn();
				newOut.insert(newOut.end(), succIn.begin(), succIn.end());
			}
			newOut.sort();
			newOut.unique();

			// 2.
			// newIn = use U (out / def)
			Variables& use = (*rit)->getUse();
			Variables& def = (*rit)->getDef();

			Variables outMinusDef;
			Variables::iterator varIt;
			for (varIt = out.begin(); varIt != out.end(); varIt++)
			{
				if (!variableExists(*varIt, def))
				{
					outMinusDef.push_back(*varIt);
				}
			}

			newIn = use;
			newIn.insert(newIn.end(), outMinusDef.begin(), outMinusDef.end());
			newIn.sort();
			newIn.unique();

			if (newIn != in || newOut != out)
			{
				doItAgain = true;
			}

			in = newIn;
			out = newOut;
		}
	}
}
