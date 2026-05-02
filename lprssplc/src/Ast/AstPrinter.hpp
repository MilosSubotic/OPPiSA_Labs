#ifndef __AST_PRINTER__
#define __AST_PRINTER__

struct AstStm;
struct AstExp;
struct AstExpList;


struct AstPrinter
{
	void printAst(AstStm* root);

private:
	void printStm(AstStm* node);
	void printExp(AstExp* node);
	void printExpList(AstExpList* node);
};

#endif
