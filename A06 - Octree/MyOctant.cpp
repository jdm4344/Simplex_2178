#include "MyOctant.h"
using namespace Simplex;
// MyOctant Generation
void MyOctant::Init(void)
{
	// Reset *almost* everything
	m_uID = m_uOctantCount;
	m_uLevel = 0;
	m_uChildren = 0;
	m_fSize = 0;
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_v3Center = vector3(0);
	m_v3Min = vector3(0);
	m_v3Max = vector3(0);
	m_pParent = nullptr;
	for (int i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
	m_pRoot = nullptr;
}
void MyOctant::Swap(MyOctant & other)
{
	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_uChildren, other.m_uChildren);
	std::swap(m_fSize, other.m_fSize);
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);
	std::swap(m_pParent, other.m_pParent);
	std::swap(m_lChild, other.m_lChild);
	for (int i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
	std::swap(m_pRoot, other.m_pRoot);
}
void MyOctant::Release(void)
{

}
// Rule of 3
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();
	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;
	m_pRoot = this;
	m_lChild.clear();
}
MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
}
MyOctant::~MyOctant(void)
{
}
// Accessors
float MyOctant::GetSize(void)
{
	return 0.0f;
}
vector3 MyOctant::GetCenterGlobal(void)
{
	return vector3();
}
vector3 MyOctant::GetMinGlobal(void)
{
	return vector3();
}
vector3 MyOctant::GetMaxGlobal(void)
{
	return vector3();
}
MyOctant* MyOctant::GetChild(uint a_nChild)
{
	return nullptr;
}
MyOctant* MyOctant::GetParent(void)
{
	return m_pParent;
}
// Methods
bool MyOctant::IsColliding(uint a_uRBIndex)
{
	return false;
}
void MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
}
void MyOctant::Display(vector3 a_v3Color)
{
}
void MyOctant::DisplayLeafs(vector3 a_v3Color)
{
}
void MyOctant::ClearEntityList(void)
{
}
void MyOctant::Subdivide(void)
{
}
bool MyOctant::IsLeaf(void)
{
	return false;
}
bool MyOctant::ContainsMoreThan(uint a_nEntities)
{
	return false;
}
void MyOctant::KillBranches(void)
{
}
void MyOctant::ConstructTree(uint a_nMaxLevel)
{
}
void MyOctant::AssignIDtoEntity(void)
{
}
uint MyOctant::GetOctantCount(void)
{
	return uint();
}
void MyOctant::ConstructList(void)
{
}
