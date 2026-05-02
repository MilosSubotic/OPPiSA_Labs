#pragma once

#include "IR.h"

/**
 * @brief Use this function to get instructions list
 *
 * @return reference to instructions list
 */
Instructions& getInstructions();

/**
 * @brief Use this function to get register variables list
 *
 * @return reference to register variables list
 */
Variables& getRegVariables();

/**
 * @brief Use this function to get memory variables list
 *
 * @return reference to memory variables list
 */
Variables& getMemVariables();


Labels& getLabels();


std::string& getFunctionName();

