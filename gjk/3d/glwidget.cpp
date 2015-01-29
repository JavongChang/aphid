#include <QtGui>
#include <QtOpenGL>
#include <BaseCamera.h>
#include "glwidget.h"
#include <KdTreeDrawer.h>
#include <GjkContactSolver.h>
#include "SimpleSystem.h"
	
GLWidget::GLWidget(QWidget *parent) : Base3DView(parent)
{
	perspCamera()->setFarClipPlane(20000.f);
	perspCamera()->setNearClipPlane(1.f);
	orthoCamera()->setFarClipPlane(20000.f);
	orthoCamera()->setNearClipPlane(1.f);
	connect(internalTimer(), SIGNAL(timeout()), this, SLOT(update()));
	
    m_tetrahedron[0].set(-2.5f, -2.5f, -2.5f);
    m_tetrahedron[1].set(-2.5f, -2.5f, 2.5f);
    m_tetrahedron[2].set(2.f, -2.5f, -2.5f);
    m_tetrahedron[3].set(0.f, 2.5f, 0.f);
	
	m_system = new SimpleSystem;

    m_alpha = 0.f;
    m_drawLevel = 1;
    m_isRunning = 1;
}
//! [0]

//! [1]
GLWidget::~GLWidget()
{
}

void GLWidget::testLine()
{
    Matrix44F mat;
    Vector3F line[2];
    mat.rotateZ(m_alpha);
    mat.rotateX(m_alpha);
    mat.translate(10.f, 10.f, 1.f);
    
    line[0] = mat.transform(Vector3F(-3.f, 0.f, 4.f));
    line[1] = mat.transform(Vector3F(3.f, 0.f, -4.f));
	
	ClosestTestContext result;
	result.hasResult = 0;
	result.distance = 1e9;
	result.needContributes = 1;
	result.referencePoint = Vector3F(8.f, 8.f * sin(m_alpha) + 2.f, 4.f * cos(m_alpha) + 4.f);
	
	Simplex & s = result.W;
	s.d = 2;
	s.p[0] = line[0];
	s.p[1] = line[1];
    closestOnSimplex(&result);
    
	glBegin(GL_LINES);
    glColor3f(result.contributes.x, 1.f - result.contributes.x, 0.f);
    glVertex3f(line[0].x , line[0].y, line[0].z);
	glColor3f(result.contributes.y, 1.f - result.contributes.y, 0.f);
    glVertex3f(line[1].x , line[1].y, line[1].z);

	glColor3f(0.f, 1.f, 1.f);
    glVertex3f(result.referencePoint.x, result.referencePoint.y, result.referencePoint.z);
    Vector3F q = result.closestPoint;
    glVertex3f(q.x, q.y, q.z);
    glEnd();
}

void GLWidget::testTriangle()
{
	Matrix44F mat;
    mat.rotateZ(m_alpha);
    mat.rotateX(m_alpha);
    mat.translate(20.f, 10.f, 1.f);
	
	Vector3F tri[3];
    tri[0] = mat.transform(Vector3F(-4.f, 0.f,-4.f));
    tri[1] = mat.transform(Vector3F(4.f, 0.f, -4.f));
	tri[2] = mat.transform(Vector3F(0.f, 0.f, 4.f));
	
	ClosestTestContext result;
	result.hasResult = 0;
	result.distance = 1e9;
	result.needContributes = 1;
	result.referencePoint = Vector3F(20.f + 5.f * sin(-m_alpha), 4.f * cos(m_alpha) + 1.f, 0.f);
	Simplex & s = result.W;
	s.d = 3;
	s.p[0] = tri[0];
	s.p[1] = tri[1];
	s.p[2] = tri[2];
    closestOnSimplex(&result);
	
	glBegin(GL_LINES);
	glColor3f(0.f, 1.f, 1.f);
	glVertex3f(result.referencePoint.x, result.referencePoint.y, result.referencePoint.z);
    glVertex3f(result.closestPoint.x, result.closestPoint.y, result.closestPoint.z);
	glEnd();
	
	glBegin(GL_LINE_LOOP);
    glColor3f(result.contributes.x, 1.f - result.contributes.x, 0.f);
    glVertex3f(tri[0].x , tri[0].y, tri[0].z);
	glColor3f(result.contributes.y, 1.f - result.contributes.y, 0.f);
    glVertex3f(tri[1].x , tri[1].y, tri[1].z);
	glColor3f(result.contributes.z, 1.f - result.contributes.z, 0.f);
    glVertex3f(tri[2].x , tri[2].y, tri[2].z);
	glEnd();
}

