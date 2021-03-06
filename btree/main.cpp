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
#include <Types.h>
#include <Array.h>
#include "Sequence.h"
#include "../shared/PseudoNoise.h"
using namespace aphid::sdb;

void testFind(Array<int, int> & arr, int k)
{
	std::cout<<"\n try to find "<<k<<"\n";
	int ek;
	int * found = arr.find(k, MatchFunction::mLequal, &ek);
	if(found) {
		std::cout<<"\n found arr["<<ek<<"] = "<<*found;
	}
	else 
		std::cout<<"\n "<<k<<" is out of range";
}

void printList(List<int> & l)
{
	std::cout<<"\nlist: ";
	l.begin();
	int i = 0;
	while(!l.end()) {
		std::cout<<" "<<l.value();
		i++;
		l.next();
	}
	std::cout<<" count "<<i<<"\n";
}

void printArray(Array<int, float> & arr)
{
	std::cout<<"\n display array \n size "<<arr.size()<<"\n";
	arr.begin();
	while(!arr.end() ) {
		std::cout<<" a["<<arr.key()<<"] "<<*arr.value();
		arr.next();
	}
}

void testArrayRemove()
{
	std::cout<<"\n test array remove";
	
	Array<int, float> arr;
	
	int k[15];
	float v[15];
	int i = 0;
	for(i=0;i<15;i++) {
		k[i] = rand() % 99 - 39;
		v[i] = ((float)(rand() % 99) )/ 99.f - .5f;
		
		arr.insert(k[i], &v[i]);
	}
	
	printArray(arr);
	
	v[8] = -99.f;
	
	printArray(arr);
	
	for(i=0;i<15;i++) {
		std::cout<<"\n rm "<<k[i];
		arr.remove(k[i]);
		printArray(arr);
		std::cout<<"\n v "<<v[i];
	}
	
	std::cout<<"\n done";
}

void testSequenceRemove()
{
#define INTERACTOVE 0
#define DOSHUFFLE 1

#if INTERACTOVE
	
	int n = 300;
#else

	int n = 1<<15;
#endif
	std::cout<<"\n test sequence n "<<n;
	Sequence<int> sq;
	sq.insert(0);
	std::cout<<"\n size aft 0 "<<sq.size();
	sq.insert(-1);
	std::cout<<"\n size aft -1 "<<sq.size();
		
	int i = 0, j = 2;
	for(;i<n;++i) {
		
		int k = i; 
		k = rand() & ((1<<30)-1);
		k++;
		
		if(!sq.findKey(k) )
			sq.insert(k);
		
		if((i & (j-1)) == 0) {
			std::cout<<"\n i"<<i<<" size "<<sq.size();
			j<<=1;
		}
	}
	
	if(!sq.findKey(0) ) {
		sq.display();
		std::cout<<"\n failed to find zero, abort";
		return;
	}
	
	int nb4rm = sq.size();
	std::cout<<"\n sequence size "<<nb4rm;
	
	if(!sq.dbgCheck() ) {
		sq.display();
		std::cout<<"\n failed check, abort";
		return;
	}
	
	std::cout<<"\n test key order ";
    int ksize = 0;
	int * keystorm = new int[nb4rm + 1];
	sq.begin();
	while(!sq.end() ) {
		int k = sq.key();
		
		if(ksize>1) {
			if(k <= keystorm[ksize-1] ) {
				std::cout<<" wrong key "<<k<<" <= "<< keystorm[ksize-1];
				return;
			}
		}
            keystorm[ksize++] = k;
        
		sq.next();
	}
    std::cout<<"\n ksize "<<ksize;

	int nrm = sq.size();
#if DOSHUFFLE
	for(i=0;i<nrm;++i) {
		for(j=0; j<33;++j) {
			int rj = rand() % nrm;
			if(rj > 19) {
				int tk = keystorm[rj];
				keystorm[rj] = keystorm[rj-19];
				keystorm[rj-19] = tk;
			}
		}
	}
#endif
	
#if INTERACTOVE
	sq.display();
#endif

	std::cout<<"\n passed\n test find n key "<<nrm;
	for(j=0;j<nrm;++j) {
			if(!sq.findKey(keystorm[j]) ) {
				std::cout<<"\n\n error cannot find "<<keystorm[j]
				<<"\n "<<" at "<<j;
				sq.dbgFind(keystorm[j]);
				return;
			}
		}
	std::cout<<"\n passed\n test remove keys "<<nrm;
	
	int anrm = 0;
	for(i=0; i< nrm; ++i) {
	
#if INTERACTOVE
		std::cout<<"\n remove "<<i<<" "<<keystorm[i];
		char c;
		std::cout << "Enter: ";
		std::cin >> c;
#endif
		
		int presz = sq.numLeaf();
		if(sq.findKey(keystorm[i])) sq.remove(keystorm[i]);
		else {
			std::cout<<"\n error cannot find to remove "<<keystorm[i];
			sq.dbgFind(keystorm[i]);
			std::cout<<"\n n leaf "<<sq.numLeaf()
			<<"\n n removed "<<anrm;
			sq.display();
			std::cout<<"\n abort after remove "<<anrm;
			
			return;
		}
		
		if(!sq.dbgCheck() ) {
			sq.display();
			std::cout<<"\n failed check, abort after remove "<<anrm;
			return;
		}
		anrm++;
#if INTERACTOVE
		sq.display();
#endif
	}
	std::cout<<"\n passded!\n all "<<nrm<<" keys removed";
    delete[] keystorm;
}

