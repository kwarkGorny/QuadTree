#include "../include/QuadTree.h"
#include <iostream>

int main()
{
	QuadTree<int> tree;
	tree.initialize(5, 10, { 0,0, 100, 100 });
	tree.add({-3, -3, 5, 5}, 0);
	tree.add({ 2, 2, 5, 5 }, 1);
	tree.add({ 0, 6, 5, 5 }, 2);
	tree.add({ 0, 6, 5, 5 }, 3);
	tree.add({ 10, 10, 5, 5 }, 4);
	tree.shrinkToFit();
	tree.each_collision([](Entry<int>& l, Entry<int>& r) {
		std::cout << l.obj << " " << r.obj << '\n';
	});


	std::cout.flush();
}