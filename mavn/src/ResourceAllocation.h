#pragma once

#include "InterferenceGraph.h"
#include "Simplification.h"

/**
 * Performs resource allocation based on simplification stack and the interference matrix.
 * @param - ss
 *	Reference to simplification stack
 * @param - ig
 *	Reference to interference graph
 * @return
 *	true - allocation without spills, otherwise false
 */
bool doAllocation(SimplificationStack& ss, InterferenceGraph& ig);
