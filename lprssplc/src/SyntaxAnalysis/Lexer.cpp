#include "Lexer.hpp"

#include <string>
#include <cctype>

#include "Error.hpp"

using namespace std;


Token Lexer::getNextToken()
{
	if (m_streamPosition < m_streamBuffer.size())
	{
		m_streamPosition++;
		return m_streamBuffer[m_streamPosition - 1];
	}
	else
	{
		m_streamPosition++;
		Token token = getNextTokenFromStream();
		m_streamBuffer.push_back(token);
		return token;
	}
}


void Lexer::pushBackToken()
{
	m_streamPosition--;
}


Token Lexer::peekNextToken()
{
	Token tmp = getNextToken();
	pushBackToken();
	return tmp;
}


Token Lexer::getNextTokenFromStream()
{
	eatWhiteSpace();

	if (m_inputStream.isEof())
	{
		return Token(TokenKind::Eof);
	}

	Token token;
	if (lexNonWords(token))
	{
		return token;
	}

	string identifier;
	if (lexIdentifier(identifier))
	{
		if (m_keywords.find(identifier) != m_keywords.end())
		{
			TokenKind tokenKind = m_keywords[identifier];
			return Token(tokenKind, identifier);
		}

		return Token(TokenKind::Id, identifier);
	}

	int intValue;
	if (lexInt(intValue))
	{
		return Token(TokenKind::Int, intValue);
	}

	error("Lexical error");

	return Token(TokenKind::Eof);
}


bool Lexer::lexNonWords(Token& token)
{
	for (const auto& it : m_nonWordTokens)
	{
		const std::string& op = it.name;

		std::size_t oldStreamPosition = m_inputStream.getStreamPosition();

		bool tokenMatched = true;
		for (std::size_t i = 0; i < op.size(); i++)
		{
			char c = m_inputStream.getNextChar();
			if (c != op[i])
			{
				m_inputStream.setStreamPosition(oldStreamPosition);
				tokenMatched = false;
				break; // next operator
			}
		}
		if (tokenMatched)
		{
			token.setKind(it.tokenKind);
			token.setStringValue(it.name);
			return true;
		}
	}
	return false;
}


bool Lexer::lexIdentifier(std::string& identifier)
{
	char c = m_inputStream.getNextChar();
	if (!isalpha(c) && (c != '_'))
	{
		m_inputStream.pushBackChar();
		return false;
	}

	identifier = "";
	identifier += c;

	while (true)
	{
		c = m_inputStream.getNextChar();
		if (!isalpha(c) && (c != '_') && !isdigit(c))
		{
			m_inputStream.pushBackChar();
			return true;
		}
		identifier += c;
	}
}


bool Lexer::lexInt(int& value)
{
	char c = m_inputStream.getNextChar();
	if (!isdigit(c))
	{
		m_inputStream.pushBackChar();
		return false;
	}

	std::string stringValue = "";
	stringValue += c;

	while (true)
	{
		c = m_inputStream.getNextChar();
		if (!isdigit(c))
		{
			m_inputStream.pushBackChar();
			value = atoi(stringValue.c_str());
			return true;
		}
		stringValue += c;
	}
}


void Lexer::eatWhiteSpace()
{
	while (true)
	{
		char c = m_inputStream.getNextChar();
		if (!isblank(c) && c != '\r' && c != '\n')
		{
			m_inputStream.pushBackChar();
			return;
		}
	}
}
