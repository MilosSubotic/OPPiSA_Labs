#ifndef __AST__
#define __AST__


#include <string>


struct AstExpList;
struct AstExp;
struct AstStm;


////////////////
// Statements //
////////////////
enum class StmKind {
	Compound,
	Assign,
	Print,
};


struct AstStm {
	AstStm(StmKind x) : kind(x) {}
	StmKind getKind() { return kind; }
private:
	StmKind kind;
};


struct CompoundStm : AstStm {
	CompoundStm(AstStm* x, AstStm* y) : AstStm(StmKind::Compound), stm1(x), stm2(y) {}
	AstStm* stm1;
	AstStm* stm2;
};


struct AssignStm : AstStm {
	AssignStm(std::string x, AstExp* y) : AstStm(StmKind::Assign), id(x), exp(y) {}
	std::string id;
	AstExp* exp;
};


struct PrintStm : AstStm {
	PrintStm(AstExpList* x) : AstStm(StmKind::Print), exps(x) {}
	AstExpList* exps;
};


AstStm* makeCompoundStm(AstStm* x, AstStm* y);


AstStm* makeAssignStm(std::string x, AstExp* y);


AstStm* makePrintStm(AstExpList* x);


/////////////////
// Expressions //
/////////////////
enum class ExpKind {
	Id,
	Num,
	Op,
	Eseq,
};


struct AstExp {
	AstExp(ExpKind x) : kind(x) {}
	ExpKind getKind() { return kind; }
private:
	ExpKind kind;
};


enum class OpKind {
	Plus,
	Minus,
	Mul,
	Div,
};


struct IdExp : AstExp {
	IdExp(const std::string& x) : AstExp(ExpKind::Id), name(x) {}
	std::string name;
};


struct NumExp : AstExp {
	NumExp(int x) : AstExp(ExpKind::Num), value(x) {}
	int value;
};


struct OpExp : AstExp {
	OpExp(AstExp* x, OpKind op, AstExp* y) : AstExp(ExpKind::Op), left(x), operKind(op), right(y) {}
	OpKind operKind;
	AstExp* left;
	AstExp* right;
};


struct EseqExp : AstExp {
	EseqExp(AstStm* x, AstExp* y) : AstExp(ExpKind::Eseq), stm(x), exp(y) {}
	AstStm* stm;
	AstExp* exp;
};


AstExp* makeIdExp(const std::string& x);


AstExp* makeNumExp(int x);


AstExp* makeOpExp(AstExp* x, OpKind op, AstExp* y);


AstExp* makeEseqExp(AstStm* x, AstExp* y);


//////////////////////
// Expressions list //
//////////////////////
enum class ExpListKind {
	Pair,
	Last,
};

struct AstExpList {
	AstExpList(ExpListKind x) : kind(x) {}
	ExpListKind getKind() { return kind; }
private:
	ExpListKind kind;
};


struct PaAstExpList : AstExpList {
	PaAstExpList(AstExp* x, AstExpList* y) : AstExpList(ExpListKind::Pair), head(x), tail(y) {}
	AstExp* head;
	AstExpList* tail;
};


struct LastExpList : AstExpList {
	LastExpList(AstExp* x) : AstExpList(ExpListKind::Last), exp(x) {}
	AstExp* exp;
};


AstExpList* makePaAstExpList(AstExp* x, AstExpList* y);


AstExpList* makeLastExpList(AstExp* x);


#endif
