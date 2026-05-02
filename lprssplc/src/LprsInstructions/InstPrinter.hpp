#ifndef __INST_PRINTER__
#define __INST_PRINTER__

#include <string>

struct LprsInstruction;


struct InstPrinter
{
	std::string toString(LprsInstruction* inst);
};


#endif
