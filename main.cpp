#include <QApplication>
#include "boatmodel.h"
#include "environment.h"
#include <qdebug.h>
#include <qmath.h>
#include <QVector>
#include <pondview.h>
#include <time.h>
#include <QwtPlot>
#include <qwt_plot_curve.h>
#include <QPolygonF>
#include <qglobal.h>
#include <QTime>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>

const double L = 0.05;
const double F = 1.0;

double f(QVector<double> &X){
    double res = 0.0;

    for (QVector<double>::iterator it = X.begin(); it != X.end(); it++)
        res += (*it - 3) * (*it - 3);

    return res;
}

double T(double t, double l){
    return 100.0*exp(-L*t) - 1;
}

double P(QVector<double>& next, QVector<double>& prev, double time){
    double fNext, fPrev;
    fNext = f(next);
    fPrev = f(prev);

    if (fNext < fPrev) return 1.0;
    else {
        return exp(-(fNext - fPrev) / T(time, L));
    }
}

double Test(BoatModel& model, QPointF dest, QVector<QPointF>& intermediates)
{
    model.initModel(QPointF(0.0, 0.0), 0.0);
    double t = 0;
    model.setHt(5);
    for (int i = 0; i < intermediates.size(); i++){
        t += model.moveTo(intermediates[i]);
    }
    t += model.moveTo(dest);
    return t;
}

double ellipticCurve(double y, double R, double factor){
    return factor * sqrt(R*R - y*y);
}

double randf(double max, double min)
{
    return min + double(qrand()) / (double(RAND_MAX/(max - min)));
}