void GLWidget::testTetrahedron()
{
    Matrix44F mat;
    mat.rotateZ(m_alpha);
    mat.rotateX(m_alpha);
    mat.translate(30.f, 10.f, 1.f);
    
    Vector3F q[4];
    for(int i = 0; i < 4; i++)
        q[i] = mat.transform(m_tetrahedron[i]);
		
	// q[0].set(2.05031,0.705789,-6.46018);
	// q[1].set(-5.69273,2.36286,-9.89069);
	// q[2].set(-2.90515,-2.54066,-10.3207);
	// q[3].set(-0.737269,5.6093,-6.03012);
			
	ClosestTestContext result;
	result.hasResult = 0;
	result.distance = 1e9;
	result.needContributes = 1;
	result.referencePoint = Vector3F(28.f + 1.f * sin(-m_alpha), 1.f * cos(-m_alpha) + 1.f, 1.f);
	// result.referencePoint = Vector3F(-5.69624,-0.751777,-11.0967);
	
	Simplex & s = result.W;
	s.d = 4;
	s.p[0] = q[0];
	s.p[1] = q[1];
	s.p[2] = q[2];
	s.p[3] = q[3];
    closestOnSimplex(&result);
	
    glBegin(GL_LINES);
    Vector3F p;

    glColor3f(0.f, 1.f, 1.f);
	glVertex3f(result.referencePoint.x, result.referencePoint.y, result.referencePoint.z);
    glColor3f(1.f, 1.f, 1.f);
	glVertex3f(result.closestPoint.x, result.closestPoint.y, result.closestPoint.z);
	
    //glVertex3f(0.f ,0.f, 0.f);
    //p = closestToOriginOnTetrahedron(q);
    //glVertex3f(p.x, p.y, p.z);
	
	Vector3F c[4];
	c[0].set(result.contributes.x, 1.f - result.contributes.x, 0.f);
    c[1].set(result.contributes.y, 1.f - result.contributes.y, 0.f);
    c[2].set(result.contributes.z, 1.f - result.contributes.z, 0.f);
    c[3].set(result.contributes.w, 1.f - result.contributes.w, 0.f);
   
// 0 - 1
	p = c[0];
	glColor3f(p.x, p.y, p.z);
    p = q[0];
    glVertex3f(p.x, p.y, p.z);
	
	p = c[1];
	glColor3f(p.x, p.y, p.z);
    p = q[1];
    glVertex3f(p.x, p.y, p.z);

// 1 - 2 
	p = c[1];
	glColor3f(p.x, p.y, p.z);
    p = q[1];
    glVertex3f(p.x, p.y, p.z);
	
	p = c[2];
	glColor3f(p.x, p.y, p.z);
    p = q[2];
    glVertex3f(p.x, p.y, p.z);
 
// 2 - 0
	glVertex3f(p.x, p.y, p.z);
	p = c[0];
	glColor3f(p.x, p.y, p.z);
    p = q[0];
    glVertex3f(p.x, p.y, p.z);

// 0 - 3
    glVertex3f(p.x, p.y, p.z);
	p = c[3];
	glColor3f(p.x, p.y, p.z);
    p = q[3];
    glVertex3f(p.x, p.y, p.z);
    
// 3 - 1    
    glVertex3f(p.x, p.y, p.z);
	p = c[1];
	glColor3f(p.x, p.y, p.z);
    p = q[1];
    glVertex3f(p.x, p.y, p.z);

// 3- 2
	p = c[3];
	glColor3f(p.x, p.y, p.z);
    p = q[3];
    glVertex3f(p.x, p.y, p.z);
	p = c[2];
	glColor3f(p.x, p.y, p.z);
    p = q[2];
    glVertex3f(p.x, p.y, p.z);
  
    glEnd();
}

