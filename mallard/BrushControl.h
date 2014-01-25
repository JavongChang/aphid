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
class QCheckBox;
QT_END_NAMESPACE

class QIntEditSlider;
class QDoubleEditSlider;
class SelectFaceBox;
class CombBox;
class CurlBox;
class ScaleBox;
class FloodBox;
class EraseBox;
class PaintBox;

class BrushControl : public QDialog
{
    Q_OBJECT

public:
    BrushControl(QWidget *parent = 0);
	QWidget * floodControlWidget();
	
public slots:
	void receiveToolContext(int c);
	
private slots:
	void sendBrushRadius(double d);
	void sendBrushStrength(double d);
	void sendBrushTwoSided(int x);
	void sendBrushFilterByColor(int x);
signals:
	void brushRadiusChanged(double d);
	void brushStrengthChanged(double d);
	void brushTwoSidedChanged(int x);
	void brushFilterByColorChanged(int x);
private:
	QStackedLayout * stackLayout;
	SelectFaceBox * selectFace;
	CombBox * comb;
	CurlBox * curl;
	ScaleBox * brushScale;
	FloodBox * flood;
	EraseBox * eraseControl;
	PaintBox * paintControl;
};
#endif