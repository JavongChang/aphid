#include "BccWorld.h"
#include <BezierCurve.h>
#include <GeoDrawer.h>
#include "BccGrid.h"
#include <BaseBuffer.h>
#include <CurveGroup.h>
#include "bcc_common.h"
#include <line_math.h>
#include "BccGlobal.h"
#include <HesperisFile.h>
BccWorld::BccWorld(GeoDrawer * drawer)
{
	m_drawer = drawer;
	m_curves = new CurveGroup;
	if(!readCurvesFromFile())
		createTestCurves();
		
	qDebug()<<" n curves "<<m_curves->numCurves();
		
	m_numSplines = 0;
	unsigned * cc = m_curves->counts();
	unsigned j, i=0;
	for(; i < m_curves->numCurves(); i++)
		m_numSplines += cc[i] - 1;
		
	std::cout<<" n spines "<<m_numSplines;
		
	m_splineBuf = new BaseBuffer;
	m_curveStartBuf = new BaseBuffer;
	m_curveStartBuf->create(m_curves->numCurves() * 12);
	Vector3F * curveStart = (Vector3F *)m_curveStartBuf->data();
	
	m_splineBuf->create(m_numSplines * sizeof(BezierSpline));
	BezierSpline * spline = (BezierSpline *)m_splineBuf->data();
	Vector3F * cvs = m_curves->points();
	
	unsigned nsp;
	unsigned ispline = 0;
	unsigned cvDrift = 0;
	for(i=0; i < m_curves->numCurves(); i++) {
		nsp = cc[i] - 1;
		curveStart[i] = cvs[cvDrift];
		for(j=0; j < nsp; j++) {			
			BezierCurve::extractSpline(spline[ispline], j, &cvs[cvDrift], nsp - 1);
			ispline++;
		}
		cvDrift += nsp + 1;
	}
	
    BoundingBox box;
    for(i=0; i < m_numSplines; i++)
		spline[i].getAabb(&box);
		
	std::cout<<" bbox "<<box.str();
	
    m_grid = new BccGrid(box); std::cout<<" finest grid size "<<(m_grid->span() / (float)(1<<6));
    m_grid->create(spline, m_numSplines, 6);
	
    std::cout<<" n tetrahedrons "<<m_grid->numTetrahedrons()<<"\n";
	std::cout<<" n vertices "<<m_grid->numTetrahedronVertices()<<"\n";
	
    createMeshData(m_grid->numTetrahedrons(), m_grid->numTetrahedronVertices());

	std::cout<<" add anchor points\n";
	m_grid->addAnchors((unsigned *)m_mesh.m_anchorBuf->data(), curveStart, m_curves->numCurves());
	m_grid->extractTetrahedronMeshData((Vector3F *)m_mesh.m_pointBuf->data(), (unsigned *)m_mesh.m_indexBuf->data());
	std::cout<<" done\n";
}

BccWorld::~BccWorld() 
{
	delete m_splineBuf;
}

void BccWorld::createTestCurves()
{
	qDebug()<<" gen test curve";
	m_curves->create(1, 9);
	m_curves->counts()[0] = 9;
	
	Vector3F * cvs = m_curves->points();
	cvs[0].set(8.f + RandomFn11(), 1.f + RandomFn11(), 4.1f);
    cvs[1].set(2.f + RandomFn11(), 9.4f + RandomFn11(), 1.11f);
    cvs[2].set(14.f + RandomFn11(), 8.4f + RandomFn11(), -3.13f);
    cvs[3].set(12.f + RandomFn11(), 1.4f + RandomFn11(), 1.14f);
    cvs[4].set(19.f + RandomFn11(), 2.4f + RandomFn11(), 2.16f);
    cvs[5].set(20.f + RandomFn11(), 3.4f + RandomFn11(), 5.17f);
    cvs[6].set(18.f + RandomFn11(), 12.2f + RandomFn11(), 3.18f);
    cvs[7].set(12.f + RandomFn11(), 12.2f + RandomFn11(), 2.19f);
    cvs[8].set(13.f + RandomFn11(), 8.2f + RandomFn11(), -2.18f);
    
    for(unsigned i=0; i<9;i++) {
        cvs[i] -= Vector3F(12.f, 0.f, 0.f);
        cvs[i] *= 3.f;
    }
}
 