void drawSimplex(const Simplex & s)
{
    if(s.d == 0) return;
    if(s.d == 1) {
        glColor3f(0.f, 1.f, 0.f);
        glBegin(GL_POINTS);
        glVertex3f(s.p[0].x, s.p[0].y, s.p[0].z);
        glEnd();
        return;
    }
    if(s.d == 2) {
        glColor3f(1.f, 1.f, 0.f);
        glBegin(GL_LINES);
        glVertex3f(s.p[0].x, s.p[0].y, s.p[0].z);
        glVertex3f(s.p[1].x, s.p[1].y, s.p[1].z);
        glEnd();
        return;
    }
    if(s.d == 3) {
        glColor3f(0.f, 1.f, 1.f);
        glBegin(GL_TRIANGLES);
        glVertex3f(s.p[0].x, s.p[0].y, s.p[0].z);
        glVertex3f(s.p[1].x, s.p[1].y, s.p[1].z);
        glVertex3f(s.p[2].x, s.p[2].y, s.p[2].z);
        glEnd();
        return;
    }
    if(s.d == 4) {
        glColor3f(1.f, 0.f, 1.f);
        glBegin(GL_LINES);
        glVertex3f(s.p[0].x, s.p[0].y, s.p[0].z);
        glVertex3f(s.p[1].x, s.p[1].y, s.p[1].z);
        glVertex3f(s.p[1].x, s.p[1].y, s.p[1].z);
        glVertex3f(s.p[2].x, s.p[2].y, s.p[2].z);
        glVertex3f(s.p[2].x, s.p[2].y, s.p[2].z);
        glVertex3f(s.p[0].x, s.p[0].y, s.p[0].z);
        glVertex3f(s.p[0].x, s.p[0].y, s.p[0].z);
        glVertex3f(s.p[3].x, s.p[3].y, s.p[3].z);
        glVertex3f(s.p[1].x, s.p[1].y, s.p[1].z);
        glVertex3f(s.p[3].x, s.p[3].y, s.p[3].z);
        glVertex3f(s.p[2].x, s.p[2].y, s.p[2].z);
        glVertex3f(s.p[3].x, s.p[3].y, s.p[3].z);
        glEnd();
        return;
    }
}

void GLWidget::testGjk()
{
    Vector3F pa[3]; 
    pa[0].set(-3.f, -2.f, -1.f);
	pa[1].set(3.f, -2.f, 1.f);
	pa[2].set(0.f, 2.f, 1.f);
	
	for(int i = 0; i < 3; i++)
	    A.X[i] = pa[i];
	
	Vector3F pb[3];
	pb[0].set(-2.f, -2.f, 1.f);
	pb[1].set(2.f, -2.f, 0.f);
	pb[2].set(3.f, 2.f, 0.f);
	
	for(int i = 0; i < 3; i++)
	    B.X[i] = pb[i];
	
	Matrix44F matA;
    matA.rotateZ(m_alpha);
    matA.rotateX(m_alpha);
    matA.translate(12.f, 2.f, 3.f);
		
	Matrix44F matB;
	matB.rotateZ(-m_alpha * .5f);
    matB.rotateY(-m_alpha);
    matB.translate(12.f + 3.f * sin(m_alpha * 2.f), 2.f, 3.f + 1.f * cos(m_alpha * 2.f));
		
	GjkContactSolver gjk;
	ClosestTestContext result;
	result.referencePoint.setZero();
	result.needContributes = 1;
	result.distance = 1e9;
	result.hasResult = 0;
	result.transformA = matA;
	result.transformB = matB;
	result.rayDirection.set(1.f, -1.f, 0.f);
	result.rayDirection.normalize();
	
	resetSimplex(result.W);
	
	gjk.distance(A, B, &result);

	float grey = 0.f;
	if(result.hasResult) grey = .3f;
    
    Vector3F q;
	
	glPushMatrix();
    getDrawer()->useSpace(matA);
	glBegin(GL_TRIANGLES);
    glColor3f(0.5f + grey, 0.5f ,0.f);
    q = A.X[0];
    glVertex3f(q.x, q.y, q.z);
    q = A.X[1];
    glVertex3f(q.x, q.y, q.z);
    q = A.X[2];
    glVertex3f(q.x, q.y, q.z);
	glEnd();
	
    glPopMatrix();
	
	glPushMatrix();
	getDrawer()->useSpace(matB);
	
	glBegin(GL_TRIANGLES);
    glColor3f(0.f, 0.5f + grey ,0.5f);
    q = B.X[0];
    glVertex3f(q.x, q.y, q.z);
    q = B.X[1];
    glVertex3f(q.x, q.y, q.z);
    q = B.X[2];
    glVertex3f(q.x, q.y, q.z);
    glEnd();
	
	glPopMatrix();
	
	if(result.hasResult) {
	    matB.translate(result.rayDirection.reversed() * result.penetrateDepth);
	    glPushMatrix();
		getDrawer()->useSpace(matB);
		glBegin(GL_TRIANGLES);
		glColor3f(0.f, 0.5f, 0.5f);
		q = B.X[0];
		glVertex3f(q.x, q.y, q.z);
		q = B.X[1];
		glVertex3f(q.x, q.y, q.z);
		q = B.X[2];
		glVertex3f(q.x, q.y, q.z);
		glEnd();
		glPopMatrix();
	}
	Vector3F wb = matB.transform(result.contactPointB);
	if(result.hasResult) glColor3f(1.f, 0.f, 0.f);
	else glColor3f(0.f, 0.f, 1.f);
	getDrawer()->arrow(wb, wb + result.contactNormal);
}

