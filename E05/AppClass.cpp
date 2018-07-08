#include "AppClass.h"
// Jordan Machalek
// Section 1
// E05
void Application::InitVariables(void)
{
	//init the mesh
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(1.0f, C_WHITE);
	
	// Generate vectors based on hard-coded array
	for (int i = 0; i < 8; i++) // Row
	{
		for (int  j = 0; j < 11; j++) // Column
		{
			if (invader[i][j] == 1)
			{
				positions.push_back(vector3(j, -i, 0));
			}
		}
	}

	// Generate cubes
	for (int k = 0; k < 46; k++)
	{
		MyMesh* newCube = new MyMesh();
		meshes.push_back(newCube);
		meshes[k]->GenerateCube(1.0f, C_BLACK);
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();

	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(0.75f, 0.75f, 0.75f)); // doesn't change - doesn't need to be called every loop iteration
	matrix4 m4Model;
	matrix4 m4Translate;

	// Update translate depending on which cube is being rendered
	for (int i = 0; i < 46; i++)
	{
		m4Translate = glm::translate(IDENTITY_M4, vector3(positions[i].x + posValue, positions[i].y + 5, 0.0f));

		m4Model = m4Scale * m4Translate;

		meshes[i]->Render(m4Projection, m4View, m4Model); // render current mesh
	}

	// Update the position value
	if (posValue > 5)
	{
		moveDir = false;
	}
	else if (posValue < -5)
	{
		moveDir = true;
	}

	if (moveDir)
	{
		posValue += 0.05f;
	}
	else
	{
		posValue -= 0.05f;
	}
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
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
	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}