#ifndef PONDVIEW_H
#define PONDVIEW_H

#include <QGraphicsview>
#include <QGraphicsScene>
#include "environment.h"

class PondView : public QGraphicsView, public Environment
{
    Q_OBJECT
public:
    PondView(QWidget *parent = 0, qreal w = 800, qreal h = 800);

signals:

    // QWidget interface
protected:
//    void mousePressEvent(QMouseEvent *);
};

#endif // PONDVIEW_H
