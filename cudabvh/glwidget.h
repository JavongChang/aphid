#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <Base3DView.h>
#include <app_define.h>

class BvhSolver;
class BaseBuffer;
class GLWidget : public Base3DView
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
public slots:
	
signals:

protected:
    virtual void clientInit();
    virtual void clientDraw();
	virtual void clientSelect(QMouseEvent *event);
    virtual void clientDeselect(QMouseEvent *event);
    virtual void clientMouseInput(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
private:
	void showEdgeContacts();
	void showAabbs();
	void showRays();
	
	const unsigned numVertices() const;
	const unsigned numTriangles() const;
	const unsigned numTriangleFaceVertices() const;
	const unsigned numEdges() const;

private:
	BvhSolver * m_solver;
	BaseBuffer * m_displayVertex;
	BaseBuffer * m_triangleIndices;
	BaseBuffer * m_displayRays;
	BaseBuffer * m_edges;
	int m_displayLevel;
	
#ifdef BVHSOLVER_DBG_DRAW
	BaseBuffer * m_displayLeafAabbs;
	BaseBuffer * m_displayInternalAabbs;
	BaseBuffer * m_displayInternalDistance;
	BaseBuffer * m_displayLeafHash;
	BaseBuffer * m_internalChildIndices;
#endif

private slots:
    
};
//! [3]

#endif
