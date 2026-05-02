#include "Simplification.h"


static SimplificationStack simplificationStack;


int getVariableDegree(InterferenceMatrix& matrix, Variable * variable)
{
	RegisterVariable* regVar = (RegisterVariable*) variable;
	int degree = 0;

	for (size_t col = 0; col < matrix.size(); col++)
	{
		if (matrix[regVar->getPosition()][col] == __INTERFERENCE__)
		{
			degree ++;
		}
	}

	return degree;
}


Variable* findSimplyNode(InterferenceMatrix& matrix, Variables& vars, int degree)
{
	Variables sortedVars;
	Variables copyList(vars);
	Variable* maxDegreeVar;

	while (!copyList.empty())
	{
		int maxDegree = -1;
		maxDegreeVar = *copyList.begin();
		for (auto it = copyList.begin(); it != copyList.end(); it++)
		{
			Variable* variable = (*it);
			int varDegree = getVariableDegree(matrix, variable);
			if (varDegree > maxDegree)
			{
				maxDegree = varDegree;
				maxDegreeVar = *it;
			}
		}
		sortedVars.push_back(maxDegreeVar);
		copyList.remove(maxDegreeVar);
	}

	for (auto it = sortedVars.begin(); it != sortedVars.end(); it++)
	{
		Variable* variable = (*it);
		
		int varDegree = getVariableDegree(matrix, variable);
		if (varDegree < degree)
		{
			vars.remove(variable);
			return variable;
		}
	}

	// there is no more simply node in graph
	return NULL;
}


void simplify(InterferenceMatrix& matrix, Variable* var)
{
	RegisterVariable* regVar = (RegisterVariable*) var;
	for(size_t i = 0; i < matrix.size(); i ++)
	{
		matrix[regVar->getPosition()][i] = __EMPTY__;
		matrix[i][regVar->getPosition()] =  __EMPTY__;
	}
}


SimplificationStack& doSimplification(InterferenceGraph& ig, int degree)
{
	InterferenceMatrix igMatrixCopy(ig.getMatrix());
	Variables varListCopy(ig.getIgVariables());

	while(1)
	{
		Variable* variable = findSimplyNode(igMatrixCopy, varListCopy, degree);

		if (variable == NULL)
		{
			/// spill or simplification end
			break;
		}
		else
		{
			simplify(igMatrixCopy, variable);
			simplificationStack.push(variable);
		}
	}

	return simplificationStack;
}