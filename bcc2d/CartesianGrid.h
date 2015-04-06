#ifndef CARTESIANGRID_H
#define CARTESIANGRID_H

#include <BoundingBox.h>
#include <MortonHash.h>
class CartesianGrid 
{
public:
    struct CellIndex {
        unsigned key;
        unsigned index;
    };
    
    CartesianGrid(const BoundingBox & bound);
    virtual ~CartesianGrid();
    
    const unsigned numCells() const;
    void getBounding(BoundingBox & bound) const;
    const Vector3F origin() const;
    
protected:
	sdb::MortonHash * cells();
    const float cellSizeAtLevel(int level) const;
	const float gridSize() const;
    void addCell(const Vector3F & p, int level);
	const Vector3F gridOrigin(unsigned code) const;
	const Vector3F cellOrigin(unsigned code, int level) const;
	void removeCell(unsigned code);
    const Vector3F cellCenter(unsigned code) const;
	void printHash();
private:
    Vector3F m_origin;
    float m_span, m_gridH; // same for each dimensions
    sdb::MortonHash * m_cellHash;
    unsigned m_numCells;
};

#endif        //  #ifndef CARTESIANGRID_H

