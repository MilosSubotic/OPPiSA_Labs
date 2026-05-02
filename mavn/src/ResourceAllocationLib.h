#pragma once

#include "InterferenceGraph.h"

bool doResourceAllocation();

/**
 * Checks whether the allocation was successful by going through variables and interference matrix.
 * If variables are in interference and have same assignment the allocation did not succeed.
 * @param - ig
 *	Reference to interference graph
 * @return
 *	true - allocation successful, otherwise false
 */
bool checkResourceAllocation(InterferenceGraph& ig);
