#include <QtGui>
#include <QtOpenGL>
#include <Types.h>
#include "glwidget.h"
#include <cmath>
#include <KdTreeDrawer.h>
#include <Sequence.h>
#include <Ordered.h>
#define NUMVERTEX 12000
GLWidget::GLWidget(QWidget *parent) : Base3DView(parent)
{
    int i;
	float w = 29.3f, h = 3.f, d = 13.f;
    m_pool = new V3[NUMVERTEX];
    for(i = 0; i < NUMVERTEX; i++) {
        V3 & t = m_pool[i];
		if(i > NUMVERTEX/2) {
			w = 5.f;
			h = 16.3f;
			d = 3.f;
		}
        t.data[0] = (float(rand()%694) / 694.f - 0.5f) * w;
        t.data[1] = (float(rand()%594) / 594.f - 0.5f) * h;
        t.data[2] = (float(rand()%794) / 794.f - 0.5f) * d;
    }
    
    m_tree = new C3Tree;
	m_tree->setGridSize(1.51f);
    
    VertexP p;
    for(i = 0; i < NUMVERTEX; i++) {
        p.key = i;
        p.index = &m_pool[i];
        m_tree->insert(p);
    }
    
	std::cout<<"grid count "<<m_tree->size();
	m_tree->calculateBBox();
	
	m_march.initialize(m_tree->boundingBox(), m_tree->gridSize());
	
	m_active.threshold = 1.87f;
	m_active.gridSize = .9f;
}

GLWidget::~GLWidget()
{
    delete m_tree;
    delete[] m_pool;
}

void GLWidget::clientDraw()
{
	KdTreeDrawer * dr = getDrawer();
	BoundingBox bb;

	m_tree->begin();
	while(!m_tree->end()) {
		drawPoints(m_tree->verticesInGrid());
		m_tree->next();
	}
	
	bb = m_tree->boundingBox();
	dr->boundingBox(bb);
	
	dr->setColor(0.f, 1.f, .3f);
	drawPoints(m_active);
}

void GLWidget::drawPoints(const List<VertexP> * d) 
{
	if(!d) return;
	KdTreeDrawer * dr = getDrawer();
	dr->beginPoint(2.f);
	const int num = d->size();
	VertexP v;
	Vector3F p;
	for(int i = 0; i < num; i++) {
		V3 * v = d->value(i).index;
		p.set(v->data[0], v->data[1], v->data[2]);
		dr->vertex(p);
	}
	dr->end();
}

void GLWidget::drawPoints(const ActiveGroup & grp)
{
	Ordered<int, VertexP> * ps = grp.vertices;
	if(ps->size() < 1) return;
	const float mxr = grp.threshold * 2.f;
	ps->begin();
	while(!ps->end()) {
		const List<VertexP> * vs = ps->value();
		if((ps->key() - 1) * grp.gridSize - grp.depthMin > mxr) return;
		drawPoints(vs);
		ps->next();
	}
}

bool GLWidget::intersect(List<VertexP> * d, const Ray & ray, ActiveGroup & dst)
{
	if(!d) return false;
	const int num = d->size();
	const int ndst = dst.vertices->size();
	Vector3F p, pop;
	for(int i = 0; i < num; i++) {
		V3 * v = d->value(i).index;
		p.set(v->data[0], v->data[1], v->data[2]);
		float tt = ray.m_origin.dot(ray.m_dir) - p.dot(ray.m_dir);
		pop = ray.m_origin - ray.m_dir * tt;
		if(p.distanceTo(pop) < dst.threshold) {
			int k = -tt / dst.gridSize;
			dst.vertices->insert(k, d->value(i));
			if(-tt > dst.depthMax) dst.depthMax = -tt;
			if(-tt < dst.depthMin) dst.depthMin = -tt;
		}
	}
	return dst.vertices->size() > ndst;
}

void GLWidget::clientSelect(QMouseEvent */*event*/)
{
	selectPoints(getIncidentRay());
}

void GLWidget::clientDeselect(QMouseEvent */*event*/) 
{
	deselectPoints();
}

void GLWidget::clientMouseInput(QMouseEvent */*event*/)
{
	selectPoints(getIncidentRay());
}

void GLWidget::selectPoints(const Ray * incident)
{
	deselectPoints();
	
	Sequence<Coord3> added;
	if(!m_march.begin(*incident)) return;
	while(!m_march.end()) {
		const std::deque<Vector3F> coords = m_march.touched(m_active.gridSize);
		std::deque<Vector3F>::const_iterator it = coords.begin();
		for(; it != coords.end(); ++it) {
			const Coord3 c = m_tree->gridCoord((const float *)&(*it));
			if(added.find(c)) continue;
			added.insert(c);
			List<VertexP> * pl = m_tree->find((float *)&(*it));
			intersect(pl, *incident, m_active);
		}
		m_march.step();
	}
}

void GLWidget::deselectPoints() {m_active.reset();}
