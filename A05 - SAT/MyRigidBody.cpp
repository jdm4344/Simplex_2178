#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = true;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	vector3 v3Corner[8];
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);
	
	//if they are colliding check the SAT
	if (bColliding)
	{
		if(SAT(a_pOther) != eSATResults::SAT_NONE)
			bColliding = false;// reset to false
	}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		switch (SAT(a_pOther))
		{
		case 1: SAT_AX;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld * glm::rotate(IDENTITY_M4, glm::radians(90.0f), vector3(0, 1, 0)) * glm::translate(IDENTITY_M4, vector3(0, 0, 1)), C_RED, RENDER_SOLID);
			break;
		case 2: SAT_AY;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld, C_GREEN, RENDER_SOLID);
			break;
		case 3: SAT_AZ;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld, C_BLUE, RENDER_SOLID);
			break;
		case 4: SAT_BX;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld, C_RED, RENDER_SOLID);
			break;
		case 5: SAT_BY;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld, C_GREEN, RENDER_SOLID);
			break;
		case 6: SAT_BZ;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld, C_BLUE, RENDER_SOLID);
			break;
		case 7: SAT_AXxBX;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld, C_RED, RENDER_SOLID);
			break;
		case 8: SAT_AXxBY;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld, C_GREEN, RENDER_SOLID);
			break;
		case 9: SAT_AXxBZ;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld, C_BLUE, RENDER_SOLID);
			break;
		case 10: SAT_AYxBX;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld, C_RED, RENDER_SOLID);
			break;
		case 11: SAT_AYxBY;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld, C_GREEN, RENDER_SOLID);
			break;
		case 12: SAT_AYxBZ;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld, C_BLUE, RENDER_SOLID);
			break;
		case 13: SAT_AZxBX;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld, C_RED, RENDER_SOLID);
			break;
		case 14: SAT_AZxBY;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld, C_GREEN, RENDER_SOLID);
			break;
		case 15: SAT_AZxBZ;
			m_pMeshMngr->AddPlaneToRenderList(this->m_m4ToWorld, C_BLUE, RENDER_SOLID);
			break;
		default:
			break;
		}

		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}

	return bColliding;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{
	float ra, rb;
	matrix3 R, AbsR;
	vector3 myHalf = m_v3HalfWidth;
	vector3 otherHalf = a_pOther->m_v3HalfWidth;
	matrix3 axes = matrix3(1, 0, 0, 0, 1, 0, 0, 0, 1);
	//matrix3 axes = this->m_m4ToWorld * a_pOther->m_m4ToWorld;
	

	// Determine rotation matrix of other relative to this
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			// get dot product of local position vectors
			R[i][j] = glm::dot(this->m_m4ToWorld[i], a_pOther->m_m4ToWorld[j]); 
			//R[i][j] = glm::dot(this->m_v3Center * axes[i], a_pOther->m_v3Center * axes[j]);
			//R[i][j] = glm::dot(this->GetCenterGlobal() * axes[i], a_pOther->GetCenterGlobal() * axes[j]);
			//R[i][j] = glm::dot(axes[i], axes[j]);
			//R[i][j] = glm::dot(this->GetCenterLocal(), a_pOther->GetCenterLocal());

		}
	}

	// Get translation vector
	vector3 translation = a_pOther->GetCenterGlobal() - this->GetCenterGlobal();

	// Bring translation to a's space
	translation = vector3(glm::dot(translation, (vector3)this->m_m4ToWorld[0]), glm::dot(translation, (vector3)this->m_m4ToWorld[1]), glm::dot(translation, (vector3)this->m_m4ToWorld[2]));
	//translation = vector3(glm::dot(translation, this->m_v3Center * axes[0]), glm::dot(translation, this->m_v3Center * axes[2]), glm::dot(translation, this->m_v3Center * axes[2]));
	//translation = vector3(glm::dot(translation, axes[0]), glm::dot(translation, axes[1]), glm::dot(translation, axes[2]));

	// Calculate "subexpressions"
	for (int k = 0; k < 3; k++)
	{
		for (int n = 0; n < 3; n++)
		{
			AbsR[k][n] = glm::abs(R[k][n]) + glm::epsilon<float>();
			//AbsR[k][n] = AbsR[k][n] + glm::epsilon<float>();
		}
	}

	// Test center axes - x, y, z
	// A0, A1, A2
	for (int i = 0; i < 3; i++)
	{
		ra = myHalf[i];
		rb = otherHalf[0] * AbsR[i][0] + otherHalf[1] * AbsR[i][1] + otherHalf[2] * AbsR[i][2];
		if (glm::abs(translation[i]) > ra + rb)
		{
			switch (i)
			{
			case 0:
				return SAT_AX;
			case 1:
				return SAT_AY;
			case 2:
				return SAT_AZ;
			}
		}
	}

	// B0, B1, B2
	for (int i = 0; i < 3; i++)
	{
		ra = myHalf[0] * AbsR[0][i] + myHalf[1] * AbsR[1][i] + myHalf[2] * AbsR[2][i];
		rb = otherHalf[i];
		if (glm::abs(translation[0] * R[0][i] + translation[1] * R[1][i] + translation[2] * R[2][i]) > ra + rb)
		{
			switch (i)
			{
			case 0:
				return SAT_BX;
			case 1:
				return SAT_BY;
			case 2:
				return SAT_BZ;
			}
		}
	}

	// Test perpendicular axes
