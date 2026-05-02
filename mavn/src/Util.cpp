#include "Util.h"

using namespace std;


void printVariable(Variable* variable)
{
	switch (variable->getType())
	{
	case Variable::MEM_VAR:
		{
			MemoryVariable* m = (MemoryVariable*)variable;
			cout << "|memvar|Name: " << m->getName() << ", Val: " << m->getValue();
			break;
		}
	case Variable::REG_VAR:
		{
			RegisterVariable* r = (RegisterVariable*)variable;
			cout << "|regvar|Name: " << r->getName() << ", Pos: " << r->getPosition() << ", Asgn: " << r->getAssignment(); 
			break;
		}
	case Variable::CONST_VAR:
		{
			ConstVariable* c = (ConstVariable*)variable;
			cout << "|cstvar|Val: " << c->getValue();
			break;
		}
	default: break;
	}
}


void printVariableName(Variable* variable)
{
	//cout << variable->getName();
}


void printVariables(Variables& variables)
{
	Variables::iterator iter;

	for (iter = variables.begin(); iter != variables.end(); iter++)
	{
		//printVariable(*iter);
		cout << endl;
	}
}


void printVariablesNames(Variables& variables)
{
	Variables::iterator iter;

	for (iter = variables.begin(); iter != variables.end(); iter++)
	{
		//cout << (*iter)->name << " ";
	}
	cout << endl;
}


bool variableExists(Variable* variable, Variables& variables)
{
	bool ret = false;

	Variables::iterator it;

	for (it = variables.begin(); it != variables.end(); it++)
	{
		if (*it == variable)
		{
			ret = true;
			break;
		}
	}

	return ret;
}


Variable* variableExists(string varName, Variables& variables)
{
	Variables::iterator it;

	for (it = variables.begin(); it != variables.end(); it++)
	{
		if ((*it)->getName() == varName)
		{
			return *it;
		}
	}

	throw runtime_error("Variable: " + varName + " not defined!");
}


Label* labelExists(string name, Labels& labels)
{
	Labels::iterator it;

	for (it = labels.begin(); it != labels.end(); it++)
	{
		if ((*it)->getName() == name)
		{
			return *it;
		}
	}

	throw runtime_error("Label: " + name + " not defined!");
}


Instruction* getInstructionByPosition(int pos, Instructions& instrs)
{
	Instructions::iterator iter;

	for (iter = instrs.begin(); iter != instrs.end(); iter++)
	{
		if ((*iter)->getPosition() == pos) 
			return *iter;
	}
	
	throw runtime_error("Cannot find instruction at this position: " + pos);
}


void printInstructionsPos(Instructions& instrs)
{
	Instructions::iterator iter;

	for (iter = instrs.begin(); iter != instrs.end(); iter++)
	{
		cout << (*iter)->getPosition() << " ";
	}
	cout << endl;
}


string instructionTypeToString(InstructionType t)
{
	switch(t)
	{
	case I_ADD:		return "add";
	case I_ADDI:	return "addi";	
	case I_SUB:		return "sub";
	case I_LA:		return "la";
	case I_LI:		return "li";
	case I_LW:		return "lw";
	case I_SW:		return "sw";
	case I_BLTZ:	return "bltz";	
	case I_B:		return "b";
	case I_NOP:		return "nop";
	default:		return "unknown";
	}
}


string registerToString(Regs r)
{
	switch(r)
	{
	case t0:	return "$t0";
	case t1:	return "$t1";
	case t2:	return "$t2";
	case t3:	return "$t3";
	case t4:	return "$t4";
	default:	return "unknown";
	}
}


void printInstruction(Instruction* instr)
{
	cout << "Type:\t" << instructionTypeToString(instr->getType());
	cout << "\tPos:\t" << instr->getPosition();
	cout << endl;
	cout << "------------------------------------------" << endl;

	int i = 1;
	for (auto it = instr->getDst().begin(); it != instr->getDst().end(); it++)
	{
		cout << "Dest" << i++ << ":\t";
		printVariable(*it);
		cout << endl;
	}
	
	i = 1;
	for (auto it = instr->getSrc().begin(); it != instr->getSrc().end(); it++)
	{
		cout << "Src" << i++ << ":\t";
		printVariable(*it);
		cout << endl;
	}
	cout << "------------------------------------------" << endl;

	cout << "PRED:\t";
	printInstructionsPos(instr->getPred());

	cout << "SUCC:\t";
	printInstructionsPos(instr->getSucc());

	cout << "USE:\t";
	printVariablesNames(instr->getUse());
	
	cout << "DEF:\t";
	printVariablesNames(instr->getDef());
	
	cout << "IN:\t";
	printVariablesNames(instr->getIn());

	cout << "OUT:\t";
	printVariablesNames(instr->getOut());
}


void printInstructions(Instructions& instrs)
{
	Instructions::iterator iter;

	for (iter = instrs.begin(); iter != instrs.end(); iter++)
	{
		if (iter != instrs.begin())
			cout << "==========================================" << endl;
		printInstruction(*iter);
		if (iter != --instrs.end())
			cout << "==========================================" << endl;
	}
}
