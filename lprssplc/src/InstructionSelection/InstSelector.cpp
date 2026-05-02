#include "InstSelector.hpp"

#include <iostream>
#include <cassert>

#include "Operand.hpp"
#include "Ast.hpp"
#include "LprsInstructions.hpp"

using namespace std;

Operand* InstSelector::printAddr = new MemoryOperand(0b11111);
Operand* InstSelector::reg0 = new RegisterOperand(0);
Operand* InstSelector::reg1 = new RegisterOperand(1);
Operand* InstSelector::reg2 = new RegisterOperand(2);


std::vector<LprsInstruction*> InstSelector::selectInstructions(AstStm* root)
{
	m_instList.clear();
	stm(root);
	return m_instList;
}


void InstSelector::stm(AstStm* node)
{
	switch (node->getKind())
	{
	case StmKind::Assign:
	{
		AssignStm* x = static_cast<AssignStm*>(node);
		Operand* d = m_varTable.insert(x->id);
		Operand* o = exp(x->exp);
		m_instList.push_back(new LprsInstruction(LprsInstKind::Ld, new RegisterOperand(0), o));
		m_instList.push_back(new LprsInstruction(LprsInstKind::St, d, new RegisterOperand(0)));
		break;
	}
	case StmKind::Compound:
	{
		CompoundStm* x = static_cast<CompoundStm*>(node);
		stm(x->stm1);
		stm(x->stm2);
		break;
	}
	case StmKind::Print:
	{
		PrintStm* x = static_cast<PrintStm*>(node);
		expList(x->exps);
		break;
	}
	default:
	{
		assert(false);
	}
	}
}


Operand* InstSelector::exp(AstExp* node)
{
	switch (node->getKind())
	{
	case ExpKind::Id:
	{
		IdExp* x = static_cast<IdExp*>(node);
		Operand* var = m_varTable.find(x->name);
		return var;
	}
	case ExpKind::Num:
	{
		NumExp* x = static_cast<NumExp*>(node);
		Operand* d = new MemoryOperand();
		m_instList.push_back(new LprsInstruction(LprsInstKind::Sub, reg0, reg0, reg0));
		m_instList.push_back(new LprsInstruction(LprsInstKind::Lrl, reg0, new ImmOperand(x->value)));
		m_instList.push_back(new LprsInstruction(LprsInstKind::St, d, reg0));
		return d;
	}
	case ExpKind::Op:
	{
		OpExp* x = static_cast<OpExp*>(node);
		Operand* lo = exp(x->left);
		Operand* ro = exp(x->right);
		Operand* d = new MemoryOperand();
		m_instList.push_back(new LprsInstruction(LprsInstKind::Ld, reg1, lo));
		m_instList.push_back(new LprsInstruction(LprsInstKind::Ld, reg2, ro));
		if (x->operKind == OpKind::Plus)
		{
			m_instList.push_back(new LprsInstruction(LprsInstKind::Add, reg0, reg1, reg2));
		}
		else
		{
			m_instList.push_back(new LprsInstruction(LprsInstKind::Sub, reg0, reg1, reg2));
		}
		m_instList.push_back(new LprsInstruction(LprsInstKind::St, d, reg0));
		return d;
	}
	case ExpKind::Eseq:
	{
		EseqExp* x = static_cast<EseqExp*>(node);
		stm(x->stm);
		return exp(x->exp);
	}
	default:
	{
		assert(false);
	}
	}

	return nullptr;
}


void InstSelector::expList(AstExpList* node)
{
	switch (node->getKind())
	{
	case ExpListKind::Pair:
	{
		PaAstExpList* x = static_cast<PaAstExpList*>(node);
		Operand* o = exp(x->head);
		m_instList.push_back(new LprsInstruction(LprsInstKind::Ld, reg0, o));
		m_instList.push_back(new LprsInstruction(LprsInstKind::St, printAddr, reg0));
		expList(x->tail);
		break;
	}
	case ExpListKind::Last:
	{
		LastExpList* x = static_cast<LastExpList*>(node);
		Operand* o = exp(x->exp);
		m_instList.push_back(new LprsInstruction(LprsInstKind::Ld, reg0, o));
		m_instList.push_back(new LprsInstruction(LprsInstKind::St, printAddr, reg0));
		break;
	}
	default:
	{
		assert(false);
	}
	}
}
