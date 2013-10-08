/*
 *  TimeControl.h
 *  mallard
 *
 *  Created by jian zhang on 10/8/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
 
#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QGroupBox;
class QScrollBar;
class QSpinBox;
class QLabel;
QT_END_NAMESPACE

class QIntEditSlider;

class TimeControl : public QDialog
{
    Q_OBJECT

public:
    TimeControl(QWidget *parent = 0);
	
	void setFrameRange(int mn, int mx);
	int rangeMin() const;
	int rangeMax() const;
	int currentFrame() const;
	
public slots:
	void updateCurrentFrame(int x);
	
private slots:
	void setPlayMin();
	void setPlayMax();
	
signals:
	void currentFrameChanged(int a);
	
private:
	QGroupBox *minGroup;
	QGroupBox *playGroup;
	QGroupBox *maxGroup;
	QScrollBar * bar;
	QLabel * maxLabel;
	QLabel * minLabel;
	QSpinBox * minSpin;
	QSpinBox * maxSpin;
	QSpinBox * currentSpin;
	
	int m_rangeMin, m_rangeMax;
};
