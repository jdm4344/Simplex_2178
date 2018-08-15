#include "AppClass.h"
using namespace Simplex;
// Jordan Machalek
// Section 1
// Final
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Jordan Machalek - jdm4344@rit.edu";

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 0.0f, 13.0f), //Position
		vector3(0.0f, 0.0f, 12.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	//Entity Manager
	m_pEntityMngr = MyEntityManager::GetInstance();

#pragma region Model Initialization
	//creeper - start at 0,1,0
	m_pEntityMngr->AddEntity("Minecraft\\Creeper.obj", "Creeper");
	m_v3Creeper = vector3(0.5f, 1.0f, 0.5f);
	m_pEntityMngr->SetModelMatrix(glm::translate(vector3(0.5f, 1.0f, 0.5f)), "Creeper");
	creeperEnt = m_pEntityMngr->GetEntity(0);

	//steve - start at 15, 1, 15
	m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Steve");
	m_v3Steve = vector3(14.5f, 1.0f, 14.5f);
	m_pEntityMngr->SetModelMatrix(glm::translate(m_v3Steve), "Steve");
	steveEnt = m_pEntityMngr->GetEntity(1);

	// Create map vectors and determine the # of cube entities needed for the ground
	for (int i = 0; i < 15; i++) // row
	{
		for (int  j = 0; j < 15; j++) // column
		{
			if (mapData[i][j] == 1)
			{
				mapVectors.push_back(vector3(i, 0, j));
			}
			else if (mapData[i][j] == 2)
			{
				mapVectors.push_back(vector3(i, 1, j));
			}
		}
	}

	// Create the cube entities
	for (int i = 0; i < mapVectors.size(); i++)
	{
			m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Cube_" + std::to_string(i));
			vector3 v3Position = mapVectors[i];
			matrix4 m4Position = glm::translate(IDENTITY_M4, v3Position);
			m_pEntityMngr->SetModelMatrix(m4Position, "Cube_" + std::to_string(i));
	}

#pragma endregion

#pragma region Data Initialization
	// Copy hard-coded array to a dynamic one 
	map = new int *[15];

	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			map[i] = new int[15];
			map[i][j] = mapData[i][j];
		}
	}

	// Set up pathfinding
	(*xlist).push_back(m_v3Creeper.x);
	(*zlist).push_back(m_v3Creeper.z);
	pathVectors.push_back(vector3(m_v3Creeper.x, mapData[(int)m_v3Creeper.x][(int)m_v3Creeper.z] - 1, m_v3Creeper.z));

	pathGraph = new Graph(map, 15, 15, m_v3Creeper.x, m_v3Creeper.z, m_v3Steve.x, m_v3Steve.z);
	/* 
	Issue with A* - See Graph.cpp
	*/
	pathGraph->AStar((*xlist).size(), pathGraph->vertices, m_v3Creeper.x, m_v3Creeper.z, xlist, zlist, m_v3Steve.x, m_v3Steve.z);
	/*
	Issue with A* - See Graph.cpp
	*/

	for (int i = 0; i < (*xlist).size(); i++)
	{
		for (int j = 0; j < (*xlist).size(); j++)
		{
			pathVectors.push_back(vector3(m_v3Creeper.x, mapData[i][j] - 1, m_v3Creeper.z));
		}
	}
#pragma endregion
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

#pragma region Background Updates
	// Check if the creeper has reached steve, if so, move steve
	if (creeperEnt->IsColliding(steveEnt))
	{
		float index = Simplex::GenerateRandom(0, mapVectors.size());
		
		// Create a new vector to place steve on top, center of the cube
		m_v3Steve = vector3(mapVectors[index].x + 0.5f, mapVectors[index].y + 1, mapVectors[index].z + 0.5f);

		m_pEntityMngr->SetModelMatrix(glm::translate(m_v3Steve), "Steve");

		// Clear existing data
		m_pMeshMngr->ClearRenderList();
		xlist->clear();
		zlist->clear();
		delete pathGraph;

		// Rebuild the graph with new end position
		xlist->clear();
		zlist->clear();
		(*xlist).push_back(m_v3Creeper.x);
		(*zlist).push_back(m_v3Creeper.z);
		pathVectors.push_back(vector3(0, 0, 0));

		pathGraph = new Graph(map, 15, 15, m_v3Creeper.x, m_v3Creeper.z, m_v3Steve.x, m_v3Steve.z);
		pathGraph->AStar((*xlist).size(), pathGraph->vertices, m_v3Creeper.x, m_v3Creeper.z, xlist, zlist, m_v3Steve.x, m_v3Steve.z);
	}

	//Set model matrix to the creeper
	//matrix4 mCreeper = glm::translate(m_v3Creeper);
	m_pEntityMngr->SetModelMatrix(glm::translate(m_v3Creeper), "Creeper");
#pragma endregion

#pragma region Movement - lerp
	/*
	Lerping apparently works correctly, though I wasn't able to actually test
	it with a good set of positions due to 
	*/
	// Create a timer
	static float timer = 0;	
	static uint clock = m_pSystem->GenClock(); 
	timer += m_pSystem->GetDeltaTime(clock); 

	// Get current position
	static vector3 currentPos = vector3((*xlist)[currPos], 1, (*zlist)[currPos]);

	// Percentage for lerping
	float percentage = static_cast<float>(MapValue(timer, 0.0f, 5.0f, 0.0f, 1.0f));

	// Lerp between current position and the next stop
	currentPos = glm::lerp(currentPos, pathVectors[currPos], percentage);

	if (percentage > 0.2f) // increment position index and reset timer
	{
		currPos++;
		timer = 0;

		if (currPos >= pathVectors.size()) // reset index
		{
			currPos = 0;
		}
	}

	// Update model matrix for the creeper
	matrix4 mCreeper = glm::translate(currentPos);
	m_pEntityMngr->SetModelMatrix(mCreeper, "Creeper");
#pragma endregion

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	// Show a plane to represent the path of the creeper
	/*
	The planes to show the path that the creeper is supposed to take do not work correctly
	One reason for this is the fault that exists with my fauly A* and my creation of vectors
	based on it. 
	*/
	for (int i = 0; i < pathVectors.size(); i++)
	{
		vector3 location = vector3(pathVectors[i].x, pathVectors[i].y + 5, pathVectors[i].z);

		m_pMeshMngr->AddPlaneToRenderList(glm::rotate(IDENTITY_M4, glm::radians(90.0f), AXIS_X) * glm::translate(IDENTITY_M4, location), C_RED, RENDER_SOLID);
	}

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release the entity manager
	m_pEntityMngr->ReleaseInstance();

	SafeDelete(pathGraph);
	//SafeDelete(steveEnt);
	//SafeDelete(creeperEnt);

	//release GUI
	ShutdownGUI();
}