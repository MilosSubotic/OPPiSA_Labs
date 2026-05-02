#pragma once

#include "Types.h"


/**
 * Performs liveness analysis.
 * @param - mode
 *	__DUMPS__ the function will print out list of instructions before and after the analysis
 *	__NO_DUMPS__ the function will not print out list of instructions
 */
void doLivenessAnalysis(int mode);