void GLWidget::testShapeCast()
{
	Vector3F pa[3]; 
    pa[0].set(-3.5f, -3.5f, 0.f);
	pa[1].set(3.5f, -3.5f, 0.f);
	pa[2].set(-2.5f, 3.5f, 0.f);
	
	for(int i = 0; i < 3; i++)
	    A.X[i] = pa[i];
	
	Vector3F pb[3];
	pb[0].set(-2.f, 2.f, 0.f);
	pb[1].set(2.f, 2.f, 0.f);
	pb[2].set(2.f, -2.f, 0.f);
	
	for(int i = 0; i < 3; i++)
	    B.X[i] = pb[i];
	
	Matrix44F matA;
    matA.rotateZ(sin(m_alpha)* 1.2f);
	matA.rotateX(-m_alpha);
	matA.translate(1.f, 1.f, -2.f);
	
		
	Matrix44F matB;
	matB.rotateZ(-m_alpha * 1.5f);
	matB.rotateY(0.2 * sin(m_alpha));
    matB.translate(1.f, 1.f, 8.f);
	
		
	Vector3F rayDir(.79f * sin(m_alpha * .5f), .23f * cos(m_alpha), -1.9f);
	rayDir.normalize();
	
	glPushMatrix();
    getDrawer()->useSpace(matA);
    
	glBegin(GL_TRIANGLES);
	
    Vector3F q;
    
    glColor3f(1.f, 0.f, 0.f);
    q = A.X[0];
    glVertex3f(q.x, q.y, q.z);
    q = A.X[1];
    glVertex3f(q.x, q.y, q.z);
    q = A.X[2];
    glVertex3f(q.x, q.y, q.z);
    
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
	getDrawer()->useSpace(matB);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glBegin(GL_TRIANGLES);
    glColor3f(0.f, 1.f, 0.f);
    q = B.X[0];
    glVertex3f(q.x, q.y, q.z);
    q = B.X[1];
    glVertex3f(q.x, q.y, q.z);
    q = B.X[2];
    glVertex3f(q.x, q.y, q.z);
    
    glEnd();
    glPopMatrix();
    
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	GjkContactSolver gjk;
	
	ClosestTestContext result;
	result.rayDirection = rayDir;
	result.referencePoint.setZero();
	result.needContributes = 1;
	result.distance = 1e9;
	result.hasResult = 0;
	result.transformA = matA;
	result.transformB = matB;
	
	resetSimplex(result.W);
	
	gjk.rayCast(A, B, &result);
	
	Vector3F rayBegin= B.X[0] * .33f + B.X[1] * .33f + B.X[2] * .33f;
	rayBegin = matB.transform(rayBegin);
	
	Vector3F rayEnd = rayBegin + rayDir * 20.f;
	
	glColor3f(0.f, 0.f, 0.f);
	glBegin(GL_LINES);
	glVertex3f(rayBegin.x, rayBegin.y, rayBegin.z);
	glVertex3f(rayEnd.x, rayEnd.y, rayEnd.z);
	glEnd();
	
	if(!result.hasResult) return;
	
	rayEnd = rayBegin + rayDir * result.distance;
	glColor3f(0.f, 0.35f, .24f);
	getDrawer()->arrow(rayBegin, rayEnd);
	
	glPushMatrix();
	matB.translate(result.rayDirection * result.distance);
	getDrawer()->useSpace(matB);
    
	glBegin(GL_TRIANGLES);
    
	glColor3f(0.f, 1.f, 0.f);
    q = B.X[0];
    glVertex3f(q.x, q.y, q.z);
    q = B.X[1];
    glVertex3f(q.x, q.y, q.z);
    q = B.X[2];
    glVertex3f(q.x, q.y, q.z);
    
    glEnd();
    
    glPopMatrix();
	
	rayBegin = matB.transform(result.contactPointB);
	rayEnd = rayBegin + result.contactNormal;
	glColor3f(0.f, 0.35f, 0.25f);
	getDrawer()->arrow(rayBegin, rayEnd);

}

