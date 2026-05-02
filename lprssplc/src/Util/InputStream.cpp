#include "InputStream.hpp"

using namespace std;


InputStream::InputStream(ifstream& inStream)
{
	inStream.seekg(0, inStream.end);
	std::size_t length = static_cast<std::size_t>(inStream.tellg());
	inStream.seekg(0, inStream.beg);
	m_streamBuffer.resize(length);
	inStream.read(&m_streamBuffer.front(), length);
}


InputStream::InputStream(const std::string& inStream)
{
	m_streamPosition = 0;
	m_streamBuffer.resize(inStream.length());
	copy(inStream.begin(), inStream.end(), m_streamBuffer.begin());
}


char InputStream::getNextChar()
{
	if (m_streamPosition < m_streamBuffer.size())
	{
		m_streamPosition++;
		return m_streamBuffer[m_streamPosition - 1];
	}
	else
	{
		m_streamPosition++;
		return 0;
	}
}


void InputStream::pushBackChar()
{
	if (m_streamPosition > 0)
		m_streamPosition--;
}


bool InputStream::isEof()
{
	if (m_streamPosition >= m_streamBuffer.size())
		return true;
	else
		return false;
}
