#include "VariableTable.hpp"

#include "Operand.hpp"


Operand* VariableTable::insert(std::string name)
{
	auto it = m_table.find(name);
	if (it == m_table.end())
	{
		Operand* p = new MemoryOperand();
		m_table[name] = p;
		return p;
	}
	else
	{
		return it->second;
	}
}


Operand* VariableTable::find(std::string name)
{
	auto it = m_table.find(name);
	if (it != m_table.end())
	{
		return it->second;
	}

	return nullptr;
}
