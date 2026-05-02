#include "Error.hpp"

#include <iostream>
#include <cstdlib>

using namespace std;


[[noreturn]] void error(const char* message)
{
	cout << message << endl;

	exit(EXIT_FAILURE);
}
