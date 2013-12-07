/*
 *  MlSkin.h
 *  mallard
 *
 *  Created by jian zhang on 9/14/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <AllMath.h>
#include <CollisionRegion.h>
#include <MlCalamus.h>
#include <Patch.h>

class AccPatchMesh;
class MeshTopology;
class MlCalamusArray;
class BaseImage;
class SelectCondition;
class FloodCondition;
class MlCluster;

class MlSkin : public CollisionRegion 
{
public:	
	struct FloodTable {
		FloodTable() {}
		FloodTable(unsigned i) {
			reset(i);
		}
		
		void reset(unsigned i) {
			faceIdx = i;
			dartBegin = dartEnd = 0;
		}
		
		unsigned faceIdx, dartBegin, dartEnd;
	};
	
	MlSkin();
	virtual ~MlSkin();
	void cleanup();
	void clearFeather();
	
	virtual void setBodyMesh(AccPatchMesh * mesh, MeshTopology * topo);
	
	void setNumFeathers(unsigned num);
	
	void floodAround(MlCalamus c, FloodCondition * condition);
	void selectAround(unsigned idx, SelectCondition * selcon);
	void discardActive();
	
	void growFeather(const Vector3F & direction);
	void combFeather(const Vector3F & direction, const Vector3F & center, const float & radius);
	void scaleFeather(const Vector3F & direction, const Vector3F & center, const float & radius);
	void pitchFeather(const Vector3F & direction, const Vector3F & center, const float & radius);
	void smoothShell(const Vector3F & center, const float & radius, const float & weight);
	void computeFaceCalamusIndirection();
	void computeVertexDisplacement();
	void conputeFaceClustering();
	void finishCreateFeather();
	void finishEraseFeather();
	unsigned numFeathers() const;
	MlCalamus * getCalamus(unsigned idx) const;
	
	
	unsigned numActive() const;
	MlCalamus * getActive(unsigned idx) const;
	
	void getPointOnBody(MlCalamus * c, Vector3F &p) const;
	void getNormalOnBody(MlCalamus * c, Vector3F &p) const;
	
	void tangentSpace(MlCalamus * c, Matrix33F & frm) const;
	void rotationFrame(MlCalamus * c, const Matrix33F & tang, Matrix33F & frm) const;
	
	bool hasFeatherCreated() const;
	unsigned numCreated() const;
	MlCalamus * getCreated(unsigned idx) const;
	MlCalamusArray * getCalamusArray() const;
	
	void resetFloodFaces();
	void restFloodFacesAsActive();
	
	void resetFaceVicinity();
	void setFaceVicinity(unsigned idx, float val);
	float faceVicinity(unsigned idx) const;
	void shellUp(std::vector<Vector3F> & dst);
	void getClustering(unsigned idx, std::vector<Vector3F> & dst);
	
	void verbose() const;
protected:
	
private:
    bool createFeather(MlCalamus & ori);
	bool isPointTooCloseToExisting(const Vector3F & pos, float minDistance);
	bool isDartCloseToExisting(const Vector3F & pos, const std::vector<Vector3F> & existing, float minDistance) const;
	bool isFloodFace(unsigned idx, unsigned & dartBegin, unsigned & dartEnd) const;
	bool isActiveFace(unsigned idx, std::vector<unsigned> & dartIndices) const;
	bool isActiveFeather(unsigned idx) const;
	void resetFaceCalamusIndirection();
	unsigned lastInactive(unsigned last) const;
	unsigned selectFeatherByFace(unsigned faceIdx, SelectCondition * selcon);
private:	
	MlCalamusArray * m_calamus;
	std::vector<unsigned> m_activeIndices;
	unsigned m_numFeather, m_numCreatedFeather;
	FloodTable * m_faceCalamusTable;
	MlCluster * m_perFaceCluster;
	float * m_perFaceVicinity;
	std::vector<FloodTable> m_activeFaces;
	std::vector<FloodTable> m_floodFaces;
};