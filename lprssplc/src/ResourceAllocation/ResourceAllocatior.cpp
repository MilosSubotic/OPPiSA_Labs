#include "ResourceAllocator.hpp"

#include "Operand.hpp"
#include "LprsInstructions.hpp"

using namespace std;


void ResourceAllocator::allocateResources(vector<LprsInstruction*>& instList)
{
	vector<Operand*> operands;
	for (auto it : instList)
	{
		if (it->getDest() != nullptr) operands.push_back(it->getDest());
		if (it->getSrc1() != nullptr) operands.push_back(it->getSrc1());
		if (it->getSrc2() != nullptr) operands.push_back(it->getSrc2());
	}

	int addr = 0;
	for (auto it : operands)
	{
		if (it->getKind() == OperandKind::Memory)
		{
			MemoryOperand* x = static_cast<MemoryOperand*>(it);
			if (x->address == -1)
			{
				x->address = addr;
				addr += 1;
			}
		}
	}
}
