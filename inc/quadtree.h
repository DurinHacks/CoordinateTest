#pragma once

#include <vector>
#include <stack>

struct Vec2
{
	float x = 0.0f;
	float y = 0.0f;
};

struct AABB
{
    Vec2 Min;
    Vec2 Max;
};

Vec2 operator+(const Vec2 & lhs, const Vec2 & rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y };
}
Vec2 operator*(const Vec2& lhs, const float& rhs)
{
	return { lhs.x * rhs, lhs.y * rhs };
}

bool AABBIntersectsAABB(const AABB& a, const AABB& b)
{
	return (a.Min.x <= b.Max.x && a.Max.x >= b.Min.x) &&
		(a.Min.y <= b.Max.y && a.Max.y >= b.Min.y);
}

struct MapCoordinate
{
	struct
	{
		float x = 0;
		float y = 0;
	} Grid;
	Vec2 Pos;

    AABB GetBounds(void) const
    {
        AABB Res{ };
		Res.Min.x = Grid.x;
		Res.Min.y = Grid.y;
		Res.Max.x = (Grid.x + 1);
		Res.Max.y = (Grid.y + 1);
		return Res;
    }
};
bool operator==(const MapCoordinate& lhs, const MapCoordinate& rhs)
{
    return (lhs.Grid.x == rhs.Grid.x) &&
        (lhs.Grid.y == rhs.Grid.y) &&
        (lhs.Pos.x == rhs.Pos.x) &&
        (lhs.Pos.y == rhs.Pos.y);
}

struct QuadTreeNode
{
    public: AABB Bounds;  // Axis-aligned bounding box representing the node's spatial extent
    public: std::vector<MapCoordinate> Transforms;  // Transforms stored in this node
    public: QuadTreeNode* Children[4];  // Pointers to the four children nodes (if any)

    // Constructor
    QuadTreeNode(const AABB& NodeBounds) : Bounds(NodeBounds) {
        for (int i = 0; i < 4; ++i) {
            Children[i] = nullptr;
        }
    }
};

#define SETPTR(CLASS, ADDRESS) reinterpret_cast<CLASS*>(ADDRESS);

class Quadtree
{
    public: Quadtree(const AABB& RootBounds, int Depth) : MaxDepth(Depth)
    {
        this->Root = new QuadTreeNode(RootBounds);
    }
    public: ~Quadtree(void)
    {
        Clear();
        delete this->Root;
        this->Root = SETPTR(QuadTreeNode, 0xDEADBABEDEADBABE);
    }

    public: void Insert(const MapCoordinate& Trfo)
    {
        QuadTreeNode* Node = Root;
        AABB NodeBounds = Root->Bounds;

        for (int depth = 0; depth < MaxDepth; ++depth)
        {
            if (Node->Children[0] == nullptr)
            {
                SubdivideNode(Node);
            }

            bool inserted = false;
            for (int i = 0; i < 4; ++i)
            {
                if (AABBIntersectsAABB(Node->Children[i]->Bounds, Trfo.GetBounds()))
                {
                    Node = Node->Children[i];
                    NodeBounds = Node->Bounds;
                    inserted = true;
                    break;
                }
            }

            if (!inserted)
            {
                Node->Transforms.push_back(Trfo);
                return;
            }
        }

        Node->Transforms.push_back(Trfo);
    }
    public: void Clear(void)
    {
        std::stack<QuadTreeNode*> nodeStack;
        nodeStack.push(this->Root);

        while (!nodeStack.empty())
        {
            QuadTreeNode* node = nodeStack.top();
            nodeStack.pop();

            for (int i = 0; i < 4; ++i)
            {
                if (node->Children[i] != nullptr)
                {
                    nodeStack.push(node->Children[i]);
                }
            }

            delete node;
        }

        this->Root = new QuadTreeNode(this->Root->Bounds);
    }
    public: void CollectCoords(const AABB& QueryBounds, std::vector<const MapCoordinate*>& OutTrfos)
    {
        std::stack<QuadTreeNode*> NodeStack;
        NodeStack.push(this->Root);

        while (!NodeStack.empty())
        {
            QuadTreeNode* Node = NodeStack.top();
            NodeStack.pop();

            if (!AABBIntersectsAABB(Node->Bounds, QueryBounds))
                continue;

            for (const MapCoordinate& Trfo : Node->Transforms)
                if (AABBIntersectsAABB(Trfo.GetBounds(), QueryBounds))
                    OutTrfos.push_back(&Trfo);

            for (int i = 0; i < 4; ++i)
                if (Node->Children[i] != nullptr)
                    NodeStack.push(Node->Children[i]);
        }
    }
    public: bool RemoveNode(const MapCoordinate& Trfo) {
        std::stack<QuadTreeNode*> NodeStack;
        NodeStack.push(this->Root);

        while (!NodeStack.empty())
        {
            QuadTreeNode* node = NodeStack.top();
            NodeStack.pop();

            // Check if the node contains the transform
            auto it = std::find(node->Transforms.begin(), node->Transforms.end(), Trfo);
            if (it != node->Transforms.end())
            {
                node->Transforms.erase(it);
                return true;
            }

            // Check if the transform can potentially exist in the children nodes
            if (AABBIntersectsAABB(node->Bounds, Trfo.GetBounds()))
            {
                for (int i = 0; i < 4; ++i)
                    if (node->Children[i] != nullptr)
                        NodeStack.push(node->Children[i]);
            }
        }

        return false;
    }

    private: QuadTreeNode* Root = nullptr;
    private: int MaxDepth = 1;

    private: void SubdivideNode(QuadTreeNode* Node)
    {
        const Vec2 Min = Node->Bounds.Min;
        const Vec2 Max = Node->Bounds.Max;
        const Vec2 Center = (Min + Max) * 0.5f;

        for (int i = 0; i < 4; ++i)
        {
            const Vec2 ChildMin(
                i & 1 ? Center.x : Min.x,
                i & 2 ? Center.y : Min.y
            );
            const Vec2 ChildMax(
                i & 1 ? Max.x : Center.x,
                i & 2 ? Max.y : Center.y
            );
            Node->Children[i] = new QuadTreeNode(AABB(ChildMin, ChildMax));
        }
    }
};
