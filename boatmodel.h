#ifndef CATBOAT_H
#define CATBOAT_H
#include "environment.h"
#include <valarray>
#include <QVector>
#include <QPair>
#include <QPointF>

using namespace std;

class BoatModel
{
public:
    BoatModel(Environment *penv, double _mass, double _sailArea, double aspectRatio, double _wetArea);
    double getVelocity() const{
        return velocity;
    }
    double getPosition() const {
        return position;
    }

    void setTack(double value);
    void setSailAngle(double value);
    void setHt(double value);

    double moveTo(QPointF dest);
    void moveTo(QPointF dest, QVector<QPair<QPointF, double> >& track);
    void initModel(QPointF pos, double);
    void startModeling(double timeLimit);
    void startModeling(int stepsCount);
    void nextStep1();

    double getTack() const{
        return tack;
    }
    double getModelTime() const{
        return modelTime;
    }

    QPointF getPosVec() const;

//protected:
    Environment* env;
    double mass;
    double sailArea;
    double wetArea;
    double sailAspectRatio;
    double liftSlope; // Cy(alpha) = liftSlope * alpha

    QPointF posVec;
    double position;
    double velocity;
    double modelTime;
    double tack;
    double sailAngle;
    valarray<double> sailVector();
    valarray<double> tackVector();

    double kAir;
    double kWater;

    void nextStep();
    double residue(double);
    double summaryForce(double);
    double ascensionalForce(double);
    double airResForce(double);
    double waterResForce(double);

    double ht;
    double epsylon;
};

double angle(valarray<double>, valarray<double>);
double mag(valarray<double>);
double mag(QPointF);
double sqmag(valarray<double>);
int sign(double);

#endif // CATBOAT_H
