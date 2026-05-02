#include "InstPrinter.hpp"

#include <map>
#include <cassert>

#include "LprsInstructions.hpp"

using namespace std;


static map<LprsInstKind, string> g_stringPatterns =
{
	{LprsInstKind::Mov, "mov %d, %s1"},
	{LprsInstKind::Add, "add %d, %s1, %s2"},
	{LprsInstKind::Sub, "sub %d, %s1, %s2"},
	{LprsInstKind::Or,  "or  %d, %s1, %s2"},
	{LprsInstKind::Not, "not %d, %s1"},
	{LprsInstKind::Inc, "inc %d, %s1"},
	{LprsInstKind::Dec, "dec %d, %s1"},
	{LprsInstKind::Shl, "shl %d, %s1"},
	{LprsInstKind::Shr, "shr %d, %s1"},
	{LprsInstKind::Neg, "neg %d, %s1"},
	{LprsInstKind::Ashr, "ashr %d, %s1"},

	{LprsInstKind::Jmp, "jmp %s1"},
	{LprsInstKind::Jmpz, "jmpz %s1"},
	{LprsInstKind::Jmps, "jmps %s1"},
	{LprsInstKind::Jmpc, "jmpc %s1"},

	{LprsInstKind::Jmpnz, "jmpnz %s1"},
	{LprsInstKind::Jmpns, "jmpns %s1"},
	{LprsInstKind::Jmpnc, "jmpnc %s1"},

	{LprsInstKind::JmpR, "jmp %s1"},
	{LprsInstKind::JmpRz, "jmpz %s1"},
	{LprsInstKind::JmpRs, "jmps %s1"},
	{LprsInstKind::JmpRc, "jmpc %s1"},

	{LprsInstKind::JmpRnz, "jmpnz %s1"},
	{LprsInstKind::JmpRns, "jmpns %s1"},
	{LprsInstKind::JmpRnc, "jmpnc %s1"},

	{LprsInstKind::LdR, "ld %d, %s1"},
	{LprsInstKind::Ld, "ld %d, %s1"},

	{LprsInstKind::Lrl, "lrl %d, %s1"},
	{LprsInstKind::Lrm, "lrm %d, %s1"},
	{LprsInstKind::Lrh, "lrh %d, %s1"},

	{LprsInstKind::StR, "st %d, %s1"},
	{LprsInstKind::St, "st %d, %s1"},
};


string InstPrinter::toString(LprsInstruction* inst)
{
	auto it = g_stringPatterns.find(inst->getKind());
	if (it == g_stringPatterns.end())
	{
		assert(false);
		return "";
	}

	string r = it->second;

	string::size_type p = r.find("%d");
	if (p != string::npos) r.replace(p, 2, inst->getDest()->toString());
	p = r.find("%s1");
	if (p != string::npos) r.replace(p, 3, inst->getSrc1()->toString());
	p = r.find("%s2");
	if (p != string::npos) r.replace(p, 3, inst->getSrc2()->toString());

	return r;
}