int main()
{
	/*
	std::cout<<"b-tree test\ntry to insert a few keys\n";
	BTree tree;
	tree.find(99);
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
	tree.insert(100);
	tree.display();
    std::cout<<"\n rm 100 ";
	tree.remove(100);tree.display();
	tree.remove(7);tree.display();
	tree.remove(9);tree.display();
	tree.remove(8);tree.display();
	
	tree.remove(2);tree.display();
	tree.remove(4);tree.display();
	tree.remove(6);tree.display();
	tree.remove(112);tree.display();
	tree.remove(1);tree.display();
	
	tree.insert(13);
	tree.insert(46);tree.display();
	
	
	tree.remove(13);
	tree.remove(200);tree.display();

	tree.insert(36);
	tree.insert(34);tree.display();
	
	tree.insert(32);
	tree.insert(35);
	tree.insert(45);
	tree.insert(80);
	
	tree.insert(70);
	tree.insert(71);
	tree.insert(93);
	tree.insert(99);
	tree.insert(3);
	tree.remove(36);tree.display();

	tree.insert(22);tree.display();
	tree.insert(10);
	tree.insert(12);
	tree.insert(19);
	tree.insert(23);tree.display();
	tree.insert(20);tree.display();
	
	tree.insert(76);tree.display();
	tree.insert(54);
	tree.insert(73);
	tree.insert(104);
	tree.insert(111);tree.display();
	tree.insert(40);
	tree.insert(42);
	tree.insert(47);
	tree.insert(44);
	tree.insert(59);
	tree.insert(55);tree.display();
	tree.insert(49);
	tree.insert(41);
	tree.insert(43);
	tree.insert(18);
	tree.insert(21);
	tree.insert(17);
	tree.insert(1);
	tree.insert(7);
	tree.insert(0);
	tree.insert(2);
	tree.insert(6);
	tree.insert(4);tree.display();
	tree.insert(-100);
	tree.insert(-32);
	tree.insert(-9);
	tree.insert(-19);
	tree.insert(-89);
	tree.insert(-29);
	tree.insert(-49);
	tree.insert(-7);
	tree.insert(-17);
	tree.insert(-79);
	tree.insert(-48);
	tree.remove(32);tree.display();
	tree.remove(23);tree.display();
    std::cout<<"remove 40";
	tree.remove(40);tree.display();
	tree.remove(80);tree.display();
	tree.remove(46);tree.display();
	tree.insert(106);
	tree.insert(206);
	tree.insert(246);tree.display();
	
	tree.displayLeaves();
	
	tree.find(106);
	tree.find(22);
	tree.find(92);
	std::cout<<"\n test array\n";
	
	PseudoNoise noi;
	TreeNode::MaxNumKeysPerNode = 256;
	TreeNode::MinNumKeysPerNode = 16;
	Array<int, int>arr;
	int p[128];
	for(int i=0; i < 128; i++) {
	    p[i] = noi.rint1(i) % 199;
	    arr.insert(i+10, &p[i]);
	}

	std::cout<<"\n";
    arr.display();
    std::cout<<"\n arr rm 99 ";
	arr.remove(99);
    arr.display();
    std::cout<<"\n arr rm 30 ";
	arr.remove(30);
    arr.display();
	
	arr.insert(31, &p[0]); arr.display();
	arr.beginAt(23);
	while(!arr.end()) {
	    std::cout<<" "<<arr.key()<<":"<<*arr.value()<<" ";
	    arr.next();   
	} 
	testFind(arr, 31);
	testFind(arr, 30);
	testFind(arr, 99);
	testFind(arr, 199);
	testFind(arr, 0);
	testFind(arr, 11);
	
	List<int> ll;
	for(int i = 0; i < 199; i++)
		ll.insert(noi.rint1(i) % 999);

	printList(ll);
	
	*ll.valueP(2) = 2;
	printList(ll);
	ll.insert(34);
	printList(ll);
	ll.remove(3);
	printList(ll);
	ll.remove(47);
	printList(ll);
	ll.remove(99);
	printList(ll);
	ll.remove(32);
	printList(ll);
	ll.remove(34);
	printList(ll);
	ll.remove(2);
	printList(ll);
	std::cout<<"\n remove "<<ll.value(128);
	ll.remove(ll.value(128));
	printList(ll);
	
	ll.clear();
	
	testArrayRemove();
	
	Sequence<Coord3> c3t;
	Pair<Coord3, Entity> * p0 = c3t.insert(Coord3(0,0,0));
	std::cout<<"\n p"<<p0->index;
	*/
	testSequenceRemove();
	std::cout<<"\n end of test\n";
	return 0;
}
