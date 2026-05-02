#ifndef __LEXER__
#define __LEXER__

#include <string>
#include <vector>
#include <map>

#include "InputStream.hpp"


enum class TokenKind
{
	None,
	Int,
	Id,
	Comma,
	Semicolumn,
	Lparen,
	Rparen,
	Assign,
	Print,
	Plus,
	Minus,
	Eof,
};


struct Token
{
public:
	Token() = default;
	Token(TokenKind kind) : m_kind(kind) {}
	Token(TokenKind kind, const std::string& stringValue) : m_kind(kind), m_stringValue(stringValue) {}
	Token(TokenKind kind, int intValue) : m_kind(kind), m_intValue(intValue) {}

	TokenKind getKind() const { return m_kind; }
	void setKind(TokenKind kind) { m_kind = kind; }

	std::string getStringValue() const { return m_stringValue; }
	void setStringValue(std::string value) { m_stringValue = value; }

	int getIntValue() const { return m_intValue; }
	void setIntValue(int value) { m_intValue = value; }

protected:
	TokenKind m_kind = TokenKind::None;
	std::string m_stringValue;
	int m_intValue = 0;
};


struct Lexer
{
	Lexer(InputStream& inStream) : m_inputStream(inStream) {}

	Token getNextToken();
	void pushBackToken();
	Token peekNextToken();

protected:
	Token getNextTokenFromStream();

	bool lexNonWords(Token& token);
	bool lexIdentifier(std::string& identifier);
	bool lexInt(int& value);

	void eatWhiteSpace();

	InputStream& m_inputStream;
	std::vector<Token> m_streamBuffer;
	std::vector<Token>::size_type m_streamPosition = 0;

	std::map<std::string, TokenKind> m_keywords = {
		{"print", TokenKind::Print},
	};

	struct LexNonWordsPair
	{
		std::string name;
		TokenKind tokenKind;
	};
	std::vector<LexNonWordsPair> m_nonWordTokens = {
		{"(", TokenKind::Lparen},
		{")", TokenKind::Rparen},
		{";", TokenKind::Semicolumn},
		{",", TokenKind::Comma},
		{"+", TokenKind::Plus},
		{"-", TokenKind::Minus},
		{":=", TokenKind::Assign},
	};
};

#endif
