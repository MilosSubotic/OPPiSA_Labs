#include "InputStream.hpp"

#include <filesystem>

using namespace std;


InputStream::InputStream(const std::string& fileName)
{
	size_t length = filesystem::file_size(fileName);
	m_streamBuffer.resize(length);
	ifstream inputFile(fileName);
	inputFile.read(&m_streamBuffer.front(), length);
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
