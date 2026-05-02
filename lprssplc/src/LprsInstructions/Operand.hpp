#ifndef __OPERAND__
#define __OPERAND__

#include <string>


enum class OperandKind
{
	Register,
	Memory,
	Immediate,
};


struct Operand
{
	OperandKind getKind() const { return kind; }

	virtual std::string toString() const = 0;

protected:
	Operand(OperandKind k) : kind(k) {}
	OperandKind kind;
};


struct RegisterOperand : Operand
{
	RegisterOperand(int i) : Operand(OperandKind::Register), index(i) {}

	std::string toString() const override
	{
		return "R" + std::to_string(index);
	}

	int index;
};


struct MemoryOperand : Operand
{
	MemoryOperand() : Operand(OperandKind::Memory), address(-1) {}
	MemoryOperand(int a) : Operand(OperandKind::Memory), address(a) {}

	std::string toString() const override
	{
		return std::to_string(address);
	}

	int address;
};


struct ImmOperand : Operand
{
	ImmOperand(size_t v) : Operand(OperandKind::Immediate), value(v) {}

	std::string toString() const override
	{
		return std::to_string(value);
	}

	int value;
};

#endif
