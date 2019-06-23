#include "../include/QuadTree.h"
#include <iostream>

int main()
{
	QuadTree<int> tree(5, 10, { 0,0, 100, 100 });
	tree.insert({-3, -3, 5, 5}, 0);
	tree.insert({ 2, 2, 5, 5 }, 1);
	tree.insert({ 0, 6, 5, 5 }, 2);
	tree.insert({ 0, 6, 5, 5 }, 3);
	tree.insert({ 10, 10, 5, 5 }, 4);

	tree.shrink_to_fit();
	tree.each_collision([](Entry<int>& l, Entry<int>& r) {
		std::cout << l.obj << " " << r.obj << '\n';
	});


	std::cout.flush();
}