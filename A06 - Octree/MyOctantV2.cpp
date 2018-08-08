#include "MyOctantV2.h"
using namespace Simplex;

// Public
MyOctantV2::MyOctantV2()
{
	std::vector<Oct*> octTree = std::vector<Oct*>();
	std::vector<MyEntity*> _shapes = std::vector<MyEntity*>();
	maxDepth = 0;
	maxPerNode = 0;
}
MyOctantV2::~MyOctantV2()
{
	Release();
}
void MyOctantV2::Init(float left, float right, float top, float bottom, float front, float back, unsigned int maxDepth, unsigned int maxPerNode, Mesh outlineTemplate)
{
	this->maxPerNode = maxPerNode;
	this->maxDepth = maxDepth;
	this->outlineTemplate = outlineTemplate;
	octTree.resize(GetDepthIndex(this->maxDepth));
	octTree[0] = InitOctant(0, 0, 0, left, right, top, bottom, front, back);
	
	for (int i = 0; i < GetDepthIndex(this->maxDepth - 1); i++)
	{
		InitChildren(i);
	}
}
void MyOctantV2::Update()
{
	// Deactivate nodes
	Reset();
	// Reactivate root
	ActivateOctant(octTree[0]);
	// Re-add entities
	for (int i = 0; i < entities.size(); i++)
	{
		AddEntity(entities[i], 0);
	}
}
void MyOctantV2::AddEntity(MyEntity* entity)
{
	entities.push_back(entity);
}
void MyOctantV2::Release()
{
	int i;

	while ((i = octTree.size()) > 0)
	{
		delete octTree[i - 1];
		octTree.pop_back();
	}
}
const std::vector<MyEntity*>& MyOctantV2::GetNearbyEntities(MyEntity* entity)
{
	uint treeSize = octTree.size();

	for (int i = 0; i < treeSize;)
	{
		Oct* currentOctant = octTree[i];
		int result = CheckEntityOctantCollision(entity, currentOctant);

		// Not colliding
		if (result == 0)
		{
			++i;
			continue;
		}
		
		// Partially colliding
		if (result == 1)
		{
			if (currentOctant->depth != 0)
			{
				return octTree[currentOctant->parent]->entities;
			}
			else
			{
				result = 2;
			}
		}

		// Fully colliding
		if(result == 2) 
		{
			if (!currentOctant->hasChildren)
			{
				return currentOctant->entities;
			}
			else
			{
				i = currentOctant->children[0];
			}
		}
	}
}
// Private
void MyOctantV2::AddEntity(MyEntity* entity, int startingOctant)
{
	for (int i = 0; i < octTree.size();)
	{
		Oct* currentOctant = octTree[i];
		int result = CheckEntityOctantCollision(entity, currentOctant);

		// Not colliding
		if (result == 0)
		{
			++i;
			continue;
		}
		
		// Partially colliding - add to the octant's parent
		if (result == 1)
		{
			if (currentOctant->depth != 0)
			{
				octTree[currentOctant->parent]->entities.push_back(entity);
				break;
			}
			else
			{
				result = 2;
			}
		}

		// Fully colliding - add to the current node
		if(result == 2)
		{
			if (currentOctant->entities.size() >= maxPerNode && currentOctant->depth < maxDepth)
			{
				if (!currentOctant->hasChildren)
				{
					ActivateChildren(currentOctant);
					// Add all entities in the current octant as children
					std::vector<MyEntity*> temp = currentOctant->entities;
					currentOctant->entities.clear();

					for (int j = 0; j < temp.size(); j++)
					{
						AddEntity(temp[j], currentOctant->children[0]);
					}
				}
				i = currentOctant->children[0];
			}
			else
			{
				if (!currentOctant->hasChildren)
				{
					currentOctant->entities.push_back(entity);
				}
				else
				{
					i = currentOctant->children[0];
				}
			}
		}
	}
}
int MyOctantV2::CheckEntityOctantCollision(MyEntity* entity, Oct* oct)
{
	// AABB collision check between entity and the current octant
	int colStatus = 0;
	MyRigidBody* col = entity->GetRigidBody();
	vector3 center = col->GetCenterGlobal();
	vector3 size = col->GetARBBSize();

	float dLeft = oct->left - (center.x - size.x / 2.0f);
	float dRight = oct->right - (center.x + size.x / 2.0f);

	float dTop =oct->top - (center.y + size.y / 2.0f);
	float dBot =oct->bottom - (center.y - size.y / 2.0f);

	float front =oct->front - (center.z - size.z / 2.0f);
	float back =oct->back - (center.z + size.z / 2.0f);

	float width =oct->right -oct->left;
	float height =oct->top -oct->bottom;
	float depth =oct->front -oct->back;

	colStatus += abs(dTop) < height && abs(dBot) < height && abs(dRight) < width && abs(dLeft) < width && abs(front) < depth && abs(back) < depth;
	colStatus += dTop > 0 && dBot < 0 && dRight > 0 && dLeft < 0 && front > 0 && back < 0;
	return colStatus;
}
void MyOctantV2::InitChildren(int index)
{
	// Create a new octant
	Oct* oct = octTree[index];
	float midX = oct->left + ((oct->right - oct->left) / 2.0f);
	float midY = oct->bottom + ((oct->top - oct->bottom) / 2.0f);
	float midZ = oct->back + ((oct->front - oct->back) / 2.0f);
	int childNum = oct->children[0] - GetDepthIndex(oct->depth);
	// Initialize its children
	octTree[oct->children[0]] = InitOctant(oct->depth + 1, index, childNum, oct->left, midX, oct->top, midY, oct->front, midZ);
	octTree[oct->children[1]] = InitOctant(oct->depth + 1, index, childNum + 1, midX, oct->right, oct->top, midY, oct->front, midZ);
	octTree[oct->children[2]] = InitOctant(oct->depth + 1, index, childNum + 2, oct->left, midX, midY, oct->bottom, oct->front, midZ);
	octTree[oct->children[3]] = InitOctant(oct->depth + 1, index, childNum + 3, midX, oct->right, midY, oct->bottom, oct->front, midZ);
	octTree[oct->children[4]] = InitOctant(oct->depth + 1, index, childNum + 4, oct->left, midX, oct->top, midY, midZ, oct->back);
	octTree[oct->children[5]] = InitOctant(oct->depth + 1, index, childNum + 5, midX, oct->right, oct->top, midY, midZ, oct->back);
	octTree[oct->children[6]] = InitOctant(oct->depth + 1, index, childNum + 6, oct->left, midX, midY, oct->bottom, midZ, oct->back);
	octTree[oct->children[7]] = InitOctant(oct->depth + 1, index, childNum + 7, midX, oct->right, midY, oct->bottom, midZ, oct->back);
}
Oct* MyOctantV2::InitOctant(int depth, int parentIndex, int childNum, float left, float right, float top, float bottom, float front, float back)
{
	// Create the octant
	Oct* oct = new Oct();
	oct->active = false;
	oct->hasChildren = false;
	oct->depth = depth;
	oct->left = left;
	oct->right = right;
	oct->top = top;
	oct->bottom = bottom;
	oct->front = front;
	oct->back = back;

	// Create a cube outline to display the octree
	Mesh* m = new Mesh();
	m->GenerateWireCube(1, C_YELLOW);
	oct->outline = m;

	int base = GetDepthIndex(depth) + childNum * 8;

	oct->children[0] = base;
	oct->children[1] = base + 1;
	oct->children[2] = base + 2;
	oct->children[3] = base + 3;
	oct->children[4] = base + 4;
	oct->children[5] = base + 5;
	oct->children[6] = base + 6;
	oct->children[7] = base + 7;
	oct->parent = parentIndex;

	/* // Need to set the position of the outline somehow
	// Example:
	node->outline->transform().position.x = (node->left + node->right) / 2.0f;
	node->outline->transform().position.y = (node->top + node->bottom) / 2.0f;
	node->outline->transform().position.z = (node->front + node->back) / 2.0f;
	node->outline->transform().scale.x = (node->right - node->left) / 2.0f;
	node->outline->transform().scale.y = (node->top - node->bottom) / 2.0f;
	node->outline->transform().scale.z = (node->front - node->back) / 2.0f;
	*/

	return oct;
}
void MyOctantV2::ActivateChildren(Oct* parent)
{
	parent->hasChildren = true;
	ActivateOctant(octTree[parent->children[0]]);
	ActivateOctant(octTree[parent->children[1]]);
	ActivateOctant(octTree[parent->children[2]]);
	ActivateOctant(octTree[parent->children[3]]);
	ActivateOctant(octTree[parent->children[4]]);
	ActivateOctant(octTree[parent->children[5]]);
	ActivateOctant(octTree[parent->children[6]]);
	ActivateOctant(octTree[parent->children[7]]);
}
void MyOctantV2::ActivateOctant(Oct* oct)
{
	oct->active = true;
	// TODO: disable the octant's mesh
	oct->hasChildren = false;
}
void MyOctantV2::DeactivateOctant(Oct* oct)
{
	oct->active = false;
	// TODO: enable the octant's mesh
	oct->hasChildren = false;
}
void MyOctantV2::Reset()
{
	std::stack<int> stack;
	stack.push(0);

	while (!stack.empty())
	{
		Oct* oct = octTree[stack.top()];
		stack.pop();

		if (oct->active)
		{
			oct->active = false;
			oct->entities.clear();
			
			// TODO: disable the octant's mesh

			if (oct->hasChildren)
			{
				stack.push(oct->children[0]);
				stack.push(oct->children[1]);
				stack.push(oct->children[2]);
				stack.push(oct->children[3]);
				stack.push(oct->children[4]);
				stack.push(oct->children[5]);
				stack.push(oct->children[6]);
				stack.push(oct->children[7]);
			}
		}
	}
}
int MyOctantV2::GetDepthIndex(int depth)
{
	float dep = (float)depth;
	float ret = 0.0f;

	for (float i = 0; i < depth || i == depth; i += 1.0f)
	{
		ret += powf(8.0f, i);
	}

	return (int)ret;
}