void GLWidget::testCollision()
{
	glColor3f(0.f, 0.f, 0.5f);
	
    glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)m_system->Vline());
	glDrawElements(GL_LINES, m_system->numVlineVertices(), GL_UNSIGNED_INT, m_system->vlineIndices());

	glDisableClientState(GL_VERTEX_ARRAY);
	
	glColor3f(0.f, 0.5f, 0.f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
    glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)m_system->X());
	glDrawElements(GL_TRIANGLES, m_system->numFaceVertices(), GL_UNSIGNED_INT, m_system->indices());

	glDisableClientState(GL_VERTEX_ARRAY);
	
	glColor3f(0.5f, 0.f, 0.f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
    glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)m_system->groundX());
	glDrawElements(GL_TRIANGLES, m_system->numGroundFaceVertices(), GL_UNSIGNED_INT, m_system->groundIndices());

	glDisableClientState(GL_VERTEX_ARRAY);
}

void GLWidget::testRotation()
{
	Quaternion q;
	Vector3F axis(cos(m_alpha), sin(m_alpha), 0.f); 
	axis.normalize();
	float theta = .5f;
	q.set(theta, axis);
	q.normalize();
	
	Matrix33F mat;
	mat.set(q);
	
	Vector3F at(-13.f, 0.f, 0.f);
	
	getDrawer()->arrow(at, at + axis * 12.f);
	getDrawer()->coordsys(mat, 12.f, &at);
	
	RigidBody * rb = m_system->rb();
	at = rb->position;
	mat.set(rb->orientation);
	getDrawer()->coordsys(mat, 8.f, &at);
	CuboidShape * cub = static_cast<CuboidShape *>(rb->shape);
	
	glColor3f(0.f, 0.5f, 0.f);
	glPushMatrix();
	Matrix44F space;
	space.setRotation(mat);
	space.setTranslation(at);
	getDrawer()->useSpace(space);
	getDrawer()->aabb(Vector3F(-cub->m_w, -cub->m_h, -cub->m_d), Vector3F(cub->m_w, cub->m_h, cub->m_d));
	glPopMatrix();
	
	RigidBody * grd = m_system->ground();
	at = grd->position;
	mat.set(grd->orientation);
	TetrahedronShape * tet = static_cast<TetrahedronShape *>(grd->shape);
	
	glColor3f(0.f, 0.f, 0.5f);
	glPushMatrix();
	space.setRotation(mat);
	space.setTranslation(at);
	getDrawer()->useSpace(space);
	getDrawer()->tetrahedron(tet->p);
	glPopMatrix();
}

void GLWidget::clientDraw()
{
	testShapeCast();
	testGjk();
    testLine();
	testTriangle();
    testTetrahedron();
	// testCollision();
	// testRotation();
	// m_system->progress();
    if(m_isRunning) m_alpha += 0.01f;
}

void GLWidget::clientSelect(Vector3F & origin, Vector3F & ray, Vector3F & hit)
{
}
//! [9]

void GLWidget::clientDeselect()
{
}

//! [10]
void GLWidget::clientMouseInput(Vector3F & stir)
{
}
//! [10]

void GLWidget::simulate()
{
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
		case Qt::Key_A:
			m_drawLevel++;
			break;
		case Qt::Key_D:
			m_drawLevel--;
			break;
		case Qt::Key_S:
			if(m_isRunning)
			    m_isRunning = 0;
			else
			    m_isRunning = 1;
			break;
		default:
			break;
	}
	Base3DView::keyPressEvent(e);
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
	Base3DView::keyReleaseEvent(event);
}

