#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <Base3DView.h>
#include <KdEngine.h>
#include <ConvexShape.h>

using namespace aphid;

class GLWidget : public aphid::Base3DView
{
    Q_OBJECT

public:
    
    GLWidget(const std::string & filename, QWidget *parent = 0);
    ~GLWidget();
	
protected:    
    virtual void clientInit();
    virtual void clientDraw();
    virtual void clientSelect(Vector3F & origin, Vector3F & ray, Vector3F & hit);
    virtual void clientDeselect();
    virtual void clientMouseInput(Vector3F & stir);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void resizeEvent(QResizeEvent * event);

private:
    void drawBoxes() const;
    void drawTree();
    void drawATreelet(KdNode4 * treelet, const BoundingBox & lftBox, const BoundingBox & rgtBox, int level);
	void drawANode(KdNode4 * treelet, int idx, const BoundingBox & box, int level, bool isRoot = false);
    void drawConnectedTreelet(KdNode4 * treelet, int idx, const BoundingBox & box, int level);
	void drawALeaf(unsigned start, unsigned n, const BoundingBox & box);
	KdNTree<cvx::Cube, KdNode4 > * tree();
	
private slots:
    bool readTree(const std::string & filename);
	void testTree();
	
private:
	KdEngine<cvx::Cube> m_engine;
	sdb::VectorArray<cvx::Cube> * m_source;
	KdNTree<cvx::Cube, KdNode4 > * m_tree;
	int m_treeletColI;
	int m_maxDrawTreeLevel;
};
//! [3]

#endif