void BccWorld::draw()
{
    BoundingBox box;
    m_grid->getBounding(box);
    
    glColor3f(.21f, .21f, .21f);
    m_drawer->boundingBox(box);
    
    m_grid->draw(m_drawer, (unsigned *)m_mesh.m_anchorBuf->data());

	drawMesh();
	drawAnchor();
    
	glDisable(GL_DEPTH_TEST);
    glColor3f(.59f, .02f, 0.f);
    drawCurves();
	drawCurveStars();
}

void BccWorld::testDistanctToCurve()
{
    Vector3F cls;
    float minD = 1e8;
	BezierSpline * spline = (BezierSpline *)m_splineBuf->data();
    unsigned i;
    for(i=0; i < m_numSplines; i++) {
        testDistanceToPoint(spline[i], m_testP, minD, cls);
    }
    
    m_drawer->setColor(.1f, .9f, 0.f);
    m_drawer->arrow(m_testP, cls);
}

void BccWorld::testDistanceToPoint(BezierSpline & spline, const Vector3F & pnt, float & minDistance, Vector3F & closestP)
{
	BoundingBox box;
	box.expandBy(spline.cv[0]);
	box.expandBy(spline.cv[1]);
	box.expandBy(spline.cv[2]);
	box.expandBy(spline.cv[3]);
	
	if(box.distanceTo(pnt) > minDistance) return;
	
    float paramMin = 0.f;
    float paramMax = 1.f;
    Vector3F line[2];
    
    line[0] = spline.calculateBezierPoint(paramMin);
    line[1] = spline.calculateBezierPoint(paramMax);
    
    Vector3F pol;
    float t;
    for(;;) {
        glColor3f(1.f, .9f, .0f);
        glBegin(GL_LINES);
        glVertex3fv((GLfloat *)&line[0]);
        glVertex3fv((GLfloat *)&line[1]);
        glEnd();
        
        float d = closestDistanceToLine(line, pnt, pol, t);
        
        const float tt = paramMin * (1.f - t) + paramMax * t;
        
// end of line
        if((tt <= 0.f || tt >= 1.f) && paramMax - paramMin < 0.1f) {
            if(d < minDistance) {
                minDistance = d;
                closestP = pol;
            }
            break;
        }
        
        const float h = .5f * (paramMax - paramMin);

// small enough       
        if(h < 1e-3) {
            if(d < minDistance) {
                minDistance = d;
                closestP = pol;
            }
            break;
        }
        
        if(t > .5f)
            paramMin = tt - h * ((t - .5f)/.5f * .5f + .5f);
        else
            paramMax = tt + h * ((.5f - t)/.5f * .5f + .5f);
            
        line[0] = spline.calculateBezierPoint(paramMin);
        line[1] = spline.calculateBezierPoint(paramMax);
    }
}

void BccWorld::moveTestP(float x, float y, float z)
{ m_testP += Vector3F(x, y, z);}

void BccWorld::testSpline()
{
	BezierSpline * spline = (BezierSpline *)m_splineBuf->data();
	BezierSpline a, b;
	unsigned i, j;
	Vector3F p;

	for(i=0; i< m_numSplines; i++) {
		glColor3f(.1f, .1f, .1f);
		glBegin(GL_LINE_STRIP);
		glVertex3fv((GLfloat *)&spline[i].cv[0]);
		glVertex3fv((GLfloat *)&spline[i].cv[1]);
		glVertex3fv((GLfloat *)&spline[i].cv[2]);
		glVertex3fv((GLfloat *)&spline[i].cv[3]);
		glEnd();
		
		glColor3f(.6f, .1f, .51f);
		glBegin(GL_LINE_STRIP);
		for(j=0; j<11; j++) {
			p = spline[i].calculateBezierPoint(0.1f * j);
			glVertex3fv((GLfloat *)&p);
		}
		glEnd();
		
		spline[i].deCasteljauSplit(a, b);
		
		glColor3f(.6f, .71f, .1f);
		glBegin(GL_LINE_STRIP);
		for(j=0; j<11; j++) {
			p = a.calculateBezierPoint(0.1f * j);
			glVertex3fv((GLfloat *)&p);
		}
		glEnd();
		
		glColor3f(.1f, .71f, .1f);
		glBegin(GL_LINE_STRIP);
		for(j=0; j<11; j++) {
			p = b.calculateBezierPoint(0.1f * j);
			glVertex3fv((GLfloat *)&p);
		}
		glEnd();
	}
}

