#include "assemblerLib.h"
#include "machineInstructionList.h"
#include "pseudoInstructionList.h"
#include "sourceLoader.h"
#include "passI.h"
#include "passII.h"

#include <iostream>

using namespace std;


void usage() {
	cout << "Wrong number of input parameteres.\n\n";
	cout << "Set parameters In command line like a form:\n";
	cout << "assembler.exe <inputFile> [output_file]\n";
}

int main(const int argc, const char* argv[]) {

	string inputFile = "..\\fact.s";
	string outputFileBin = "..\\fact.o";
	string outputFileFxt = "..\\fact.lst";

	cout << "\nSPPURV1 - V6\n\tMIPS Assembler\n\n\n";

	// check passed arguments
	if (argc > 3) {
		usage();
		return 1;
	} else if (argc == 2) {
		inputFile.assign(argv[1]);
		string basic_name = inputFile.substr(0, inputFile.rfind("."));
		outputFileBin = basic_name + ".o";
		outputFileFxt = basic_name + ".lst";
	} else if (argc == 3) {
		inputFile.assign(argv[1]);
		outputFileBin.assign(argv[2]);
		outputFileFxt.assign(argv[2]);
		outputFileBin = outputFileBin.substr(0, outputFileBin.rfind(".")) + ".o";
		outputFileFxt = outputFileFxt.substr(0, outputFileFxt.rfind(".")) + ".lst";
	}

	// initialize all tables
	init(g_machineInstructions, g_pseudoInstructions);

	// load source to sourceList and replace pseudo instructions
	if (loadSourceRepPseudo(inputFile)) {
		// run first pass
		if (passI()) {
			// run second pass
			if (passII()) {
				cout << "\n---------------------------------";
				cout << "\nAssembling completed successfuly.\n\n";

				// write binary and text data to out file
				writeDataToFile(outputFileBin, outputFileFxt);
			}
		}
	}


	cout << "\n";
	return 0;
}
