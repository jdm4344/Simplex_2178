// Jordan Machalek
// Section 1
// Final
// For the purpose of this program, positions denoted "y" in this class are z-values in 3D
// By the time I realized what I had been writing it was not worth the work to go back through 
// and fix
#include "AVertex.h"

AVertex::AVertex()
{
	weight = 1;
	aboveVertex = nullptr;
	belowVertex = nullptr;
	leftVertex = nullptr;
	rightVertex = nullptr;
}

AVertex::~AVertex()
{
}
