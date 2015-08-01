#include <CartesianGrid.h>
class BaseBuffer;
class KdIntersection;
class AdaptiveGrid : public CartesianGrid
{
public:
    AdaptiveGrid(const BoundingBox & bound);
    virtual ~AdaptiveGrid();
    
    void create(KdIntersection * tree);
protected:
    virtual bool tagCellsToRefine(KdIntersection * tree);
    void refine(KdIntersection * tree);
private:
    void setCellToRefine(unsigned k, const sdb::CellValue * v,
                         int toRefine);
    bool cellNeedRefine(unsigned k);
    bool check24NeighboursToRefine(unsigned k, const sdb::CellValue * v);
private:
    sdb::CellHash * m_cellsToRefine;
};
