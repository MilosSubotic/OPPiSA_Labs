#include "Commons.h"
#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "LivenessAnalysisLib.h"
#include "ResourceAllocationLib.h"
#include "Emitter.h"

#include <iostream>
#include <exception>

using namespace std;

int main(int argc, char** argv)
{
	try
	{
		if (argc < 2)
		{
			throw runtime_error("\nException! Usage: mavn input_file!\n");
		}

		std::string fileName = argv[1];

		LexicalAnalysis lex;
		
		if (!lex.readInputFile(fileName))
			throw runtime_error("\nException! Failed to open input file!\n");
		
		lex.initialize();

		bool retVal = lex.Do();

		if (retVal)
		{
			cout << "Lexical analysis finished successfully!" << endl;
			//lex.printTokens();
		}
		else
		{
			lex.printLexError();
			throw runtime_error("\nException! Lexical analysis failed!\n");
		}

		SyntaxAnalysis syntax(lex);

		retVal = syntax.Do();

		if (retVal)
		{
			cout << "Syntax analysis finished successfully!" << endl << endl;
		}
		else
		{
			throw runtime_error("\nException! Syntax analysis failed!\n");
		}

		printInstructions(getInstructions());

		doLivenessAnalysis(__NO_DUMPS__);

		if (doResourceAllocation())
		{
			printVariables(getRegVariables());
		}

		generateAsmFile(fileName);
	}
	catch (runtime_error e)
	{
		cout << e.what() << endl;
	}

	return 0;
}