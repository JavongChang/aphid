#ifndef WldWidget_H
#define WldWidget_H

#include <QGLWidget>
#include <Base3DView.h>
#include <KdEngine.h>
#include <ConvexShape.h>
#include <IntersectionContext.h>
#include <VoxelGrid.h>

class WldWidget : public aphid::Base3DView
{
    Q_OBJECT

public:
    
    WldWidget(const std::string & filename, QWidget *parent = 0);
    ~WldWidget();
	
protected:    
    virtual void clientInit();
    virtual void clientDraw();
    virtual void clientSelect(QMouseEvent *event);
    virtual void clientDeselect(QMouseEvent *event);
    virtual void clientMouseInput(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void resizeEvent(QResizeEvent * event);

private:
    void drawBoxes() const;
    void drawTree();
    void drawIntersect();
	aphid::KdNTree<aphid::cvx::Cube, aphid::KdNode4 > * tree();
	bool readTree(const std::string & filename);
	void testTree();
	void testIntersect(const aphid::Ray * incident);
	void drawActiveSource(const unsigned & iLeaf);
	aphid::BoundingBox getFrameBox();
	
private slots:
	
private:
	aphid::IntersectionContext m_intersectCtx;
	aphid::sdb::VectorArray<aphid::cvx::Cube> * m_source;
	aphid::KdNTree<aphid::cvx::Cube, aphid::KdNode4 > * m_tree;
};
//! [3]

#endif
