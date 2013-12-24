#include "BarbView.h"
#include <QtGui>
#include <MlFeather.h>
#include <MlFeatherCollection.h>
#include <KdTreeDrawer.h>
#include <PseudoNoise.h>
#include "MlVane.h"
BarbView::BarbView(QWidget *parent) : Base3DView(parent)
{
	std::cout<<"Barbview ";
	m_numLines = 202;
	m_numVerticesPerLine = new unsigned[m_numLines];
	for(unsigned i = 0; i < m_numLines; i++) m_numVerticesPerLine[i] = 11;
	m_vertices = new Vector3F[m_numLines * 11];
	m_colors = new Vector3F[m_numLines * 11];
	m_seed = 99;
	m_numSeparate = 9;
	m_separateStrength = 0.5f;
}

BarbView::~BarbView()
{
    delete[] m_numVerticesPerLine;
	delete[] m_vertices;
	delete[] m_colors;
}

void BarbView::clientDraw()
{
    if(!FeatherLibrary) return;
	getDrawer()->lineStripes(m_numLines, m_numVerticesPerLine, m_vertices, m_colors);
}

void BarbView::clientSelect()
{

}

void BarbView::clientMouseInput()
{
	
}

void BarbView::receiveShapeChanged()
{
	MlFeather *f = FeatherLibrary->selectedFeatherExample();
    if(!f) return;
	
	float * dst = f->angles();
	const short ns = f->numSegment();
	for(short s=0; s < ns; s++) {
		dst[s] = -0.4f * s / (float)ns;
	}
	f->bend();
	f->updateVane();
	f->setSeed(m_seed);
	f->setNumSeparate(m_numSeparate);
	f->setSeparateStrength(m_separateStrength);
	f->sampleColor(100, 10, m_colors);
	f->samplePosition(100, 10, m_vertices);

	update();
}

void BarbView::receiveSeed(int s)
{
	m_seed = s;
	receiveShapeChanged();
}

void BarbView::receiveNumSeparate(int n)
{
	m_numSeparate = n;
	receiveShapeChanged();
}

void BarbView::receiveSeparateStrength(double k)
{
	m_separateStrength = k;
	receiveShapeChanged();
}