#pragma once

#include <vector>
#include <algorithm>
#include <stack>

struct Rect
{
	float x, y, width, height;
	constexpr bool intersects(const Rect& other)const noexcept
	{
		if (x > other.x + other.width || x + width < other.x)
		{
			return false;
		}
		if (y + height < other.y || y > other.y + other.height)
		{
			return false;
		}
		return true; 
	}
};

template<class T>
struct Entry
{
	Rect bound{};
	T obj{};
};

template<class T>
struct QuadNode
{
	Rect bound{};
	int parent = -1;
	int children = -1;
	std::vector<Entry<T>> objects{};
};

template<class T>
class QuadTree
{
public:
	void initialize(int maxLevel, size_t maxObjects, Rect bound)noexcept;
	void insert(Rect bound, T&& obj)noexcept;

	template<class PairCollisionPred, class PairCollisionFunc>
	void each_collision_if(PairCollisionPred isColliding, PairCollisionFunc onCollision)noexcept;

	template<class PairCollisionFunc>
	void each_collision(PairCollisionFunc onCollision)noexcept;

	template<class Collider, class OnCollisionFunc>
	void each_collision(Collider collider, OnCollisionFunc func)noexcept;

	template<class Pred>
	void remove_if(Pred pred);

	void reserve(size_t size)noexcept { m_Nodes.reserve(size); }
	void clear()noexcept { m_Nodes.clear(); }
	void shrinkToFit()noexcept;

private:
	void split(int index, QuadNode<T>& node)noexcept;

	constexpr static int getIndex(float vLine, float hLine, Rect bound)noexcept;
	constexpr static int NON_CHILD = -1;
	constexpr static int NE = 0;
	constexpr static int NW = 1;
	constexpr static int SE = 2;
	constexpr static int SW = 3;
	constexpr static int COUNT = 4;


	int m_MaxLevel = 0;
	size_t m_MaxObjects = 0;
	std::vector<QuadNode<T>> m_Nodes{};
};

template<class T>
void QuadTree<T>::initialize(int maxLevel, size_t maxObjects, Rect bound)noexcept
{
	m_MaxLevel = maxLevel;
	m_MaxObjects = maxObjects;
	m_Nodes.push_back(QuadNode<T>{ std::move(bound), -1, -1, {} });
}

template<class T>
void QuadTree<T>::insert(Rect bound, T&& obj)noexcept
{
	if (m_Nodes.size() == 0 ||
		!m_Nodes[0].bound.intersects(bound))
	{
		return;
	}
	int level = 0;
	int index = 0;
	while (true)
	{
		auto& node = m_Nodes[index];
		if (node.children == QuadTree::NON_CHILD)
		{
			node.objects.push_back(Entry<T>{bound, std::forward<T>(obj)});
			if (node.objects.size() > m_MaxObjects && level < m_MaxLevel)
			{
				split(index, node);
			}
			return;
		}
		const float verticalLine = node.bound.x + node.bound.width / 2;
		const float horizontalLine = node.bound.y + node.bound.height / 2;
		const int target = getIndex(verticalLine, horizontalLine, bound);
		if (target == QuadTree::NON_CHILD)
		{
			node.objects.push_back(Entry<T>{bound, std::forward<T>(obj)});
			return;
		}
		index = node.children + target;
		++level;
	}
}

template<class T>
template<class PairCollisionPred, class PairCollisionFunc>
void QuadTree<T>::each_collision_if(PairCollisionPred isColliding, PairCollisionFunc onCollision)noexcept
{
	for (auto& node : m_Nodes)
	{
		auto& objects = node.objects;
		for (auto itr = objects.begin(); itr != objects.end(); ++itr)
		{
			for (auto nextItr = itr + 1; nextItr != objects.end(); ++nextItr)
			{
				if (isColliding(*itr, *nextItr))
				{
					onCollision(*itr, *nextItr);
				}
			}
		}

		int parent = node.parent;
		while (parent != QuadTree::NON_CHILD)
		{
			auto& parentObjects = m_Nodes[parent].objects;
			for (auto& obj : objects)
			{
				for (auto pObj : parentObjects)
				{
					if (isColliding(obj, pObj))
					{
						onCollision(obj, pObj);
					}
				}
			}
			parent = m_Nodes[parent].parent;
		}
	}
}

