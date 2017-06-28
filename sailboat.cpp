#include "sailboat.h"

#include <QPainter>
#include <qmath.h>
#include <QGraphicsItemAnimation>
#include <QTimeLine>
#include "qdebug.h"
#include <QGraphicsSceneEvent>

SailBoat::SailBoat(PondView* p, double l, double w) :  BoatModel(new Environment, 75, 10, 7, 2),
    pond(p), length(l), width(w)
{
    initModel(QPointF(0.0, 0.0), 0.0);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);
    setPos(0.0, 0.0);
    setRotation(-90);
}


QRectF SailBoat::boundingRect() const
{
    return QRectF(QPointF(-0.5*width, 0.5*length), QPointF(width, -0.5*length));
}

QPainterPath SailBoat::shape() const
{
    // key points defining nose and feed of the boat
    QPointF leftFeed(-width*0.5*0.7, -length*0.5);
    QPointF rightFeed(width*0.5*0.7, -length*0.5);
    QPointF nose(0.0, length*0.5);

    QPainterPath path;
    path.moveTo(rightFeed);
    path.lineTo(leftFeed);

    // to define cubic splines forming boat's boards we should specify
    // control points to define derivatives near the end points
    double der1, der2; //derivative magnitudes
    double ang1, ang2; // derivative angles
    der1 = width*0.9;
    der2 = width*0.5;
    ang1 =  M_PI_4*1.6;
    ang2 = -M_PI_4*1.2;

    QPointF ctrl1(der1*cos(ang1), der1*sin(ang1));
    QPointF ctrl2(der2*cos(ang2), der2*sin(ang2));

    ctrl1 += rightFeed;
    ctrl2 += leftFeed;
    path.cubicTo(ctrl1, ctrl2, nose);
    ctrl2.rx() *= -1;
    ctrl1.rx() *= -1;
    path.cubicTo(ctrl2, ctrl1, leftFeed);

    // now we'll draw a sail
    double alpha = -sailAngle + M_PI_2;
    QPointF sailEnd(width*cos(alpha), width*sin(alpha));
    path.moveTo(0.0, 0.0);
    path.lineTo(sailEnd);

    return path;
}

void SailBoat::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    QPen newPen(QBrush(Qt::SolidPattern), width / 30.0,
                Qt::SolidLine,Qt::RoundCap,Qt::MiterJoin);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(179, 89, 50));

    painter->setPen(newPen);
    painter->setBrush(brush);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPath(makePath());

    painter->restore();
}

QPainterPath SailBoat::makePath()
{
    // key points defining nose and feed of the boat
    QPointF leftFeed(-width*0.5*0.7, -length*0.5);
    QPointF rightFeed(width*0.5*0.7, -length*0.5);
    QPointF nose(0.0, length*0.5);

    QPainterPath path;
    path.moveTo(leftFeed);
    path.lineTo(rightFeed);

    // to define cubic splines forming boat's boards we should specify
    // control points to define derivatives near the end points
    double der1, der2; //derivative magnitudes
    double ang1, ang2; // derivative angles
    der1 = width*0.9;
    der2 = width*0.5;
    ang1 =  M_PI_4*1.6;
    ang2 = -M_PI_4*1.2;

    QPointF ctrl1(der1*cos(ang1), der1*sin(ang1));
    QPointF ctrl2(der2*cos(ang2), der2*sin(ang2));

    ctrl1 += rightFeed;
    ctrl2 += nose;
    path.cubicTo(ctrl1, ctrl2, nose);
    ctrl2.rx() *= -1;
    ctrl1.rx() *= -1;
    path.cubicTo(ctrl2, ctrl1, leftFeed);

    // now we'll draw a sail
    double alpha = -sailAngle + M_PI_2;
    QPointF sailEnd(width*cos(alpha), width*sin(alpha));
    path.moveTo(0.0, 0.0);
    path.lineTo(sailEnd);

    return path;
}


void SailBoat::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    double T; // total in secs
    QGraphicsItemAnimation* anim = new QGraphicsItemAnimation;
    QTimeLine* timer;

    anim->setItem(this);

    sailAngle = M_PI_2;
    ht = 0.5;
    tack = M_PI_2;

    QVector<QPair<QPointF,double> > track;
    QPointF dest(mapToScene(event->lastPos()));

    moveTo(dest, track);
    if (track.size() == 0) {
        return;
    }
    QVector<QPair<QPointF,double> >::iterator it;

    T = track.last().second;
    double factor = 0.1;
    timer = new QTimeLine(T*1000*factor);
    anim->setTimeLine(timer);

    for (it = track.begin(); it != track.end(); it++) {
        anim->setPosAt((it->second / T), it->first);
        anim->setRotationAt((it->second / T), (tack / M_PI)*180.0);
    }

    QRectF rect(QPointF(dest.x()-5, dest.y()+5),
                QPointF(dest.x()+5, dest.y()-5));
    scene()->addEllipse(rect, QPen(), QBrush(Qt::red));
    timer->start();
}
