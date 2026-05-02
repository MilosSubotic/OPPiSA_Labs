#pragma once

#include "IR.h"
#include "Commons.h"

/**
 * This type represents matrix of integers.
 */
typedef std::vector<std::vector<int>> InterferenceMatrix;

/**
 * This class represents interference graph.
 */
class InterferenceGraph
{
public:
	InterferenceGraph() : m_variables(getRegVariables()) {} 
	Variables& getIgVariables() {return m_variables; }
	InterferenceMatrix& getMatrix() { return m_matrix; }

private:
	Variables& m_variables;
	InterferenceMatrix m_matrix;
};


/**
 * Builds the interference graph based on instruction list of the analyzed program.
 * @param - instructions
 *	List of instructions
 * @return
 *	Reference to the interference graph
 */
InterferenceGraph& buildInterferenceGraph(Instructions& instructions);

/**
 * Prints the interference graph
 */
void printInterferenceMatrix();
