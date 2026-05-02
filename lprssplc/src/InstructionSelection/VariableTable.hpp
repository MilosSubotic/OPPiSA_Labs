#ifndef __VARIABLE_TABLE__
#define __VARIABLE_TABLE__

#include <map>
#include <string>


struct Operand;


struct VariableTable
{
	Operand* insert(std::string name);
	Operand* find(std::string name);

private:
	std::map<std::string, Operand*> m_table;
};

#endif
