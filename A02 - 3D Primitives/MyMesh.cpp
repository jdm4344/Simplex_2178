#include "MyMesh.h"
// Jordan Machalek
// Section 1
// A02
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	vector3 center = vector3(0, 0, 0); // center of base
	vector3 pinnacle = vector3(0, 0, a_fHeight); // point of the cone
	vector3* prevPoint = nullptr; // last point generated - to be used for 'first' point of next tri
	vector3* points = new vector3[a_nSubdivisions + 2]; // all points generated - # of subDiv + center + pinnacle
	points[0] = center; // assign center
	points[1] = pinnacle; // assign pinnacle

	// Generate base
	for (int i = 0; i < a_nSubdivisions + 1; i++)
	{
		// Get current angle
		float angle = i * (2.0f * 3.14f / a_nSubdivisions);

		// Get coordinates of vertices
		float x = a_fRadius * std::sin(angle);
		float y = a_fRadius * std::cos(angle);

		points[i] = vector3(x, y, 0);

		// Make the new Tri
		if (prevPoint != nullptr) // Make sure there are enough points for a full tri
		{
			AddTri(points[i], vector3(prevPoint->x, prevPoint->y, 0), center);
			AddTri(points[i], pinnacle, vector3(prevPoint->x, prevPoint->y, 0));
			delete prevPoint;

			prevPoint = new vector3(x, y, 0);
		}
		else if (prevPoint == nullptr) // Wait to get a third point
		{
			prevPoint = new vector3(x, y, 0);
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();

	// Cleanup
	delete[] points;
	delete prevPoint;
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	vector3 center1 = vector3(0, 0, a_fHeight / 2); // center of end
	vector3 center2 = vector3(0, 0, -a_fHeight / 2);
	vector3* prevPoint1 = nullptr; // last point generated
	vector3* prevPoint2 = nullptr;
	vector3* points1 = new vector3[a_nSubdivisions + 1]; // all points generated for 1 end - # of subDiv + center
	vector3* points2 = new vector3[a_nSubdivisions + 1]; 

	// Generate top and bottom
	for (size_t j = 0; j < 2; j++)
	{
		points1[0] = center1; // assign top center
		points1[1] = center2; // assign bottom center 
	
		for (int i = 0; i < a_nSubdivisions + 1; i++)
		{
			// Get current angle
			float angle = i * (2.0f * 3.14f / a_nSubdivisions);

			// Get coordinates of vertices
			float x = a_fRadius * std::sin(angle);
			float y = a_fRadius * std::cos(angle);

			if (j == 1) // top
			{
				points1[i] = vector3(x, y, a_fHeight / 2);
			}
			else // bottom
			{
				points2[i] = vector3(x, y, -a_fHeight / 2);
			}

			// Make the new Tri
			if (prevPoint1 != nullptr) // Make sure there are enough points for a full tri
			{
				if (j == 1) // top
				{
					AddTri(points1[i], vector3(prevPoint1->x, prevPoint1->y, a_fHeight / 2), center1);
					delete prevPoint1;
					prevPoint1 = new vector3(x, y, a_fHeight / 2);
				}
				else // bottom
				{
					AddTri(vector3(prevPoint1->x, prevPoint1->y, -a_fHeight / 2), points2[i], center2); // swap prevPoint and points2[i] so face is outwards
					delete prevPoint1;
					prevPoint1 = new vector3(x, y, -a_fHeight / 2);
				}
			}
			else if (prevPoint1 == nullptr) // Wait to get a third point
			{
				if (j == 1) // top
				{
					prevPoint1 = new vector3(x, y, a_fHeight / 2);
				}
				else // bottom
				{
					prevPoint1 = new vector3(x, y, -a_fHeight / 2);
				}
			}
		}

		// Cleanup
		delete prevPoint1;
		prevPoint1 = nullptr;
		delete prevPoint2;
		prevPoint2 = nullptr;
	}

	// Create sides
	for (size_t k = 0; k < a_nSubdivisions + 1; k++)
	{
		if (prevPoint1 != nullptr && prevPoint2 != nullptr)
		{
			AddQuad(*prevPoint1, points1[k], *prevPoint2, points2[k]);
			prevPoint1 = &points1[k];
			prevPoint2 = &points2[k];
		}
		else if (prevPoint1 == nullptr || prevPoint2 == nullptr)
		{
			AddQuad(points1[k], points1[k + 1], points2[k], points2[k + 1]);
			prevPoint1 = &points1[k + 1];
			prevPoint2 = &points2[k + 1];
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();

	// Cleanup
	delete[] points1;
	delete[] points2;
	//delete prevPoint1; // Deleting these causes exception - b/c points1&2 are deleted?
	//delete prevPoint2;
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	vector3* prevInner = nullptr; // last point generated
	vector3* prevOuter = nullptr;
	vector3* innerTopPoints = new vector3[a_nSubdivisions + 1]; // all points generated for 1 end - # of subDiv + center
	vector3* outerTopPoints = new vector3[a_nSubdivisions + 1];
	vector3* innerBottomPoints = new vector3[a_nSubdivisions + 1]; // all points generated for 1 end - # of subDiv + center
	vector3* outerBottomPoints = new vector3[a_nSubdivisions + 1];

	// Generate top and bottom
	for (size_t j = 0; j < 2; j++)
	{
		for (int i = 0; i < a_nSubdivisions + 1; i++)
			{
				// Get current angle
				float angle = i * (2.0f * 3.14f / a_nSubdivisions);

				// Get coordinates of vertices
				float xInner = a_fInnerRadius * std::sin(angle);
				float yInner = a_fInnerRadius * std::cos(angle);

				float xOuter = a_fOuterRadius * std::sin(angle);
				float yOuter = a_fOuterRadius * std::cos(angle);

				if (j == 1) // top
				{
					innerTopPoints[i] = vector3(xInner, yInner, a_fHeight / 2);
					outerTopPoints[i] = vector3(xOuter, yOuter, a_fHeight / 2);
				}
				else // bottom
				{
					innerBottomPoints[i] = vector3(xInner, yInner, -a_fHeight / 2);
					outerBottomPoints[i] = vector3(xOuter, yOuter, -a_fHeight / 2);
				}

				// Make the new Quad
				if (prevInner != nullptr && prevOuter != nullptr) // Make sure there are enough points for a full quad
				{
					if (j == 1) // top
					{\
						AddQuad(innerTopPoints[i], outerTopPoints[i], *prevInner, *prevOuter);
						delete prevInner;
						delete prevOuter;
						prevInner = new vector3(xInner, yInner, a_fHeight / 2);
						prevOuter = new vector3(xOuter, yOuter, a_fHeight / 2);
					}
					else // bottom
					{
						AddQuad(*prevInner, *prevOuter, innerBottomPoints[i], outerBottomPoints[i]);
						delete prevInner;
						delete prevOuter;
						prevInner = new vector3(xInner, yInner, -a_fHeight / 2);
						prevOuter = new vector3(xOuter, yOuter, -a_fHeight / 2);
					}
				}
				else if (prevInner == nullptr && prevOuter == nullptr) // Wait to get a second set of points
				{
					if (j == 1) // top
					{
						prevInner = new vector3(xInner, yInner, a_fHeight / 2);
						prevOuter = new vector3(xOuter, yOuter, a_fHeight / 2);
					}
					else // bottom
					{
						prevInner = new vector3(xInner, yInner, -a_fHeight / 2);
						prevOuter = new vector3(xOuter, yOuter, -a_fHeight / 2);
					}
				}
		}
		
		// Cleanup
		delete prevInner;
		prevInner = nullptr;
		delete prevOuter;
		prevOuter = nullptr;
	}

	vector3* prevTop = nullptr;
	vector3* prevBottom = nullptr;

	// Create sides
	for (int n = 0; n < 2; n++)
	{
		if (n == 1) // inner
		{
			for (size_t k = 0; k < a_nSubdivisions + 1; k++)
			{
				if (prevTop != nullptr && prevBottom != nullptr) // make sure points are assigned
				{
					AddQuad(*prevBottom, innerBottomPoints[k], *prevTop, innerTopPoints[k]);
					// advance previous
					prevTop = &innerTopPoints[k];
					prevBottom = &innerBottomPoints[k];
				}
				else if (prevTop == nullptr || prevBottom == nullptr)
				{
					AddQuad(innerBottomPoints[k], innerTopPoints[k], innerBottomPoints[k + 1], innerTopPoints[k + 1]);
					// set previous
					prevTop = &innerTopPoints[k + 1];
					prevBottom = &innerBottomPoints[k + 1];
				}
			}
		}
		else // outer
		{
			for (size_t k = 0; k < a_nSubdivisions + 1; k++)
			{
				if (prevTop != nullptr && prevBottom != nullptr) // make sure points are assigned
				{
					AddQuad(*prevTop, outerTopPoints[k], *prevBottom, outerBottomPoints[k]);
					// advance previous
					prevTop = &outerTopPoints[k];
					prevBottom = &outerBottomPoints[k];
				}
				else if (prevTop == nullptr || prevBottom == nullptr)
				{
					AddQuad(outerTopPoints[k], outerTopPoints[k + 1], outerBottomPoints[k], outerBottomPoints[k + 1]);
					// set previous
					prevTop = &outerTopPoints[k + 1];
					prevBottom = &outerBottomPoints[k + 1];
				}
			}
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();

	// Clean up
	delete prevInner; 
	delete prevOuter;
	//delete prevTop;
	//delete prevBottom;
	delete[] innerTopPoints;
	delete[] outerTopPoints;
	delete[] innerBottomPoints;
	delete[] outerBottomPoints;
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// a_nSubdivisionsA - horizontal rings
	// a_nSubdivisionsB - vertical rings

	// Attempted to follow explanation of point calculation for a torus given in the following forum response:
	// https://gamedev.stackexchange.com/a/16850

	vector3 center = vector3(0, 0, 0); // center of the torus
	std::vector<std::vector<vector3>> allRingsA; // vector of vectors containing all points in a ring
	std::vector<std::vector<vector3>> allRingsB; // vector of vectors containing all points in a ring
	float radiusDiff = a_fOuterRadius - a_fInnerRadius; // distance between inner and outer radii
	float increment = radiusDiff / a_nSubdivisionsA;

	// Create ring of vectors between inner and outer radii
	std::vector<vector3> centerRing;

	for (int i = 0; i < a_nSubdivisionsA + 1; i++)
	{
		// Get current angle
		float angle = i * (2.0f * 3.14f / a_nSubdivisionsB);

		// Get coordinates of vertices
		float x = (a_fInnerRadius + radiusDiff) * std::sin(angle);
		float y = (a_fInnerRadius + radiusDiff) * std::cos(angle);

		centerRing.push_back(vector3(x, y, 0));
	}

	// create ring around each of the points along centerRing
	for (int j = 0; j < a_nSubdivisionsB + 1; j++)
	{
		vector3 currentVector = centerRing[j];

		// Get current angle
		float angle = j * (2.0f * 3.14f / a_nSubdivisionsB);

		
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	vector3* prevPoint = nullptr; // last point generated
	vector3* currPoints = new vector3[a_nSubdivisions + 1];
	vector3* prevPoints = new vector3[a_nSubdivisions + 1];
	std::vector<std::vector<vector3>> points;
	float height = -a_fRadius;
	vector3 center = vector3(0, 0, height);

	// Generate quads for latitudes
#pragma region Attempt 3
	/*

	float hRadius = a_fRadius / a_nSubdivisions; // horizontal radius
	float radIncrement = a_fRadius / a_nSubdivisions;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		std::vector<vector3> ring;

		// Create vertices for current ring
		for (int j = 0; j < a_nSubdivisions + 1; j++)
		{
			// Get current angle
			float angle = j * (2.0f * 3.14f / a_nSubdivisions);

			// Get coordinates of vertices
			float x = a_fRadius * std::sin(angle);
			float y = a_fRadius * std::cos(angle);

			ring.push_back(vector3(x, y, height));
		}

		points.push_back(ring);
		
		// increment height
		height += (radIncrement * radIncrement);
		// Increment or decrement the horizontal radius exponentially
		if (height < 0) // grow hRadius until equator
		{
			radIncrement = (radIncrement * radIncrement);
			hRadius += radIncrement;
		}
		else if (height == 0) // reset radIncrement when at equator
		{
			radIncrement = a_fRadius / a_nSubdivisions;
		}
		else if (height > 0) // shrink hRadius above equator
		{
			radIncrement = (radIncrement * radIncrement);
			hRadius -= radIncrement;
		}
	}

	std::vector<vector3> topRing;
	std::vector<vector3> bottomRing;

	for (int k = 0; k < a_nSubdivisions - 1; k++)
	{
		if (k < a_nSubdivisions)
		{
			topRing = points[k + 1];
			bottomRing = points[k];
		}
		else
		{
			continue;
		}

		for (int n = 0; n < a_nSubdivisions; n++)
		{
			AddQuad(topRing[n], topRing[n + 1], bottomRing[n], bottomRing[n + 1]);
		}
	}
	*/
#pragma endregion

#pragma region Attempt 2
	float radModifier = a_fRadius / a_nSubdivisions;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		std::vector<vector3> ring;

		for (int j = 0; j < a_nSubdivisions + 1; j++)
		{
			// Get current angle
			float angle = j * (2.0f * 3.14f / a_nSubdivisions);

			// Get coordinates of vertices
			float x = (a_fRadius) * std::sin(angle);
			float y = (a_fRadius) * std::cos(angle);

			ring.push_back(vector3(x, y, height));
		}

		height += a_fRadius / a_nSubdivisions; // increment height of next level
		if (height < 0) // grow the horizontal and lateral radius
		{
			radModifier += a_fRadius / a_nSubdivisions;
		}
		else if (height > 0) // shrink the horizontal and lateral radius
		{
			radModifier -= a_fRadius / a_nSubdivisions;
		}

		points.push_back(ring);

		if (i >= 1)
		{
			std::vector<vector3> bottom = points[i - 1];
			std::vector<vector3> top = points[i];

			for (int n = 0; n < a_nSubdivisions; n++)
			{
				AddQuad(top[n], bottom[n], top[n + 1], bottom[n + 1]);
			}
		}
	}

	// Create end caps
	std::vector<vector3> ring;
	for (int l = 0; l < 2; l++)
	{
		if (l == 0)
		{
			ring = points[0];
		}
		else
		{
			ring = points[a_nSubdivisions - 1];
		}

		for (int m = 0; m < a_nSubdivisions; m++)
		{
			if (l == 0)
			{
				AddTri( ring[m + 1], ring[m], vector3(0, 0, -a_fRadius));
			}
			else
			{
				AddTri(vector3(0, 0, a_fRadius), ring[m], ring[m + 1] );
			}
		}
	}
	
#pragma endregion

#pragma region Attempt 1
	/*
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		currPoints = new vector3[a_nSubdivisions + 1];

		for (int j = 0; j < a_nSubdivisions + 1; j++)
		{
			// Get current angle
			float angle = j * (2.0f * 3.14f / a_nSubdivisions);

			// Get coordinates of vertices
			float x = a_fRadius * std::sin(angle);
			float y = a_fRadius * std::cos(angle);

			currPoints[j] = vector3(x, y, height);
		}

		// Generate tris for caps - top and bottom
		if (height == -a_fRadius || height == a_fRadius)
		{
			for (int k = 0; k < a_nSubdivisions + 1; k++)
			{
				// Make the new Tri
				if (prevPoint != nullptr) // Make sure there are enough points for a full tri
				{
					if (height == -a_fRadius) // Bottom
					{
						AddTri(currPoints[k], vector3(prevPoint->x, prevPoint->y, height), center);
					}
					else if(height == a_fRadius) // Top
					{
						AddTri(center, vector3(prevPoint->x, prevPoint->y, height), currPoints[k]);
					}

					prevPoint = &currPoints[k];
				}
				else if (prevPoint == nullptr) // Wait to get a third point
				{
					prevPoint = &currPoints[k];
				}
			}
		}
		
		else if (i >= 1)
		{
			for (int n = 0; n < a_nSubdivisions; n++)
			{
				AddQuad(currPoints[n], prevPoints[n], currPoints[n + 1], prevPoints[n + 1]);
			}
		}

		// After a ring is made, increment the height
		height += a_fRadius / a_nSubdivisions;

		// Copy currPoints to prevPoints
		prevPoints = currPoints;
		delete[] currPoints;
		currPoints = new vector3[a_nSubdivisions + 1];
	}
	*/
#pragma endregion

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();

	// Cleanup
	//delete prevPoint;
	delete[] currPoints;
	//delete[] prevPoints;
}