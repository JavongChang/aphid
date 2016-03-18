#ifndef CARTESIANGRID_H
#define CARTESIANGRID_H

#include <BoundingBox.h>
#include <MortonHash.h>

namespace aphid {

class BaseBuffer;

class CartesianGrid 
{
/// left-bottom-back corner
	Vector3F m_origin;
/// same for each dimensions
    float m_span, m_gridH;
/// 30-bit morton code limited to level 10 
    sdb::CellHash * m_cellHash;
	
public:
    CartesianGrid();
	virtual ~CartesianGrid();
    
    const unsigned numCells() const;
    void getBounding(BoundingBox & bound) const;
    const Vector3F origin() const;
	const float span() const;
    
	sdb::CellHash * cells();
    const Vector3F cellCenter(unsigned code) const;
    const float cellSizeAtLevel(int level) const;
	BoundingBox cellBox(unsigned code, int level) const;
	
	void addCell(unsigned code, int level, int visited, unsigned index);
	unsigned addCell(const Vector3F & p, int level, int visited);
	
    unsigned mortonEncodeLevel(const Vector3F & p, int level) const;
	void printGrids(BaseBuffer * dst);
	
	bool isPInsideBound(const Vector3F & p) const;
	void putPInsideBound(Vector3F & p) const;
	
	sdb::CellValue * findCell(unsigned code) const;
	
protected:
	void setBounding(float * originSpan);
	void setBounding(const BoundingBox & bound);
    
	const float gridSize() const;
	
	void gridOfP(const Vector3F & p, unsigned & x,
									unsigned & y,
									unsigned & z) const;
	void gridOfCell(unsigned & x,
									unsigned & y,
									unsigned & z,
									int level) const;
	const unsigned mortonEncode(const Vector3F & p) const;
	sdb::CellValue * findGrid(unsigned code) const;
	unsigned addGrid(const Vector3F & p);
    const Vector3F gridOrigin(unsigned code) const;
	const Vector3F cellOrigin(unsigned code, int level) const;
	void removeCell(unsigned code);
    void printHash();
	
	unsigned encodeNeighborCell(unsigned code, 
									int level,
									int dx, int dy, int dz);

    sdb::CellValue * findNeighborCell(unsigned code, 
									int level,
									int dx, int dy, int dz);
									
	unsigned encodeFinerNeighborCell(unsigned code, 
									int level,
									int dx, int dy, int dz,
									int cx, int cy, int cz) const;
									
	sdb::CellValue * findFinerNeighborCell(unsigned code, 
									int level,
									int dx, int dy, int dz,
									int cx, int cy, int cz);
									
	static const float Cell8ChildOffset[8][3];
	
private:
    
};

}
#endif        //  #ifndef CARTESIANGRID_H

