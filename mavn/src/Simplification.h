#pragma once

#include "InterferenceGraph.h"

#include <stack>

/**
 * This type represents stack of pointers to variables.
 */
typedef std::stack<Variable*> SimplificationStack;

/**
 * Performs simplification of the interference graph by removing variable nodes from it.
 * @param - ig
 *	Reference of the interference graph
 * @param - degree
 *	Number of available registers
 * @return
 *	Reference to the simplification stack
 */
SimplificationStack& doSimplification(InterferenceGraph& ig, int degree);
