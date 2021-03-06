/*
 *  EasyModelOut.h
 *  easymodel
 *
 *  Created by jian zhang on 4/25/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <modelIO.h>
#include <string>
#include <AllMath.h>

class EasyModelOut {
public:
	EasyModelOut(const char* filename);
	~EasyModelOut();
	
	void begin();
	void end();
	
	void writeFaceCount(unsigned numPolygon, int * faceCounts); 
	void writeFaceConnection(unsigned numFaceVertices, int * faceVertices);
	void writeP(unsigned numVertices, Vector3F * vertexPositions);
	void writeUV(const char * name, const unsigned & c, float * u, float * v, const unsigned & cid, unsigned * id);

	void flush();
private:
	ModelIO _doc;
	std::string m_fileName;
};