#ifndef SAILBOAT_H
#define SAILBOAT_H

#include <QGraphicsItem>
#include "pondview.h"
#include "boatmodel.h"

class SailBoat : public QGraphicsItem, public BoatModel
{
public:
    SailBoat(PondView* pond, double l = 100, double w = 50);

    // QGraphicsItem interface
public:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath makePath();

private:
    PondView* pond;
    double length;
    double width;

    // QGraphicsItem interface
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
};

#endif // SAILBOAT_H
