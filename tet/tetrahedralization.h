/*
 *  tetrahedralization.h
 *  foo
 *
 *  Created by jian zhang on 6/5/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef TTG_TETRAHEDRALIZATION_H
#define TTG_TETRAHEDRALIZATION_H

#include "bipyramid_flip.h"

namespace ttg {

/// split t1 to t1 t2 t3 t4 by vi
inline void splitTetrahedron(ITetrahedron * t1, ITetrahedron * t2, 
							ITetrahedron * t3, ITetrahedron * t4,
							int vi)
{
	std::cout<<"\n split "; printTetrahedronVertices(t1);
	
/// vertices of t1		
	const int v0 = t1->iv0;
	const int v1 = t1->iv1;
	const int v2 = t1->iv2;
	const int v3 = t1->iv3;
	
	ITetrahedron * nei1 = neighborOfTetrahedron(t1, v1, v2, v3);
	ITetrahedron * nei2 = neighborOfTetrahedron(t1, v0, v1, v3);
	ITetrahedron * nei3 = neighborOfTetrahedron(t1, v0, v2, v1);
	ITetrahedron * nei4 = neighborOfTetrahedron(t1, v0, v3, v2);
		
	setTetrahedronVertices(*t1, vi, v1, v2, v3);
	setTetrahedronVertices(*t2, vi, v0, v1, v3);
	setTetrahedronVertices(*t3, vi, v0, v2, v1);
	setTetrahedronVertices(*t4, vi, v0, v3, v2);

	connectTetrahedrons(t1, t2);
	connectTetrahedrons(t1, t3);
	connectTetrahedrons(t1, t4);
	connectTetrahedrons(t1, nei1);
	
	connectTetrahedrons(t2, t3);
	connectTetrahedrons(t2, t4);
	connectTetrahedrons(t2, nei2);
	
	connectTetrahedrons(t3, t4);
	connectTetrahedrons(t3, nei3);
	
	connectTetrahedrons(t4, nei4);
	
	std::cout<<"\n to "; printTetrahedronVertices(t1);
	std::cout<<"\n  + "; printTetrahedronVertices(t2);
	std::cout<<"\n  + "; printTetrahedronVertices(t3);
	std::cout<<"\n  + "; printTetrahedronVertices(t4);
	
	if(!checkTetrahedronConnections(t1) ) printTetrahedronNeighbors(t1);
	if(!checkTetrahedronConnections(t2) ) printTetrahedronNeighbors(t2);
	if(!checkTetrahedronConnections(t3) ) printTetrahedronNeighbors(t3);
	if(!checkTetrahedronConnections(t4) ) printTetrahedronNeighbors(t4);
	std::cout<<"\n end of split ";
}

/// once an face is flipped, spawn three more dipyramids for potential face flipping 
inline void spawnFaces(std::deque<Bipyramid> & pyras,
						ITetrahedron * tc)
{
	Bipyramid p0 = pyras[0];
	std::cout<<"\n spawn "; printBipyramidVertices(&p0);
	
	const int v1 = p0.iv1;
	const int v2 = p0.iv2;
	const int v3 = p0.iv3;
	const int v4 = p0.iv4;
	ITetrahedron * nei4 = neighborOfTetrahedron(p0.ta, v4, v1, v2);
	
	if(nei4) {std::cout<<"\n side 4";
		Bipyramid pa;
		if(createBipyramid(&pa, p0.ta, nei4) )
			pyras.push_back(pa);
	}
	
	ITetrahedron * nei5 = neighborOfTetrahedron(p0.tb, v4, v2, v3);
	
	if(nei5) {std::cout<<"\n side 5";
		Bipyramid pb;
		if(createBipyramid(&pb, p0.tb, nei5) )
			pyras.push_back(pb);
	}
	
	ITetrahedron * nei6 = neighborOfTetrahedron(tc, v4, v3, v1);

	if(nei6) {std::cout<<"\n side 6";
		Bipyramid pc;
		if(createBipyramid(&pc, tc, nei6) )
			pyras.push_back(pc);
	}
}

inline void flipFaces(std::deque<Bipyramid> & pyras, 
							const aphid::Vector3F * X,
							ITetrahedron * tets,
							int & numTets)
{
	int nq = pyras.size();
	int mergNei, i=0;
	while(nq>0) {
		if(canSplitFlip(pyras[0], X) ) {
			if(processSplitFlip(pyras[0], tets, numTets) ) {
				spawnOnSide(pyras, pyras[0], pyras[0].ta, 4);
				spawnOnSide(pyras, pyras[0], pyras[0].tb, 5);
				spawnOnSide(pyras, pyras[0], pyras[0].tc, 6);
			}
		}
		else if(canMergeFlip(mergNei, pyras[0], X) ) {
			if(processMergeFlip(&pyras[0], mergNei, 
							tets, numTets) ) {
							
				spawnOnSide(pyras, pyras[0], pyras[0].ta, 1);
				spawnOnSide(pyras, pyras[0], pyras[0].ta, 2);
				spawnOnSide(pyras, pyras[0], pyras[0].ta, 3);
				spawnOnSide(pyras, pyras[0], pyras[0].tb, 4);
				spawnOnSide(pyras, pyras[0], pyras[0].tb, 5);
				spawnOnSide(pyras, pyras[0], pyras[0].tb, 6);
			}
		}
		pyras.erase(pyras.begin() );
		nq = pyras.size();
	}
	std::cout<<"\n end flip";
}

inline void splitTetrahedron4(std::vector<ITetrahedron *> & tets,
							ITetrahedron * t,
							const int & vi)
{

}

inline void splitTetrahedron3(std::vector<ITetrahedron *> & tets,
							ITetrahedron * t,
							const int & vi)
{

}

inline void expandTetrahedronRegion(std::vector<ITetrahedron *> & tets,
							std::vector<ITetrahedron *> & source)
{
	std::vector<ITetrahedron *>::iterator it = source.begin();
	for(;it!=source.end();++it) {
		addTetrahedronTo(tets, *it );
		addTetrahedronTo(tets, (*it)->nei0);
		addTetrahedronTo(tets, (*it)->nei1);
		addTetrahedronTo(tets, (*it)->nei2);
		addTetrahedronTo(tets, (*it)->nei3);
	}
}

inline void addTetrahedronFaces(ITetrahedron * t,
					aphid::sdb::Array<aphid::sdb::Coord3, IFace > & faces)
{
	ITRIANGLE fa;
	int i=0;
	for(;i<4;++i) {
		faceOfTetrahedron(&fa, t, i);
		aphid::sdb::Coord3 itri = aphid::sdb::Coord3(fa.p1, fa.p2, fa.p3).ordered();
		IFace * tri = faces.find(itri );
		if(!tri) {
			tri = new IFace;
			tri->key = itri;
			tri->ta = t;
			
			faces.insert(itri, tri);
		}
		else {
			tri->tb = t;
		}
	}
}

inline void splitTwoWay(ITetrahedron * t,
						std::vector<ITetrahedron *> & tets,
						aphid::sdb::Array<aphid::sdb::Coord3, IFace > & faces,
						const int & vx,
						const int & va, const int & vb)
{
	//std::cout<<"\n split "; printTetrahedronVertices(t);
	
	ITRIANGLE fa = oppositeFace(t, va);
	ITRIANGLE fb = oppositeFace(t, vb);

	ITetrahedron * na = neighborOfTetrahedron(t, fa.p1, fa.p2, fa.p3);
	ITetrahedron * nb = neighborOfTetrahedron(t, fb.p1, fb.p2, fb.p3);
	
/// change the existing
	setTetrahedronVertices(*t, vx, fa.p1, fa.p2, fa.p3);

	if(na) {
		connectTetrahedrons(t, na, fa.p1, fa.p2, fa.p3);
	}
	
/// add an extra
	ITetrahedron * tb = new ITetrahedron;
	setTetrahedronVertices(*tb, vx, fb.p1, fb.p2, fb.p3);
	tb->index = tets.size();
	
	
	if(nb) {
		connectTetrahedrons(tb, nb, fb.p1, fb.p2, fb.p3);
	}
	
	connectTetrahedrons(t, tb);
	
	tets.push_back(tb);
	
	addTetrahedronFaces(t, faces);
	addTetrahedronFaces(tb, faces);
}

/// split on edge, find all connected tetra, split each into two
inline void splitTetrahedron2(std::vector<ITetrahedron *> & tets,
							ITetrahedron * t,
							const int & vi,
                            const Float4 & coord)
{
    IEdge e;
    findTetrahedronEdge(&e, t, coord);
    
    std::vector<ITetrahedron *> tetOnEdge;
	tetOnEdge.push_back(t);
	
    findTetrahedronsConnectedToEdge(tetOnEdge, e, t);
	
	aphid::sdb::Array<aphid::sdb::Coord3, IFace > faces;
	
	std::vector<ITetrahedron *>::iterator it = tetOnEdge.begin();
	for(;it!=tetOnEdge.end();++it) {
		splitTwoWay(*it, tets, faces,
					vi, e.iv0, e.iv1);
	}
	
	connectTetrahedrons(faces);
	
	faces.clear();
	tetOnEdge.clear();
}

inline void splitTetrahedron1(std::vector<ITetrahedron *> & tets,
							ITetrahedron * t,
							const int & vi)
{

}

inline void splitTetrahedron(std::vector<ITetrahedron *> & tets,
							ITetrahedron * t,
							const int & vi,
							const Float4 & coord)
{
	int stat = aphid::barycentricCoordinateStatus(coord);
	if(stat == 0) {
		splitTetrahedron4(tets, t, vi);
	}
	else if(stat == 1) {
		splitTetrahedron3(tets, t, vi);
	}
	else if(stat == 2) {
		splitTetrahedron2(tets, t, vi, coord);
	}
	else if(stat == 3) {
		splitTetrahedron1(tets, t, vi);
	}
}

}
#endif
