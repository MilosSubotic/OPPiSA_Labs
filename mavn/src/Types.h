#pragma once

#include "Constants.h"

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <stack>

/**
* Supported token types.
*/
enum TokenType
{
	T_NO_TYPE,

	T_ID,
	T_M_ID,
	T_R_ID,
	T_NUM,
	T_WHITE_SPACE,

	// reserved words
	T_MEM,
	T_REG,
	T_FUNC,
	T_ADD,
	T_ADDI,
	T_SUB,
	T_LA,
	T_LI,
	T_LW,
	T_SW,
	T_BLTZ,
	T_B,
	T_NOP,

	// operators
	T_COMMA, 
	T_L_PARENT,
	T_R_PARENT,
	T_COL,
	T_SEMI_COL,

	// utility
	T_COMMENT,
	T_END_OF_FILE,
	T_ERROR,
};


/**
 * Instruction type.
 */
enum InstructionType
{
	I_NO_TYPE = 0,
	I_ADD,
	I_ADDI,
	I_SUB,
	I_LA,
	I_LI,
	I_LW,
	I_SW,
	I_BLTZ,
	I_B,
	I_NOP
};

/**
 * Reg names.
 */
enum Regs
{
	no_assign = 0,
	t0,
	t1,
	t2,
	t3,
	t4
};
