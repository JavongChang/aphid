/*
 *  YarnPatch.h
 *  knitfabric
 *
 *  Created by jian zhang on 6/5/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <accPatch.h>
#include <BaseMesh.h>

class YarnPatch : public AccPatch, public BaseMesh {
public:
	YarnPatch();
	void setQuadVertices(unsigned *v);
	void findWaleEdge(unsigned v0, unsigned v1, int bothSide);
	void waleEdges(short &n, unsigned * v) const;
	
	char hasWaleEdges() const;
	char hasTessellation() const;
	void tessellate();
	void increaseWaleGrid(int dv);
	void increaseCourseGrid(unsigned v0, unsigned v1, int dv);
	
	char isWaleEdge(unsigned v0, unsigned v1) const;
	char isCourseEdge(unsigned v0, unsigned v1) const;
	short getWaleGrid() const;
	void setWaleGrid(short val);
	
	short getCourseGrid(unsigned v0, unsigned v1) const;
	char setCourseGrid(unsigned v0, unsigned v1, short val);
private:
	void setWaleEdge(short i, short j);
	void setSecondWaleEdge(short j, char dir);
	char verifyNumGrid();
	void fillP(const short & nrow, const short & ncol0, const short & ncol1, const short & colDir);
	void fillF(const short & nrow, const short & ncol0, const short & ncol1, const short & colDir);
	void getCornerUV(short quadV, float & u, float & v) const;
	short rowDifference(short & step, short & rowEnd, const short & targetRowEnd, const short & irow) const;
	short courseSide(unsigned v0, unsigned v1) const;
	char isTriangle() const;
	void tessellateQuad();
	void tessellateTriangle();
	char isConverging() const;

	unsigned * m_quadVertices;
	short m_waleVertices[4];
	short m_numWaleEdges;
	short m_numWaleGrid;
	short m_numCourseGrid[2];
	char m_hasTessellation;
};