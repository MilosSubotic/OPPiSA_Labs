#ifndef __PARSER__
#define __PARSER__

#include "Lexer.hpp"
#include "Ast.hpp"


struct Parser
{
	Parser(InputStream& inStream) : m_lexer(inStream) {}

	AstStm* parse();

private:
	AstStm* stm();
	AstStm* stmPrim();
	AstExp* exp();
	AstExp* expPrim(AstExp* e);
	AstExpList* expList();

	void eat(TokenKind kind);

	Lexer m_lexer;
	Token m_currentToken;
};

#endif
