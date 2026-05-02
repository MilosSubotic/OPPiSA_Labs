#include "Parser.hpp"

#include <string>

#include "Error.hpp"

using namespace std;


AstStm* Parser::parse()
{
	m_currentToken = m_lexer.getNextToken();
	return stm();
}


AstStm* Parser::stm()
{
	AstStm* s = stmPrim();
	switch (m_currentToken.getKind())
	{
	case TokenKind::Semicolumn:
	{
		eat(TokenKind::Semicolumn);
		return makeCompoundStm(s, stm());
	}
	case TokenKind::Eof:
	case TokenKind::Comma:
	{
		return s;
	}
	default:
	{
		error("Parsing error");
		return nullptr;
	}
	}
}


AstStm* Parser::stmPrim()
{
	switch (m_currentToken.getKind())
	{
	case TokenKind::Id:
	{
		string id = m_currentToken.getStringValue();
		eat(TokenKind::Id);
		eat(TokenKind::Assign);
		return makeAssignStm(id, exp());
	}

	case TokenKind::Print:
	{
		eat(TokenKind::Print);
		eat(TokenKind::Lparen);
		AstExpList* AstExpList = expList();
		eat(TokenKind::Rparen);
		return makePrintStm(AstExpList);
	}
	default:
	{
		error("Parsing error");
		return nullptr;
	}
	}
}


AstExp* Parser::exp()
{
	switch (m_currentToken.getKind())
	{
	case TokenKind::Id:
	{
		AstExp* e = makeIdExp(m_currentToken.getStringValue());
		eat(TokenKind::Id);
		return expPrim(e);
	}

	case TokenKind::Int:
	{
		AstExp* e = makeNumExp(m_currentToken.getIntValue());
		eat(TokenKind::Int);
		return expPrim(e);
	}

	case TokenKind::Lparen:
	{
		eat(TokenKind::Lparen);
		AstStm* s = stm();
		eat(TokenKind::Comma);
		AstExp* e = exp();
		eat(TokenKind::Rparen);
		return makeEseqExp(s, e);
	}

	default:
		error("Parsing error");
		return nullptr;
		break;
	}
}


AstExp* Parser::expPrim(AstExp* e)
{
	switch (m_currentToken.getKind())
	{
	case TokenKind::Plus:
	{
		eat(TokenKind::Plus);
		AstExp* re = exp();
		return makeOpExp(e, OpKind::Plus, re);
	}
	case TokenKind::Minus:
	{
		eat(TokenKind::Minus);
		AstExp* re = exp();
		return makeOpExp(e, OpKind::Minus, re);
	}
	default:
	{
		return e;
	}
	}
}


AstExpList* Parser::expList()
{
	AstExp* irExp = exp();
	switch (m_currentToken.getKind())
	{
	case TokenKind::Comma:
	{
		eat(TokenKind::Comma);
		AstExpList* AstExpList = expList();
		return makePaAstExpList(irExp, AstExpList);
	}

	default:
		return makeLastExpList(irExp);
	}
}


void Parser::eat(TokenKind kind)
{
	if (m_currentToken.getKind() == kind)
		m_currentToken = m_lexer.getNextToken();
	else
		error("Parsing error");
}