void BccWorld::testIntersection()
{
	BoundingBox box;
    box.setMin(m_testP.x - 1.f, m_testP.y - 1.f, m_testP.z - 1.f);
	box.setMax(m_testP.x + 1.f, m_testP.y + 1.f, m_testP.z + 1.f);
	
	bool intersected = 0;
	
	BezierSpline * spline = (BezierSpline *)m_splineBuf->data();
    unsigned i;
    for(i=0; i < m_numSplines; i++) {
        intersected = testIntersection(spline[i], box);
		if(intersected) break;
    }
	
	m_drawer->boundingBox(box);
}

bool BccWorld::testIntersection(BezierSpline & spline, const BoundingBox & box)
{
	BoundingBox abox;
	abox.expandBy(spline.cv[0]);
	abox.expandBy(spline.cv[1]);
	abox.expandBy(spline.cv[2]);
	abox.expandBy(spline.cv[3]);
	
	if(!abox.intersect(box)) return false;
	
	if(abox.inside(box)) return true;
	
	BezierSpline stack[64];
	int stackSize = 2;
	spline.deCasteljauSplit(stack[0], stack[1]);
	
	while(stackSize > 0) {
		BezierSpline c = stack[stackSize - 1];
		stackSize--;
		
		abox.reset();
		abox.expandBy(c.cv[0]);
		abox.expandBy(c.cv[1]);
		abox.expandBy(c.cv[2]);
		abox.expandBy(c.cv[3]);
		
		m_drawer->boundingBox(abox);
		
		if(abox.inside(box)) return true;
		
		if(abox.intersect(box)) {
			if(abox.area() < 0.007f) return true;
			
			BezierSpline a, b;
			c.deCasteljauSplit(a, b);
			
			stack[ stackSize ] = a;
			stackSize++;
			stack[ stackSize ] = b;
			stackSize++;
		}
	}
	
	return false;
}

void BccWorld::testTetrahedronBoxIntersection()
{
    Vector3F p[4];
    p[0] = m_testP + Vector3F(0.f, -1.f, 0.f);
    p[1] = m_testP + Vector3F(.5f, 0.f, -.5f);
    p[2] = m_testP + Vector3F(-.5f, 0.f, -.5f);
    p[3] = m_testP + Vector3F(0.f, 1.f, 0.f);
    
    BoundingBox tetbox;
    tetbox.expandBy(p[0]);
    tetbox.expandBy(p[1]);
    tetbox.expandBy(p[2]);
    tetbox.expandBy(p[3]);
    
    Vector3F line0(1.f, 1.f, 1.f);
    Vector3F line1(10.f, 10.f, 4.f);
    
    
    PluckerCoordinate pier, pies;
    pier.set(line0, line1);
    
    pies.set(p[0], p[1]);
    // std::cout<<" r dot s01 "<<pier.dot(pies)<<" ";
    
    pies.set(p[1], p[2]);
    // std::cout<<" r dot s12 "<<pier.dot(pies)<<" ";
    
    pies.set(p[2], p[0]);
    // std::cout<<" r dot s20 "<<pier.dot(pies)<<"\n";
    
    Vector3F q;
    // if(tetrahedronLineIntersection(p, line0, line1, q))
    if(intersectTetrahedron(p))
    // if(m_curve->intersectTetrahedron(p))
        glColor3f(1.f, 0.f, 0.f);
    else 
        glColor3f(0.f, .3f, .1f);
    
    glBegin(GL_TRIANGLES);
    int i;
    for(i=0; i< 12; i++) {
        glVertex3fv((GLfloat *)&p[TetrahedronToTriangleVertex[i]]);
    }
    glEnd();
    
    m_drawer->arrow(line0, line1);
    m_drawer->arrow(line0, q);
}

