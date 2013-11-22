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

class MlSkin : public CollisionRegion 
{
public:
	enum FloodCondition {
		ByDistance = 0,
		ByColor = 1
	};
	
	struct FloodTable {
		FloodTable(unsigned i) {
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
	
	void floodAround(MlCalamus c, const Vector3F & pos, const Vector3F & nor, const float & maxD, const float & minD);
	void selectAround(unsigned idx, const Vector3F & pos, const Vector3F & nor, const float & maxD, char byRegion = 0);
	void discardActive();
	
	void growFeather(const Vector3F & direction);
	void combFeather(const Vector3F & direction, const Vector3F & center, const float & radius);
	void scaleFeather(const Vector3F & direction, const Vector3F & center, const float & radius);
	void pitchFeather(const Vector3F & direction, const Vector3F & center, const float & radius);
	
	void computeFaceCalamusIndirection();
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
	
	void setFloodCondition(FloodCondition fc);
	FloodCondition floodCondition() const;
	
	char hasActiveFaces() const;
	void clearActiveFaces();
	void resetActiveFaces();
	void resetFloodFaces();
	void restFloodFacesAsActive();
	
	void setFloodRegion(char on);
	char floodRegion() const;
	
	void setEraseRegion(char on);
	char eraseRegion() const;
	
	void verbose() const;
	
protected:
	
private:
	bool createFeather(MlCalamus & ori);
	bool isPointTooCloseToExisting(const Vector3F & pos, float minDistance);
	bool isDartCloseToExisting(const Vector3F & pos, const std::vector<Vector3F> & existing, float minDistance) const;
	bool isFloodFace(unsigned idx, unsigned & dartBegin, unsigned & dartEnd) const;
	void resetFaceCalamusIndirection();
	unsigned lastInactive() const;
	void selectFeatherByFace(unsigned faceIdx);
private:
	struct SelectCondition {
		Vector3F center;
		Vector3F normal;
		float maxDistance;
		char byRegion;
	};
	
	MlCalamusArray * m_calamus;
	std::vector<unsigned> m_activeIndices;
	unsigned m_numFeather, m_numCreatedFeather;
	unsigned * m_faceCalamusStart;
	FloodCondition m_floodCondition;
	std::vector<unsigned> m_activeFaces;
	std::vector<FloodTable> m_floodFaces;
	SelectCondition m_selectCondition;
	char m_floodRegion, m_eraseRegion;
};