void generate(QVector<QPointF>& intermediates, int n, QPointF dest, QPointF wind)
{
    double minX;
    double maxX;
    double minY = 0.0;
    double maxY = dest.y();
    double x;
    double y;
    QPointF d;
    QPointF cur;
    double s;
    double l;
    int m = 5;

    // parameters of the elliptic boundary
    double factor = F;
    double R = dest.y();

    y = randf(maxY, minY);
    minX = -ellipticCurve(y, R, factor);
    maxX = ellipticCurve(y, R, factor);
    if (wind.y() < 0) {
        cur.rx() = (wind.x() / wind.y()) * y; // restricted X-coordinate
        if (cur.x() > maxX || cur.x() < minX)
            cur.rx() = randf(maxX, minX);
        else {
            l = (maxX - minX)*0.3;
            x = cur.x() + randf(-1, 1) * l;
            cur = QPointF(x,y);
        }
    }
    else
        cur.rx() = randf(maxX, minX);
    intermediates << cur;
    minY = y;

    for (int i = 1; i < n; i++) {
        //        qDebug() << "for";
        y = minY + double(rand()) / (double(RAND_MAX/(maxY - minY)));
        minX = -ellipticCurve(y, R, factor);
        maxX = ellipticCurve(y, R, factor);

        if (wind.y() < 0) {
            cur.rx() = (wind.x() / wind.y()) * (y - intermediates[i-1].y()) +
                    intermediates[i-1].x(); // restricted X-coordinate
            if (cur.x() > maxX || cur.x() < minX)
                cur.rx() = minX + double(rand()) / (double(RAND_MAX/(maxX - minX)));
            else {
                l = (maxX - minX)*0.3;
                x = cur.x() + randf(-1, 1) * l;
                cur = QPointF(x,y);
            }
        }
        else
            cur.rx() = minX + double(rand()) / (double(RAND_MAX/(maxX - minX)));
        intermediates << cur;
        minY = y;
    }
    if (fabs(intermediates.last().x() - dest.x()) < (maxX - minX)*0.3)
        intermediates.last().rx() += randf(-1, 1) * (maxX - minX)*0.3;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Environment* env = new Environment;
    BoatModel model(env, 75, 10, 7, 2);
//    QwtPlot plot;
//    QPolygonF points;
//    QwtPlotCurve curve;
//    QwtPlotGrid* grid = new QwtPlotGrid;
//    grid->attach(&plot);

//    double t;
//    for (double x = -200; x < 200; x += 0.05) {
//        model.initModel(QPointF(0,0), 0);
//        t = model.moveTo(QPointF(x, 250));
//        t += model.moveTo(QPointF(0,500));
//        points << QPointF(x, t);
//    }
//    curve.setPen(Qt::blue, 1);
//    curve.setRenderHint(QwtPlotItem::RenderAntialiased, true);
//    curve.setSamples(points);
//    curve.setTitle("При у = 250");
//    curve.attach(&plot);

//    plot.setTitle("Время пути");
//    plot.setCanvasBackground(Qt::white);
//    plot.insertLegend(new QwtLegend);
//    plot.show();
//    return app.exec();

    QTime timer = QTime::currentTime();
    qsrand(uint(timer.msec()));

    double dist = 500;
    PondView view;
    view.resize(800, 600);
    view.setScene(new QGraphicsScene);

    QVector<QPointF> current;
    QVector<QPointF> best;

    int n = 3;
    double t = 0;
    QPointF dest(0, dist);
    QPointF wind;
    wind.rx() = env->windVector()[0];
    wind.ry() = env->windVector()[1];

    double bestTest, currentTest;
    double p, P;

    generate(best, n, dest, wind); // initial best solution
    bestTest = Test(model, dest, best);
    while (T(t,L) > 0) {
        generate(current, n, dest, wind);
        currentTest = Test(model, dest, current);
        p = double(rand()) / double(RAND_MAX);

        if (currentTest < bestTest)
            P = 1.0;
        else {
            P = exp(-(currentTest - bestTest) / T(t, L));
        }
        if (p < P){
            bestTest = currentTest;
            best = current;
        }
        t += 0.01;
        current.clear();
        qDebug() << bestTest << T(t,L);
    }

    view.scene()->addLine(QLineF(QPointF(0,0), best[0]));
    view.scene()->addLine(QLineF(best[best.size()-1], dest));
    double r = 8;
    for (int i = 1; i < best.size(); i++){
        view.scene()->addLine(QLineF(best[i-1],best[i]));
        view.scene()->addEllipse(QRectF(best[i-1]+QPointF(-r,r),
                                 best[i-1]+QPointF(r,-r)),

                QPen(QColor(Qt::black)),QBrush(QColor(Qt::blue))
                );
    }
    view.scene()->addEllipse(QRectF(best.last()+QPointF(-r,r),
                                    best.last()+QPointF(r,-r)),
                             QPen(QColor(Qt::black)),QBrush(QColor(Qt::blue)));

    QPolygonF ellipsePoints;
    ellipsePoints << QPointF(-500, 0) << QPointF(500, 0) << QPointF(500, 500)
                  << QPointF(-500, 500);
    QBrush brush;
    QPen pen;
    brush.setColor(QColor(Qt::red));
    pen.setColor(QColor(Qt::black));
    view.scene()->addPolygon(ellipsePoints, QPen(QBrush(QColor(Qt::red)), 2.0));
    r *= 0.5;
    view.scene()->addEllipse(QRectF(QPointF(-r,r),QPointF(r,-r)),
                             QPen(QColor(Qt::black)),QBrush(QColor(Qt::red)));
    view.scene()->addEllipse(QRectF(dest + QPointF(-r,r), dest - QPointF(-r,r)),

                             QPen(QColor(Qt::black)),QBrush(QColor(Qt::red)));
    QLineF line(QPointF(500,500), QPointF(500,500) + wind*10);
    view.scene()->addLine(line);
    QRectF rect(QPointF(500,500)+QPointF(-r, r), QPointF(500,500)+QPointF(r, -r));
    view.scene()->addEllipse(rect, QPen(QColor(Qt::black)), QBrush(QColor(Qt::green)));
    view.show();

    return app.exec();
//    return 0;
}
