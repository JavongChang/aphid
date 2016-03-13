#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>
#include <CubeRender.h>

QT_BEGIN_NAMESPACE
class QImage;
QT_END_NAMESPACE

class RenderThread : public QThread
{
    Q_OBJECT

public:
    RenderThread(QObject *parent = 0);
    ~RenderThread();

	void setR(CubeRender * r);
    void render(QSize resultSize);

signals:
    void renderedImage(const QImage &image);

protected:
    void run();

private:
    QMutex mutex;
    QWaitCondition condition;

    QSize m_resultSize, m_portSize;
    bool restart;
    bool abort;
    
	CubeRender * m_r;
};

#endif
