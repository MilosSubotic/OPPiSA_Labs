#ifndef __LPRS_INSTRUCTIONS__
#define __LPRS_INSTRUCTIONS__

#include <string>
#include <map>

#include "Operand.hpp"


enum class LprsInstKind
{
	Mov,
	Add,
	Sub,
	And,
	Or,
	Not,
	Inc,
	Dec,
	Shl,
	Shr,
	Neg,
	Ashr,

	Jmp,
	Jmpz,
	Jmps,
	Jmpc,

	Jmpnz,
	Jmpns,
	Jmpnc,

	JmpR,
	JmpRz,
	JmpRs,
	JmpRc,

	JmpRnz,
	JmpRns,
	JmpRnc,

	LdR,
	Ld,

	Lrl,
	Lrm,
	Lrh,

	St,
	StR,
};


struct LprsInstruction
{
	LprsInstruction(LprsInstKind k, Operand* d, Operand* s1, Operand* s2)
		: m_kind(k), m_dest(d), m_src1(s1), m_src2(s2) {}

	LprsInstruction(LprsInstKind k, Operand* d, Operand* s1)
		: m_kind(k), m_dest(d), m_src1(s1), m_src2(nullptr) {}

	LprsInstruction(LprsInstKind k, Operand* s1)
		: m_kind(k), m_dest(nullptr), m_src1(s1), m_src2(nullptr) {}

	LprsInstKind getKind() { return m_kind; }

	Operand* getDest() { return m_dest; }
	Operand* getSrc1() { return m_src1; }
	Operand* getSrc2() { return m_src2; }

private:
	Operand* m_dest;
	Operand* m_src1;
	Operand* m_src2;

	LprsInstKind m_kind;
};

#endif
