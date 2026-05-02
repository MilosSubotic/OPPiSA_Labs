#pragma once

#include "IR.h"

/**
 * Builds control flow graph
 */
void buildCfg(Instructions& instructions);

void setDefsAndUses(Instructions& instructions);

void livenessAnalysis(Instructions& instructions);

