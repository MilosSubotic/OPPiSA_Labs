#include "Commons.h"


static Instructions instructions;

static Variables regVariables;

static Variables memVariables;

static Labels labels;

static std::string functionName;

Instructions& getInstructions()
{
	return instructions;
}


Variables& getRegVariables()
{
	return regVariables;
}


Variables& getMemVariables()
{
	return memVariables;
}


Labels& getLabels()
{
	return labels;
}


std::string& getFunctionName()
{
	return functionName;
}
