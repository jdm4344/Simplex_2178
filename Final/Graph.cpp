// Jordan Machalek
// Section 1
// Final
// For the purpose of this program, positions denoted "y" in this class are z-values in 3D
// By the time I realized what I had been writing it was not worth the work to go back through 
// and fix
#include "Graph.h"

Graph::Graph()
{
}

Graph::Graph(Graph const & input)
{
}

Graph::Graph(int** grid, int width, int height, int xStart, int yStart, int xEnd, int yEnd)
{
	startVertex = new AVertex();
	endVertex = new AVertex();
	vertices = SetUpGraph(grid, width, height);
	vertices = SetAdjacency(grid, vertices, width, height);
	SetEnds(xStart, yStart, xEnd, yEnd, *startVertex, *endVertex, vertices);
	vertices = CalcHeuristic(vertices, endVertex);
}

Graph::~Graph()
{
	delete startVertex;
	delete endVertex;
}

std::vector<AVertex*> Graph::SetUpGraph(int** grid, int width, int height)
{
	std::vector<AVertex*> tempVerts; 

	for (int i = 0; i < width; i++) // row
	{
		for (int j = 0; j < height; j++) // col
		{
			// Create a new vertex and assigh its data based on current iteration then save it
			AVertex* vert = new AVertex();
			vert->xPos = i;
			vert->yPos = j;
			vert->weight = grid[i][j];
			tempVerts.push_back(vert);
		}
	}

	return tempVerts;
}

std::vector<AVertex*> Graph::SetAdjacency(int** grid, std::vector<AVertex*> vertexList, int width, int height)
{
	for (size_t i = 0; i < vertexList.size(); i++) // row
	{
		for (size_t j = 0; j < vertexList.size(); j++) // col
		{
			if (&vertexList[i] != &vertexList[j]) // vertex shouldn't/can't be adjacent to itself
			{
				// Sets belowVertex
				if (vertexList[i]->xPos == vertexList[j]->xPos && vertexList[i]->yPos - 1 == vertexList[j]->yPos)
				{
					vertexList[i]->belowVertex = vertexList[j];
				}
				// Set aboveVertex
				if (vertexList[i]->xPos == vertexList[j]->xPos && vertexList[i]->yPos + 1 == vertexList[j]->yPos)
				{
					vertexList[i]->aboveVertex = vertexList[j];
				}
				// Set leftVertex
				if (vertexList[i]->yPos == vertexList[j]->yPos && vertexList[i]->xPos - 1 == vertexList[j]->xPos)
				{
					vertexList[i]->leftVertex = vertexList[j];
				}
				// Set rightVertex
				if (vertexList[i]->yPos == vertexList[j]->yPos && vertexList[i]->xPos + 1 == vertexList[j]->xPos)
				{
					vertexList[i]->rightVertex = vertexList[j];
				}
			}
		}
	}

	return vertexList;
}

void Graph::SetEnds(int xStart, int yStart, int xEnd, int yEnd, AVertex& startVertex, AVertex& endVertex, std::vector<AVertex*> vertexList)
{
	// Search through all vertices and find the start and end, then assign them
	for (int i = 0; i < vertexList.size(); i++)
	{
		if (vertexList[i]->xPos == xStart && vertexList[i]->yPos == yStart)
		{
			startVertex = *vertexList[i];
		}
		else if (vertexList[i]->xPos == xEnd && vertexList[i]->yPos == yEnd)
		{
			endVertex = *vertexList[i];
		}
	}
}

std::vector<AVertex*> Graph::CalcHeuristic(std::vector<AVertex*> vertexList, AVertex* endVertex)
{
	for (int i = 0; i < vertexList.size(); i++)
	{
		// distance = (root)[x1 - x2]^2 + [y1 - y2]^2
		vertexList[i]->heuristic = sqrt(pow((vertexList[i]->xPos - endVertex->xPos), 2) + pow((vertexList[i]->yPos - endVertex->yPos), 2));
	}

	return vertexList;
}

