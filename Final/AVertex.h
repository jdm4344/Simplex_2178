// Jordan Machalek
// Section 1
// Final
// For the purpose of this program, positions denoted "y" in this class are z-values in 3D
// By the time I realized what I had been writing it was not worth the work to go back through 
// and fix
#ifndef __AVertexCLASS_H_
#define __AVertexCLASS_H_

class AVertex
{
public:
	AVertex();
	~AVertex();
	// Object data
	int xPos;
	int yPos; // z-Pos in 3D space
	int heuristic;
	int lowestCost;
	bool visited;
	// Adjacent vertices
	AVertex* aboveVertex;
	AVertex* belowVertex;
	AVertex* leftVertex;
	AVertex* rightVertex;
	// Pathing data
	bool start;
	bool end;
	int weight;
};

#endif