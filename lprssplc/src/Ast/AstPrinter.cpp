#include "AstPrinter.hpp"

#include <iostream>
#include <cassert>

#include "Ast.hpp"

using namespace std;


void AstPrinter::printAst(AstStm* root)
{
	printStm(root);
}


void AstPrinter::printStm(AstStm* node)
{
	switch (node->getKind())
	{
	case StmKind::Assign:
	{
		AssignStm* x = static_cast<AssignStm*>(node);
		cout << x->id << " : = ";
		printExp(x->exp);
		break;
	}
	case StmKind::Compound:
	{
		CompoundStm* x = static_cast<CompoundStm*>(node);
		printStm(x->stm1);
		cout << " ; ";
		printStm(x->stm2);
		break;
	}
	case StmKind::Print:
	{
		PrintStm* x = static_cast<PrintStm*>(node);
		cout << "print (";
		printExpList(x->exps);
		cout << ")";
		break;
	}
	default:
	{
		assert(false);
	}
	}
}


void AstPrinter::printExp(AstExp* node)
{
	switch (node->getKind())
	{
	case ExpKind::Id:
	{
		IdExp* x = static_cast<IdExp*>(node);
		cout << x->name;
		break;
	}
	case ExpKind::Num:
	{
		NumExp* x = static_cast<NumExp*>(node);
		cout << x->value;
		break;
	}
	case ExpKind::Op:
	{
		OpExp* x = static_cast<OpExp*>(node);
		printExp(x->left);
		if (x->operKind == OpKind::Plus)
			cout << " + ";
		else
			cout << " - ";
		printExp(x->right);
		break;
	}
	case ExpKind::Eseq:
	{
		EseqExp* x = static_cast<EseqExp*>(node);
		cout << "( ";
		printStm(x->stm);
		cout << " , ";
		printExp(x->exp);
		cout << " )";
		break;
	}
	default:
	{
		assert(false);
	}
	}
}


void AstPrinter::printExpList(AstExpList* node)
{
	switch (node->getKind())
	{
	case ExpListKind::Pair:
	{
		PaAstExpList* x = static_cast<PaAstExpList*>(node);
		printExp(x->head);
		cout << " , ";
		printExpList(x->tail);
		break;
	}
	case ExpListKind::Last:
	{
		LastExpList* x = static_cast<LastExpList*>(node);
		printExp(x->exp);
		break;
	}
	default:
	{
		assert(false);
	}
	}
}
