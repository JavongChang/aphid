/*
 *  ImageView.h
 *  mallard
 *
 *  Created by jian zhang on 12/29/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include <BaseServer.h>
#include <QWidget>
class Qimage;
class BaseServer;

class ImageView : public QWidget, public BaseServer
{
    Q_OBJECT

public:
    ImageView(QWidget *parent = 0);

protected:
	virtual void processRead(const char * data, size_t length);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private slots:
    
private:
	void beginBucket(const char * data);
	void processPackage(const char * data);
	void endBucket();
private:
	QImage * m_image;
	float * m_colors;
	int bucketRect[4];
	int m_packageStart;
};