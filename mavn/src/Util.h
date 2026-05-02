#pragma once

#include "IR.h"
#include <string>

/**
 * Use this function to print given variable.
 * @param variable pointer to variable for printing
 */
void printVariable(Variable* variable);

/**
 * Use this function to print all variables in list.
 * @param variables list of variables
 */
void printVariables(Variables& variables);

/**
 * Use this function to print names of all variables in list.
 * @param variables list of variables
 */
void printVariablesNames(Variables& variables);

/**
 * Use this function to check if variable exists in variables list.
 * @param variable pointer to variable which existence is meant to be checked
 * @param variables list of variables to check existence of variable
 * @return true if variable exists in variables list; false otherwise
 */
bool variableExists(Variable* variable, Variables& variables);

std::string instructionTypeToString(InstructionType t);

std::string registerToString(Regs r);

Variable* variableExists(std::string varName, Variables& variables);

Label* labelExists(std::string name, Labels& labels);

Instruction* getInstructionByPosition(int pos, Instructions& instrs);

/**
 * Use this function to print all instructions in Instructions list.
 * @param instrs list of instructions
 */
void printInstructions(Instructions& instrs);

