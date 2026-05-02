#include <iostream>
#include <cstdlib>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "InstSelector.hpp"
#include "ResourceAllocator.hpp"
#include "InstPrinter.hpp"

#include "LprsInstructions.hpp"

#include "AstPrinter.hpp"

using namespace std;


/**
 *
 *
 **/


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Usage: lprssplc input_file" << endl;
		cout << "Output is in the file with the same name, but with .asm extensions." << endl;
		return EXIT_SUCCESS;
	}
	string inputFileName(argv[1]);
	ifstream inputFile(inputFileName);
	InputStream is(inputFileName);

	// Syntax analysis: Lexer and Parser
	Parser pars(is);
	AstStm* root = pars.parse();

	// Debug print
	AstPrinter astPrinter;
	astPrinter.printAst(root);
	cout << endl;

	// Instruction selection
	InstSelector instSelector;
	auto instList = instSelector.selectInstructions(root);

	// Resource allocation
	ResourceAllocator resourceAllocator;
	resourceAllocator.allocateResources(instList);

	// Generate assembly code
	string::size_type p = inputFileName.rfind(".");
	string outputFileName = inputFileName.substr(0, p) + ".asm";
	ofstream outputFile(outputFileName);
	InstPrinter instPrinter;
	for (auto it : instList)
	{
		outputFile << instPrinter.toString(it) << endl;
	}
	outputFile.close();

	return EXIT_SUCCESS;
}
