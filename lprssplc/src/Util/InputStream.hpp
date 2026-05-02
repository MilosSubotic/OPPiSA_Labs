#ifndef	__INPUT_STREAM__
#define __INPUT_STREAM__

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

class InputStream
{
public:
	InputStream(std::ifstream& inStream);
	InputStream(const std::string& inStream);

	char getNextChar();
	void pushBackChar();

	bool isEof();

	std::size_t getStreamPosition() const { return m_streamPosition; }
	void setStreamPosition(std::size_t streamPosition) { m_streamPosition = streamPosition; }

protected:
	std::vector<char> m_streamBuffer;
	std::size_t m_streamPosition = 0;
	
};

#endif
