#include "Emitter.h"
#include "Commons.h"
#include "Util.h"
#include <fstream>

using namespace std;

void emitDataSegment(ofstream& outFile)
{
	outFile << ".data " << endl;
	for (auto it = getMemVariables().begin(); it != getMemVariables().end(); it++)
	{
		MemoryVariable* memVar = (MemoryVariable*)*it;
		outFile << memVar->getName() << ":\t.word " << memVar->getValue() << endl;
	}
}

void emitTextSegment(ofstream& outFile)
{
	outFile << ".text " << endl;
	outFile << getFunctionName() << ":" << endl;

	for (auto it = getInstructions().begin(); it != getInstructions().end(); it++)
	{
		Instruction* instr = *it;
		int instrPos = instr->getPosition();
		for (auto labIt = getLabels().begin(); labIt != getLabels().end(); labIt++)
		{
			if ((*labIt)->getPosition() == instrPos)
			{
				// print labels first
				outFile << (*labIt)->getName() << ":\n";
			}
		}
		outFile << "\t\t" << instructionTypeToString(instr->getType()) << "\t";

		if (instr->getType() == I_BLTZ)
		{
			for (auto srcIt = instr->getSrc().begin(); srcIt != instr->getSrc().end(); srcIt++)
			{
				if ((*srcIt)->getType() == Variable::MEM_VAR)
				{
					outFile << (*srcIt)->getName() << " ";
				}
				else if ((*srcIt)->getType() == Variable::CONST_VAR)
				{
					ConstVariable* c = (ConstVariable*)*srcIt;
					outFile << c->getValue() << " ";
				}
				else
				{
					RegisterVariable* r = (RegisterVariable*)*srcIt;
					outFile << registerToString(r->getAssignment()) << " ";
				}
			}
			outFile << instr->getTargetLabel()->getName();
		}
		else if (instr->getType() == I_LW)
		{
			for (auto dstIt = instr->getDst().begin(); dstIt != instr->getDst().end(); dstIt++)
			{
				if ((*dstIt)->getType() == Variable::MEM_VAR)
				{
					outFile << (*dstIt)->getName() << " ";
				}
				else if ((*dstIt)->getType() == Variable::CONST_VAR)
				{
					ConstVariable* c = (ConstVariable*)*dstIt;
					outFile << c->getValue() << " ";
				}
				else
				{
					RegisterVariable* r = (RegisterVariable*)*dstIt;
					outFile << registerToString(r->getAssignment()) << " ";
				}
			}
			ConstVariable* src1 = (ConstVariable*)instr->getSrc().front();
			RegisterVariable* src2 = (RegisterVariable*)instr->getSrc().back();
			
			outFile << src1->getValue() << "(";
			outFile << registerToString(src2->getAssignment()) << ")";
		}
		else if (instr->getType() == I_SW)
		{
			RegisterVariable* src1 = (RegisterVariable*)instr->getSrc().front();
			ConstVariable* src2 = (ConstVariable*)(*(++instr->getSrc().begin()));
			RegisterVariable* src3 = (RegisterVariable*)instr->getSrc().back();

			outFile << registerToString(src1->getAssignment()) << " ";
			outFile << src2->getValue() << "(";
			outFile << registerToString(src3->getAssignment()) << ")";
		}
		else
		{
			for (auto dstIt = instr->getDst().begin(); dstIt != instr->getDst().end(); dstIt++)
			{
				if ((*dstIt)->getType() == Variable::MEM_VAR)
				{
					outFile << (*dstIt)->getName() << " ";
				}
				else if ((*dstIt)->getType() == Variable::CONST_VAR)
				{
					ConstVariable* c = (ConstVariable*)*dstIt;
					outFile << c->getValue() << " ";
				}
				else
				{
					RegisterVariable* r = (RegisterVariable*)*dstIt;
					outFile << registerToString(r->getAssignment()) << " ";
				}
			}

			for (auto srcIt = instr->getSrc().begin(); srcIt != instr->getSrc().end(); srcIt++)
			{
				if ((*srcIt)->getType() == Variable::MEM_VAR)
				{
					outFile << (*srcIt)->getName() << " ";
				}
				else if ((*srcIt)->getType() == Variable::CONST_VAR)
				{
					ConstVariable* c = (ConstVariable*)*srcIt;
					outFile << c->getValue() << " ";
				}
				else
				{
					RegisterVariable* r = (RegisterVariable*)*srcIt;
					outFile << registerToString(r->getAssignment()) << " ";
				}
			}
		}
		outFile << endl;
	}
}

void generateAsmFile(std::string fileName)
{
	ofstream outFile;
	int pos = fileName.find_last_of('.');
	fileName = fileName.substr(0, pos) + ".s";
	
	outFile.open(fileName);

	if (!outFile)
		throw runtime_error("Failed to create output file!");

	outFile << ".globl " << getFunctionName() << endl;

	emitDataSegment(outFile);

	outFile << endl;

	emitTextSegment(outFile);

	outFile << endl;
}
