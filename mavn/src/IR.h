#pragma once

#include "Types.h"

/**
 * This class represents one variable from program code.
 */
class Variable
{
public:
	enum VariableType
	{
		NO_TYPE,
		MEM_VAR,
		REG_VAR,
		CONST_VAR
	};
	Variable() : m_name(""), m_varType(NO_TYPE) {}
	Variable(std:: string name, VariableType type) : m_name(name), m_varType(type) {}
	std::string getName() {return m_name;}
	void setName(std::string name) { m_name = name;}
	VariableType getType() {return m_varType;}
	void setType(VariableType type) {m_varType = type;}
private:
	std::string m_name;
	VariableType m_varType;
};


/**
 * This type represents list of variables from program code.
 */
typedef std::list<Variable*> Variables;


/**
 * This class represents register variable in program code.
 */
class RegisterVariable : public Variable 
{
public:
	RegisterVariable(std::string nName, int pos) : Variable(nName, Variable::REG_VAR),
		m_position(pos), m_assignment(no_assign) {}
	int getPosition() {return m_position;}
	void setPosition(int pos) {m_position = pos;}
	Regs getAssignment() {return m_assignment;}
	void setAssignment(Regs reg) {m_assignment = reg;}
private:
	int m_position;
	Regs m_assignment;
};


/**
 * This class represents memory variable in program code.
 */
class MemoryVariable : public Variable 
{
public:
	MemoryVariable(std::string nName, std::string value) : Variable(nName, Variable::MEM_VAR),
		m_value(value){}
	std::string getValue() {return m_value;}
	void setValue(int val) {m_value = val;}
private:
	std::string m_value;
};


/**
 * This class represents constant variable in program code.
 */
class ConstVariable : public Variable 
{
public:
	ConstVariable(std::string value) : Variable("", Variable::CONST_VAR),
		m_value(value){}
	std::string getValue() {return m_value;}
	void setValue(std::string val) {m_value = val;}
private:
	std::string m_value;
};


class Label
{
public:
	Label() : m_name(""), m_instrPosition(-1) {}
	Label(std::string name, int position) : m_name(name), m_instrPosition(position) {}
	std::string getName() {return m_name;}
	void setName(std::string name) {m_name = name;}
	int getPosition() {return m_instrPosition;}
	void setPosition(int pos) {m_instrPosition = pos;}
private:
	std::string m_name;
	int m_instrPosition;
};

/**
 * This type represents labels in in program code.
 */
typedef std::list<Label*> Labels;


/**
 * This class represents one instruction in program code.
 */
class Instruction
{
public:
	Instruction () : m_position(0), m_type(I_NO_TYPE) {}
	Instruction (int pos, InstructionType type, Variables& dst, Variables src, Label* label) :
		m_position(pos), m_type(type), m_dst(dst), m_src(src), m_targetLabel(label) {}

	Variables& getDst() {return m_dst;}
	Variables& getSrc() {return m_src;}

	Variables& getDef() {return m_def;}
	Variables& getUse() {return m_use;}
	Variables& getIn() {return m_in;}
	Variables& getOut() {return m_out;}
	
	std::list<Instruction*>& getPred() {return m_pred;}
	std::list<Instruction*>& getSucc() {return m_succ;}

	int getPosition() {return m_position;}
	void setPosition(int pos) {m_position = pos;}

	InstructionType getType() {return m_type;}
	void setType(InstructionType type) {m_type = type;}

	Label* getTargetLabel() { return m_targetLabel;}
	void setTargetLabel(Label* label) { m_targetLabel = label; }

private:
	int m_position;
	InstructionType m_type;
	Variables m_dst;
	Variables m_src;

	Variables m_use;
	Variables m_def;
	Variables m_in;
	Variables m_out;
	std::list<Instruction*> m_succ;
	std::list<Instruction*> m_pred;

	Label* m_targetLabel;
};

/**
 * This type represents few instructions in program code.
 */
typedef std::list<Instruction*> Instructions;
