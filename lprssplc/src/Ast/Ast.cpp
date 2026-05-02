#include "Ast.hpp"


AstStm* makeCompoundStm(AstStm* x, AstStm* y) {
	return new CompoundStm(x, y);
}


AstStm* makeAssignStm(std::string x, AstExp* y) {
	return new AssignStm(x, y);
}


AstStm* makePrintStm(AstExpList* x) {
	return new PrintStm(x);
}


AstExp* makeIdExp(const std::string& x) {
	return new IdExp(x);
}


AstExp* makeNumExp(int x) {
	return new NumExp(x);
}


AstExp* makeOpExp(AstExp* x, OpKind op, AstExp* y) {
	return new OpExp(x, op, y);
}


AstExp* makeEseqExp(AstStm* x, AstExp* y) {
	return new EseqExp(x, y);
}


AstExpList* makePaAstExpList(AstExp* x, AstExpList* y) {
	return new PaAstExpList(x, y);
}


AstExpList* makeLastExpList(AstExp* x) {
	return new LastExpList(x);
}
