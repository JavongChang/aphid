/*
 *  BrushControl.h
 *  mallard
 *
 *  Created by jian zhang on 9/23/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef BRUSH_CONTROL_H
#define BRUSH_CONTROL_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QGroupBox;
class QStackedLayout;
QT_END_NAMESPACE

class QIntEditSlider;
class QDoubleEditSlider;

class BrushControl : public QDialog
{
    Q_OBJECT

public:
    BrushControl(QWidget *parent = 0);
	
	QWidget * numSamplesWidget();
	QWidget * radiusWidget();
	QWidget * pitchWidget();

public slots:
	void receiveToolContext(int c);
	
private slots:
	void sendBrushRadius(double d);
signals:
	void brushRadiusChanged(double d);
private:
	void createGroup();
	void eraseGroup();
	void combGroup();
	void scaleGroup();
	void bendGroup();
	
private:
	QStackedLayout * stackLayout;
	QGroupBox * controlsGroupC;
	QDoubleEditSlider * m_radiusValueC;
	QDoubleEditSlider * m_pitchValueC;
	QIntEditSlider * m_numSampleValueC;
	QGroupBox * controlsGroupE;
	QDoubleEditSlider * m_radiusValueE;
	QGroupBox * controlsGroupB;
	QDoubleEditSlider * m_radiusValueB;
	QGroupBox * controlsGroupS;
	QDoubleEditSlider * m_radiusValueS;
	QGroupBox * controlsGroupD;
	QDoubleEditSlider * m_radiusValueD;
};

#endif