template<class T>
template<class PairCollisionFunc>
void QuadTree<T>::each_collision(PairCollisionFunc onCollision)noexcept
{
	each_collision_if([](const Entry<T>& l, const Entry<T>& r) {return l.bound.intersects(r.bound); }, onCollision);
}

template<class T>
template<class Collider, class OnCollisionFunc>
void QuadTree<T>::each_collision(Collider collider, OnCollisionFunc func)noexcept
{
	if (m_Nodes.size() == 0 ||
		!collider.intersects(m_Nodes[0].bound))
	{
		return;
	}
	std::stack<int> stack;
	stack.emplace(0);
	while (!stack.empty())
	{
		const int index = stack.top();
		stack.pop();
		auto& node = m_Nodes[index];
		for (auto& obj : node.objects)
		{
			if (collider.intersects(obj.bound))
			{
				func(obj);
			}
		}
		if (node.children != QuadTree::NON_CHILD)
		{
			for (int i = 0; i < QuadTree::COUNT; ++i)
			{
				if (collider.intersects(m_Nodes[node.children + i].bound))
				{
					stack.emplace(node.children + i);
				}
			}
		}
	}
}

template<class T> template<class Pred>
void QuadTree<T>::remove_if(Pred pred)
{
	for (auto& node : m_Nodes)
	{
		node.objects.erase(std::remove_if(node.objects.begin(), node.objects.end(), pred), node.objects.end());
	}
}

template<class T>
void QuadTree<T>::shrinkToFit()noexcept
{
	m_Nodes.shrink_to_fit();
	for (auto& node : m_Nodes)
	{
		node.objects.shrink_to_fit();
	}
}

template<class T>
void QuadTree<T>::split(int index, QuadNode<T>& node)noexcept
{
	const int childId = m_Nodes.size();
	node.children = m_Nodes.size();

	const Rect bound = node.bound;
	const float childWidth = bound.width / 2;
	const float childHeight = bound.height / 2;
	const float verticalLine = bound.x + childWidth;
	const float horizontalLine = bound.y + childHeight;

	m_Nodes.emplace_back(QuadNode<T>{ Rect{ bound.x + childWidth, bound.y, childWidth, childHeight }, index });
	m_Nodes.emplace_back(QuadNode<T>{ Rect{ bound.x, bound.y, childWidth, childHeight }, index });
	m_Nodes.emplace_back(QuadNode<T>{ Rect{ bound.x, bound.y + childHeight, childWidth, childHeight }, index });
	m_Nodes.emplace_back(QuadNode<T>{ Rect{ bound.x + childWidth, bound.y + childHeight, childWidth, childHeight }, index });

	auto& objects = m_Nodes[index].objects;
	objects.erase(std::remove_if(objects.begin(), objects.end(), [verticalLine, horizontalLine, childId, this](auto& entry)
		{
			const int target = getIndex(verticalLine, horizontalLine, entry.bound);
			if (target == QuadTree::NON_CHILD)
			{
				return false;
			}
			m_Nodes[childId + target].objects.push_back(std::move(entry));
			return true;
		}), objects.end());
}

template<class T>
constexpr int QuadTree<T>::getIndex(float vLine, float hLine, Rect bound)noexcept
{
	if (bound.y < hLine && (bound.height + bound.y < hLine))
	{
		if (bound.x > vLine)
		{
			return QuadTree::NE;
		}
		if (bound.x < vLine && (bound.x + bound.width < vLine))
		{
			return QuadTree::NW;
		}
	}
	if (bound.y > hLine)
	{
		if (bound.x > vLine)
		{
			return QuadTree::SE;
		}
		if (bound.x < vLine && (bound.x + bound.width < vLine))
		{
			return QuadTree::SW;
		}
	}
	return QuadTree::NON_CHILD;
}