bool BccWorld::intersectTetrahedron(Vector3F * p)
{
    BezierSpline * spline = (BezierSpline *)m_splineBuf->data();
    unsigned i;
    for(i=0; i < m_numSplines; i++) {
        if(intersectTetrahedron(spline[i], p)) return 1;
    }
    return 0;
}

bool BccWorld::intersectTetrahedron(BezierSpline & spline, Vector3F * p)
{
    BoundingBox tbox;
    tbox.expandBy(p[0]);
	tbox.expandBy(p[1]);
	tbox.expandBy(p[2]);
	tbox.expandBy(p[3]);
    
    BoundingBox abox;
	abox.expandBy(spline.cv[0]);
	abox.expandBy(spline.cv[1]);
	abox.expandBy(spline.cv[2]);
	abox.expandBy(spline.cv[3]);
	
	if(!abox.intersect(tbox)) return false;
	
	BezierSpline stack[64];
	int stackSize = 2;
	spline.deCasteljauSplit(stack[0], stack[1]);
	Vector3F q;
	while(stackSize > 0) {
		BezierSpline c = stack[stackSize - 1];
		stackSize--;
		
		abox.reset();
		abox.expandBy(c.cv[0]);
		abox.expandBy(c.cv[1]);
		abox.expandBy(c.cv[2]);
		abox.expandBy(c.cv[3]);
		
		if(abox.intersect(tbox)) {
			if(c.straightEnough()) {
			    m_drawer->boundingBox(abox);
			    glBegin(GL_LINE_STRIP);
			    glVertex3fv((GLfloat *)&c.cv[0]);
			    glVertex3fv((GLfloat *)&c.cv[1]);
			    glVertex3fv((GLfloat *)&c.cv[2]);
			    glVertex3fv((GLfloat *)&c.cv[3]);
			    glEnd();
			    
			    if(tetrahedronLineIntersection(p, c.cv[0], c.cv[3], q))
			        return true;
			}
			else {
                BezierSpline a, b;
                c.deCasteljauSplit(a, b);
                
                stack[ stackSize ] = a;
                stackSize++;
                stack[ stackSize ] = b;
                stackSize++;
			}
		}
	}
	
	return false;
}

void BccWorld::testLineLine()
{
    Vector3F line0(1.f, 11.f, 0.f);
    Vector3F line1(10.f, 11.f, 0.f);
    
    Vector3F line2(-4.f, 10.f, 1.f);
    Vector3F line3 = m_testP;//(10.f, 10.f, 1.f);
    
    if(areIntersectedOrParallelLines(line0, line1, line2, line3)) {
        if(areParallelLines(line0, line1, line2, line3))
            glColor3f(0.f, 1.f, 0.f);
        else 
            glColor3f(1.f, 0.f, 0.f);
    }
    else
        glColor3f(0.f, 0.f, 0.3f);
    
    m_drawer->arrow(line0, line1);
    m_drawer->arrow(line2, line3);
    // std::cout<<" d "<<distanceBetweenLines(line0, line1, line2, line3)<<" ";
}

void BccWorld::drawCurves()
{
	BezierSpline * spline = (BezierSpline *)m_splineBuf->data();
	unsigned i=0;
	for(;i<m_numSplines;i++)
		m_drawer->smoothCurve(spline[i], 16);
}

void BccWorld::drawCurveStars()
{
	const float csz = m_grid->span() / 1024.f;
	m_drawer->setWired(0);
	Vector3F * curveStart = (Vector3F *)m_curveStartBuf->data();
	unsigned i=0;
	for(; i < m_curves->numCurves(); i++)
		m_drawer->cube(curveStart[i], csz);
}

