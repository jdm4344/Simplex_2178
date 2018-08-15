#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
	m_uEntityCount = 0;
	m_entityList.clear();
}
void MyEntityManager::Release(void)
{
	for (int i = 0; i < m_entityList.size(); i++)
	{
		MyEntity* currEntity = m_entityList[i];
		SafeDelete(currEntity);
	}
	Init();
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	// Search all entities
	for (int i = 0; i < m_entityList.size(); i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return i;
		}
	}
	// Not found
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	// Make sure there are entities and index is not out of bounds
	if (m_entityList.size() > 0 && a_uIndex > -1 && a_uIndex < m_entityList.size())
	{
		return m_entityList[a_uIndex]->GetModel();
	}

	return nullptr;
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	// Search all entities
	for (int i = 0; i < m_entityList.size(); i++)
	{
		// If found, return the model
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return m_entityList[i]->GetModel();
		}
	}

	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	// Make sure there are entities and index is not out of bounds
	if (m_entityList.size() > 0 && a_uIndex > -1 && a_uIndex < m_entityList.size())
	{
		return m_entityList[a_uIndex]->GetRigidBody();
	}

	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	// Search all entities
	for (int i = 0; i < m_entityList.size(); i++)
	{
		// If found, return the rigidbody
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return m_entityList[i]->GetRigidBody();
		}
	}

	return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	// Make sure there are entities and index is not out of bounds
	if (m_entityList.size() > 0 && a_uIndex > -1 && a_uIndex < m_entityList.size())
	{
		return m_entityList[a_uIndex]->GetModelMatrix();
	}

	return IDENTITY_M4;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	// Search all entities
	for (int i = 0; i < m_entityList.size(); i++)
	{
		// If found, return the matrix
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return m_entityList[i]->GetModelMatrix();
		}
	}

	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	for (int i = 0; i < m_entityList.size(); i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			m_entityList[i]->SetModelMatrix(a_m4ToWorld);
		}
	}
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	if (a_uIndex > -1 && a_uIndex < m_entityList.size())
	{
		m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
	}
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	// Collision checking
	for (int i = 0; i < m_uEntityCount; i++)
	{
		for (int  j = i + 1; j < m_uEntityCount; j++)
		{
			m_entityList[i]->IsColliding(m_entityList[j]);
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	// Make temporary entity
	MyEntity* temp = new MyEntity(a_sFileName, a_sUniqueID);
	// Save the entity
	if (temp->IsInitialized())
	{
		m_entityList.push_back(temp);
		m_uEntityCount++;
	}
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	if (m_entityList.size() > 0)
	{
		// Check for out of bounds
		if (a_uIndex >= m_uEntityCount)
		{
			a_uIndex = m_uEntityCount - 1;
		}

		// Make the entity to be the last in the vector
		if (a_uIndex != m_uEntityCount - 1)
		{
			std::swap(m_entityList[a_uIndex], m_entityList[m_uEntityCount - 1]);
		}

		// Now that it's last, remove it
		MyEntity* temp = m_entityList[m_uEntityCount - 1];
		SafeDelete(temp);
		m_entityList.pop_back();
		m_uEntityCount--;
	}
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	// Get the index and use existing method to remove
	uint index = GetEntityIndex(a_sUniqueID);
	RemoveEntity(index);
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	// Make sure there are entities
	if (m_entityList.size() > 0)
	{
		if (a_uIndex >= m_entityList.size())
		{
			return m_entityList[m_entityList.size() - 1]->GetUniqueID();
		}
		return m_entityList[a_uIndex]->GetUniqueID();
	}

	return "";
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	// Make sure there are entities and index is not out of bounds
	if (m_entityList.size() > 0 && a_uIndex < m_entityList.size())
	{
		return m_entityList[a_uIndex];
	}

	return nullptr;
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	// Render all entities if the index given > count
	if (a_uIndex > m_uEntityCount)
	{
		for (int i = 0; i < m_uEntityCount; i++)
		{
			m_entityList[i]->AddToRenderList(a_bRigidBody);
		}
	}
	else // Render just the given entity
	{
		m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
	}
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	// Use existing method to get the entity
	MyEntity* temp = MyEntity::GetEntity(a_sUniqueID);
	if (temp)
	{
		temp->AddToRenderList(a_bRigidBody);
	}
}