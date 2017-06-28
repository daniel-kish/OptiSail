#include "pondview.h"
#include <QGraphicsScene>
#include <qdebug.h>
#include "sailboat.h"
#include <QGraphicsLineItem>
#include "environment.h"

PondView::PondView(QWidget *parent, qreal w, qreal h) :
    QGraphicsView(parent), Environment()
{
    scale(0.7, -0.7);
    QGraphicsScene * scene = new QGraphicsScene(this);

    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-0.5*w, -0.5*h, w, h);
    setScene(scene);
//    setCacheMode(QGraphicsView::CacheBackground);
//    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setMinimumSize(400, 400);
    setWindowTitle(tr("Pond"));

    SailBoat *boat = new SailBoat(this, 50, 25);
    boat->setZValue(0);
    boat->setPos(0.0, 0.0);
    scene->addItem(boat);
    boat->grabMouse();
    windAngle = M_PI_4;

    QRectF sceneRect = scene->sceneRect();
    QGraphicsItem* yAxis = new QGraphicsLineItem(QLineF(0.0, -sceneRect.height()*0.5,
                                                        0.0, sceneRect.height()*0.5));
    QGraphicsItem* xAxis = new QGraphicsLineItem(QLineF(-sceneRect.width()*0.5, 0.0,
                                                        sceneRect.height()*0.5, 0.0));
    yAxis->setZValue(-1);
    xAxis->setZValue(-1);
    scene->addItem(yAxis);
    scene->addItem(xAxis);
}


//void PondView::mousePressEvent(QMouseEvent *event)
//{

//}
