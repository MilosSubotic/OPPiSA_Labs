#pragma once

#include "LexicalAnalysis.h"
#include "Token.h"
#include "Util.h"
#include "Commons.h"

class SyntaxAnalysis
{

private:
	
	/**
	 * Reference to lexical analysis module
	 */
	LexicalAnalysis& lexicalAnalysis;

	/**
	 * Iterator to the token list which represents the output of the lexical analysis
	 */
	TokenList::iterator tokenIterator;

	/**
	 * Current token that is being analyzed
	 */
	Token currentToken;

	/**
	 * Nonterminal Q
	 */
	void q();

	/**
	 * Nonterminal S
	 */
	void s();

	/**
	 * Nonterminal L
	 */
	void l();

	/**
	 * Nonterminal E
	 */
	Instruction* e();

	Instruction* checkAndCreateInstruction();

	/**
	 * Syntax error indicator
	 */
	bool errorFound;

	int m_instrPosition;

	int m_regVarPosition;

public:

	/**
	 * Constructor
	 */
	SyntaxAnalysis(LexicalAnalysis& lex);

	/**
	 * Method which performs syntax analysis
	 */
	bool Do();

	/**
	 * Prints the token that caused the syntax error
	 */
	void printSyntaxError(Token& token);

	/**
	 * Eats the current token if its type is "t"
	 * otherwise reports syntax error
	 *
	 * param[in] - t - the expected token type
	 */
	void eat(TokenType t);

	/**
	 * Returns the next token from the token list
	 */
	Token getNextToken();

	Instructions& m_instrs;

	Variables& m_regVars;

	Variables& m_memVars;

	Labels& m_labels;
};

