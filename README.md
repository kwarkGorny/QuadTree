# QuadTree
One header only implementation of non-recursive quadtree
# Constructing a quadtree
```cpp
QuadTree<YourType> quadtree(maxLevels, maxObjectsPerNode, { x, y, width, height});
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
```cpp
// resovling collision using default collision check function
quadtree.each_collision( [](Entry<YourType>& entryL, Entry<YourType>& entryR) {
  std::cout << "collision detected"
});

auto yourCollsionCheckFuncion = [](Entry<YourType>& entryL, Entry<YourType>& entryR) {
     return entryL.bound.intersects(entryR.bound);
 };
 
 auto yourCollsionResolveFunction = [](Entry<YourType>& entryL, Entry<YourType>& entryR) {
     std::cout << "collision detected"
 };
 
// resolving collsions with own intersection function
quadtree.each_collision(yourCollsionCheckFuncion, yourCollsionResolveFunction);
  
// resolving collsions with object outside quadtree
quadtree.each_collision(mousePosition, yourCollsionCheckFuncion, yourCollsionResolveFunction);

# Utility functions
// reserve memory for 5 QuadNodes
quadtree.reserve(5);
// removing all nodes
quadtree.clear();
// release unneccesery memmory
quadtree.shrink_to_fit();

  
  
```
