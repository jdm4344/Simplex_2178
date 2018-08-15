// Jordan Machalek
// Section 1
// Final
// For the purpose of this program, positions denoted "y" in this class are z-values in 3D
// By the time I realized what I had been writing it was not worth the work to go back through 
// and fix
#ifndef __GRAPHCLASS_H_
#define __GRAPHCLASS_H_

#include "Simplex\Simplex.h"
#include "AVertex.h"

class Graph
{
public:
	Graph();
	Graph(Graph const& input);
	Graph(int** grid, int width, int height, int xStart, int yStart, int xEnd, int yEnd);
	~Graph();
	// Variables
	std::vector<AVertex*> vertices;
	AVertex* startVertex;
	AVertex* endVertex;
	// Methods
	// Creates vertices based on the given data for a map grid
	std::vector<AVertex*> SetUpGraph(int** grid, int width, int height);
	// Assigns the adjacency for each created vertex
	std::vector<AVertex*> SetAdjacency(int** grid, std::vector<AVertex*> vertexList, int width, int height);
	// Sets the starting and ending vertices
	void SetEnds(int xStart, int yStart, int xEnd, int yEnd, AVertex& startVertex, AVertex& endVertex, std::vector<AVertex*> vertexList);
	// Determines the distance from each position to endVertex
	std::vector<AVertex*> CalcHeuristic(std::vector<AVertex*> vertexList, AVertex* endVertex);
	// Calculates which vertex should be moved to next
	AVertex* GetNextPos(int distance, std::vector<AVertex*> vertexList, int xLoc, int yLoc);
	// AStar calculates the best possible path to the destination based on best guess of distance per position
	void AStar(int distance, std::vector<AVertex*> vertexList, int xLoc, int yLoc, std::vector<int>* xList, std::vector<int>* yList, int xEnd, int yEnd);
};
#endif 