#ifndef __INST_SELECTOR__
#define __INST_SELECTOR__

#include <vector>

#include "VariableTable.hpp"

struct AstStm;
struct AstExp;
struct AstExpList;

struct Operand;
struct LprsInstruction;


struct InstSelector
{
	std::vector<LprsInstruction*> selectInstructions(AstStm* root);

private:
	void stm(AstStm* node);
	Operand* exp(AstExp* node);
	void expList(AstExpList* node);

	std::vector<LprsInstruction*> m_instList;

	static Operand* printAddr;
	static Operand* reg0;
	static Operand* reg1;
	static Operand* reg2;

	VariableTable m_varTable;
};

#endif
