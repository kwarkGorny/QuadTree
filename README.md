# QuadTree
One header only implementation of non-recursive quadtree
# Constructing a quadtree
```cpp
QuadTree<YourType> quadtree;
tree.initialize(maxLevels, maxObjectsPerNode, { x, y, width, height});
```
# Inserting objects
```cpp
quadtree.insert({ x, y, width, height}, data);
```
# Removing objects
```cpp
//Removing all objects with predicate
quadTree.erase_if([](Entry<YourType>& entry) { ... });

//Removing first object with predicate
quadTree.erase_first_if([](Entry<YourType>& entry) { ... });

//Removing all objects by value
quadTree.erase(value);

//Removing first object by value
quadTree.erase_first(value);
```

# Collision checking
quadtree.each_collision([](Entry<YourType>& entryL, Entry<YourType>& entryR) {})
