/*
 *  main.cpp
 *  
 *
 *  Created by jian zhang on 4/24/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#include <BTree.h>
int main()
{
	std::cout<<"b-tree test\ntry to insert a few keys\n";
	BTree tree;
	tree.insert(200);
	tree.insert(112);
	tree.insert(2);
	tree.insert(6);
	tree.insert(1);
	tree.insert(9);
	tree.insert(11);
	tree.insert(5);
	tree.insert(7);
	tree.insert(8);
	tree.insert(4);
	tree.insert(13);
	tree.insert(46);
	tree.insert(36);
	tree.insert(34);
	tree.insert(32);
	tree.insert(35);
	tree.insert(45);
	tree.insert(100);
	tree.insert(80);
	tree.insert(70);
	tree.insert(71);
	tree.insert(93);
	tree.insert(99);
	tree.insert(3);
	tree.insert(22);
	tree.insert(10);
	tree.insert(12);
	tree.insert(19);
	tree.insert(23);
	tree.insert(20);
	tree.insert(18);
	tree.insert(21);
	tree.insert(17);
	tree.insert(31);
	tree.insert(16);
	tree.insert(15);
	tree.insert(49);
	tree.insert(39);
	tree.insert(43);
	tree.insert(24);
	tree.insert(29);
	tree.insert(30);
	tree.insert(73);
	tree.insert(38);
	tree.insert(41);
	tree.insert(37);
	tree.insert(54);
	tree.insert(76);
	tree.insert(40);
	tree.insert(42);
	tree.insert(47);
	tree.insert(44);
	tree.insert(104);
	tree.insert(111);
	tree.insert(59);
	tree.insert(55);
	tree.display();
	return 0;
}