#ifndef __MYOCTANTCLASSV2_H_
#define __MYOCTANTCLASSV2_H_

#include "MyEntityManager.h"
#include <stack>

namespace Simplex
{
struct Oct
{
	std::vector<MyEntity*> entities; // MyEntity contains both model and rigidbody
	Mesh* outline; // Mesh for displaying the octant
	int children[8];
	int parent;

	bool active;
	bool hasChildren;
	uint depth;
	float left;
	float right;
	float top;
	float bottom;
	float front;
	float back;
};

class MyOctantV2
{
public:
	MyOctantV2();
	~MyOctantV2();
	// Creats a new octant
	void Init(float left, float right, float top, float bottom, float front, float back, unsigned int maxDepth, unsigned int maxPerNode, Mesh outlineTemplate);
	// Deactivates every octant and then re-adds entities
	void Update();
	// Adds an entity to the entities vector
	void AddEntity(MyEntity* entity);
	// Frees memory
	void Release();
	// Gets all entities in the same octant as the given entitity
	const std::vector<MyEntity*>& GetNearbyEntities(MyEntity* entity);
private:
	Mesh outlineTemplate;
	std::vector<Oct*> octTree;
	std::vector<MyEntity*> entities;
	uint maxDepth;
	uint maxPerNode;

	// Adds a given entity to the space within specified octant
	void AddEntity(MyEntity* entity, int startingOctant);
	// Check if an entity is within an octant
	int CheckEntityOctantCollision(MyEntity* entity, Oct* oct);
	// Initialize the childrent of an octant
	void InitChildren(int index);
	// Initialize a new octant
	Oct* InitOctant(int depth, int parentIndex, int childNum, float left, float right, float top, float bottom, float front, float back);
	// Activates all childrent of a given octant
	void ActivateChildren(Oct* parent);
	// Activate a given octant
	void ActivateOctant(Oct* oct);
	// Deactivate a given octant
	void DeactivateOctant(Oct* oct);
	// Deactivates all octants
	void Reset();
	// Returns the octant index at a given depth
	int GetDepthIndex(int depth);
};
}
#endif