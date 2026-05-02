#ifndef	__INPUT_STREAM__
#define __INPUT_STREAM__

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#define DEBUG(var) \
	do{ \
		std::cout << #var << " = " << var << std::endl; \
	}while(0)

class InputStream
{
public:
	InputStream(const std::string& fileName);

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