bool BccWorld::readCurvesFromFile()
{
	if(BaseFile::InvalidFilename(BccGlobal::FileName)) 
		return false;
	
	if(!BaseFile::FileExists(BccGlobal::FileName)) {
		BccGlobal::FileName = "unknown";
		return false;
	}
	HesperisFile hes;
	hes.setReadComponent(HesperisFile::RCurve);
	hes.addCurve("curves", m_curves);
	if(!hes.open(BccGlobal::FileName)) return false;
	hes.close();
	
	return true;
}

void BccWorld::createMeshData(unsigned nt, unsigned nv)
{
    m_mesh.m_numTetrahedrons = nt;
    m_mesh.m_numPoints = nv;
	m_mesh.m_anchorBuf = new BaseBuffer;
	m_mesh.m_anchorBuf->create(nv * 4);
	m_mesh.m_pointBuf = new BaseBuffer;
	m_mesh.m_pointBuf->create(nv * 12);
	m_mesh.m_indexBuf = new BaseBuffer;
	m_mesh.m_indexBuf->create(nt * 4 * 4);
	
	resetAnchors(nv);
}

void BccWorld::resetAnchors(unsigned n)
{
	unsigned * anchor = (unsigned *)m_mesh.m_anchorBuf->data();
	unsigned i=0;
	for(; i < n; i++)
		anchor[i] = 0;
}

void BccWorld::drawMesh()
{
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.3f, 0.4f, 0.33f);
	drawMesh(m_mesh.m_numTetrahedrons, (Vector3F *)m_mesh.m_pointBuf->data(),
	         (unsigned *)m_mesh.m_indexBuf->data());

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(.03f, .14f, .44f);
    drawMesh(m_mesh.m_numTetrahedrons, (Vector3F *)m_mesh.m_pointBuf->data(),
	         (unsigned *)m_mesh.m_indexBuf->data());
}

void BccWorld::drawMesh(unsigned nt, Vector3F * points, unsigned * indices)
{
    glBegin(GL_TRIANGLES);
    unsigned i, j;
    Vector3F q;
    unsigned * tet;
    for(i=0; i< nt; i++) {
        tet = &indices[i*4];
        for(j=0; j< 12; j++) {
            q = points[ tet[ TetrahedronToTriangleVertex[j] ] ];
            glVertex3fv((GLfloat *)&q);
        }
    }
    glEnd();
}

void BccWorld::drawAnchor()
{
    const float csz = m_grid->span() / 1024.f;
	m_drawer->setWired(0);
	unsigned nt = m_mesh.m_numTetrahedrons;
    Vector3F * p = (Vector3F *)m_mesh.m_pointBuf->data();
    unsigned * a = (unsigned *)m_mesh.m_anchorBuf->data();
    unsigned * t = (unsigned *)m_mesh.m_indexBuf->data();
    unsigned i, j;
    Vector3F q;
    glColor3f(.59f, .21f, 0.f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
    for(i=0; i< nt; i++) {
        bool anchored = 1;
        unsigned * tet = &t[i*4];
        for(j=0; j<4; j++) {
            if(a[tet[j]] < 1) {
                anchored = 0;
                break;
            }
        }
        if(!anchored) continue;
        for(j=0; j< 12; j++) {
            q = p[ tet[ TetrahedronToTriangleVertex[j] ] ];
            glVertex3fv((GLfloat *)&q);
        }
    }
    glEnd();
}

bool BccWorld::save()
{
	if(BaseFile::InvalidFilename(BccGlobal::FileName)) {
		std::cout<<" no specifc file to save\n";
		return false;
	}
	
	HesperisFile hes;
	hes.setWriteComponent(HesperisFile::WTetra);
	hes.addTetrahedron("tetra", &m_mesh);
	if(!hes.open(BccGlobal::FileName)) return false;
	hes.setDirty();
	hes.save();
	hes.close();
	return true;
}
//:~