#pragma region A0 x B0
	ra = myHalf[1] * AbsR[2][0] + myHalf[2] * AbsR[1][0];
	rb = otherHalf[1] * AbsR[0][2] + otherHalf[2] * AbsR[0][1];
	if (glm::abs(translation[2] * R[1][0] - translation[1] * R[2][0] > ra + rb)) return SAT_AXxBX;
#pragma endregion
	
#pragma region A0 x B1
	ra = myHalf[1] * AbsR[2][1] + myHalf[2] * AbsR[1][1];
	rb = otherHalf[0] * AbsR[0][2] + otherHalf[2] * AbsR[0][0];
	if (glm::abs(translation[2] * R[1][1] - translation[1] * R[2][1] > ra + rb)) return SAT_AXxBY;
#pragma endregion

#pragma region A0 x B2
	ra = myHalf[1] * AbsR[2][2] + myHalf[2] * AbsR[1][2];
	rb = otherHalf[0] * AbsR[0][1] + otherHalf[1] * AbsR[0][0];
	if (glm::abs(translation[2] * R[1][2] - translation[1] * R[2][2] > ra + rb)) return SAT_AXxBZ;
#pragma endregion

#pragma region A1 x B0
	ra = myHalf[0] * AbsR[2][0] + myHalf[2] * AbsR[0][0];
	rb = otherHalf[1] * AbsR[1][2] + otherHalf[2] * AbsR[1][1];
	if (glm::abs(translation[0] * R[2][0] - translation[2] * R[0][0] > ra + rb)) return SAT_AYxBX;
#pragma endregion

#pragma region A1 x B1
	ra = myHalf[0] * AbsR[2][1] + myHalf[2] * AbsR[0][1];
	rb = otherHalf[0] * AbsR[1][2] + otherHalf[2] * AbsR[1][0];
	if (glm::abs(translation[0] * R[2][1] - translation[2] * R[0][1] > ra + rb)) return SAT_AYxBY;
#pragma endregion

#pragma region A1 x B2
	ra = myHalf[0] * AbsR[2][2] + myHalf[2] * AbsR[0][2];
	rb = otherHalf[0] * AbsR[1][1] + otherHalf[1] * AbsR[1][0];
	if (glm::abs(translation[0] * R[2][2] - translation[2] * R[0][2] > ra + rb)) return SAT_AYxBZ;
#pragma endregion

#pragma region A2 x B0
	ra = myHalf[0] * AbsR[1][0] + myHalf[1] * AbsR[0][0];
	rb = otherHalf[1] * AbsR[2][2] + otherHalf[2] * AbsR[2][1];
	if (glm::abs(translation[1] * R[0][0] - translation[0] * R[1][0] > ra + rb)) return SAT_AZxBX;
#pragma endregion

#pragma region A2 x B1
	ra = myHalf[0] * AbsR[1][1] + myHalf[1] * AbsR[0][1];
	rb = otherHalf[0] * AbsR[2][2] + otherHalf[2] * AbsR[2][0];
	if (glm::abs(translation[1] * R[0][1] - translation[0] * R[1][1] > ra + rb)) return SAT_AZxBY;
#pragma endregion

#pragma region A2 x B2
	ra = myHalf[0] * AbsR[1][2] + myHalf[1] * AbsR[0][2];
	rb = otherHalf[0] * AbsR[2][1] + otherHalf[1] * AbsR[2][0];
	if (glm::abs(translation[1] * R[0][2] - translation[0] * R[1][2] > ra + rb)) return SAT_AZxBZ;
#pragma endregion

	//there is no axis test that separates this two objects
	return eSATResults::SAT_NONE;
}