AVertex* Graph::GetNextPos(int distance, std::vector<AVertex*> vertexList, int xLoc, int yLoc)
{
	AVertex* nextVertex = nullptr;
	AVertex* currVertex = nullptr;
	int path = -1; 

	// Get current position
	for (int i = 0; i < vertexList.size(); i++)
	{
		if (vertexList[i]->xPos == xLoc && vertexList[i]->yPos == yLoc)
		{
			currVertex = vertexList[i];
			break;
		}
	}

	// Examine all adjacent vertices, move to the best one
	// Top
	if (currVertex->aboveVertex != nullptr)
	{
		if (nextVertex == nullptr)
		{
			// Set nextVertex and update the path value
			nextVertex = currVertex->aboveVertex;
			path = distance + nextVertex->heuristic + nextVertex->weight;
		}
		// This is the first path cost calculated so there is no need to compare it against the nextVertex
	}
	
	// Bottom
	if (currVertex->belowVertex != nullptr)
	{
		if (nextVertex == nullptr)
		{
			// Set nextVertex and update the path value
			nextVertex = currVertex->belowVertex;
			path = distance + nextVertex->heuristic + nextVertex->weight;
		}
		else // Check path cost against currently chosen next vertex
		{
			int belowPath = distance + currVertex->belowVertex->heuristic + currVertex->belowVertex->weight;

			if (belowPath < path)
			{
				nextVertex = currVertex->belowVertex;
				path = belowPath;
			}
		}
	}

	// Left
	if (currVertex->leftVertex != nullptr)
	{
		if (nextVertex == nullptr)
		{
			nextVertex = currVertex->leftVertex;
			path = distance + nextVertex->heuristic + nextVertex->weight;
		}
		else // Check path cost against currently chosen next vertex
		{
			int leftPath = distance + currVertex->leftVertex->heuristic + currVertex->leftVertex->weight;

			if (leftPath < path)
			{
				nextVertex = currVertex->leftVertex;
				path = leftPath;
			}
		}
	}

	// Right
	if (currVertex->rightVertex != nullptr)
	{
		if (nextVertex == nullptr)
		{
			nextVertex = currVertex->rightVertex;
			path = distance + nextVertex->heuristic + nextVertex->weight;
		}
		else // Check path cost against currently chosen next vertex
		{
			int rightPath = distance + currVertex->rightVertex->heuristic + currVertex->rightVertex->weight;

			if (rightPath < path) // Just because its the "rightPath" doesn't mean its the correct one
			{
				nextVertex = currVertex->rightVertex;
				path = rightPath;
			}
		}
	}

	return nextVertex;
}

void Graph::AStar(int distance, std::vector<AVertex*> vertexList, int xPos, int yPos, std::vector<int>* xList, std::vector<int>* yList, int xEnd, int yEnd)
{
	// Keep track of the starting location
	int xStart = xPos;
	int yStart = yPos;
	std::vector<AVertex*> pathList; // vertices on current path
	pathList.push_back(startVertex);

	// Generate a base path
	int hitCount = 0;

	/*
	This loop runs forever, hence the existence of the hitCount variable above
	It would appear that the GetNextPos() method is to blame, as it's just
	returning vertices with incrementing x and y (z) values and I was unable to
	determine where the fault in the method is that is causing this.
	*/
	while (true)
	{
		// Get the next best location to move to
		AVertex* nextVertex = GetNextPos(distance, vertexList, xPos, yPos);
		
		(*xList).push_back(nextVertex->xPos);
		(*yList).push_back(nextVertex->yPos);

		pathList.push_back(nextVertex);

		// Advance to next location
		xPos = nextVertex->xPos;
		yPos = nextVertex->yPos;
		distance++;
		hitCount++;

		if ((nextVertex->xPos == xEnd && nextVertex->yPos == yEnd) || hitCount > 29) // Reached the end?
		{
			break;
		}
	}

	// Reexamine the base path to see if there is a more efficient path within
	std::vector<int> xTemp;
	std::vector<int> yTemp;
	
	for (int i = pathList.size() - 1; i > 0; i--)
	{
		xTemp.push_back(pathList[i]->xPos);
		yTemp.push_back(pathList[i]->yPos);

		// Get the difference between coordinates and use that to see if a position is better
		int xDiff = startVertex->xPos - pathList[i]->xPos;
		int yDiff = startVertex->yPos - pathList[i]->yPos;
		int combDiff = xDiff - yDiff;

		if (xDiff <= 1 && xDiff >= -1 && yDiff <= 1 && yDiff >= -1 && (combDiff == 1 || combDiff == -1))
		{
			xTemp.push_back(startVertex->xPos);
			yTemp.push_back(startVertex->yPos);
		}
	}

	// Empty the external list and save the new data to them
	xList->clear();
	yList->clear();

	for (int i = xTemp.size() - 1; i > 0; i--)
	{
		xList->push_back(xTemp[i]);
		yList->push_back(yTemp[i]);
	}

	xList->push_back(xEnd);
	yList->push_back(yEnd);
}
