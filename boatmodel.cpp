#include "boatmodel.h"
#include "environment.h"
#include <qmath.h>
#include <qdebug.h>
#include <iostream>
#include <QTime>

using namespace std;

BoatModel::BoatModel(Environment* penv, double _mass, double _sailArea,
                     double aspectRatio, double _wetArea) :
    env(penv), mass(_mass), sailArea(_sailArea), wetArea(_wetArea), sailAspectRatio(aspectRatio)
{
    position = 0.0;
    velocity = 0.0;
    tack = M_PI_2;
    sailAngle = M_PI_2;
    modelTime = 0.0;

    kAir = 0.5 * env->airDensity * sailArea;
    kWater = 0.5 * env->waterDensity * wetArea * env->frictionFactor;
    liftSlope = fabs(M_PI / (1 - 2/sailAspectRatio));

    epsylon = 1.0e-3;
    ht = 1.0;
}


void BoatModel::setTack(double value)
{
    tack = value;
}

void BoatModel::setSailAngle(double value)
{
    sailAngle = value;
}

void BoatModel::setHt(double value)
{
    ht = value;
}

double BoatModel::moveTo(QPointF dest)
{
    double factor;
    QPointF diff = dest - posVec;
    double newTack = atan2(diff.y(), diff.x());
    double turnTime = 7.059*fabs(velocity) + 16.477*fabs(newTack - tack);
    double delta = fabs(newTack - tack);
    if (delta < M_PI_4)
        factor = 0;
    else if (delta < M_PI_2)
        factor = 0.7;
    else if (delta < M_PI)
        factor = 0.3;
    else
        factor = 0.0;

    velocity *= factor;
    tack = newTack;

    double maxForce = summaryForce(velocity);
    double force;
    double maxAngle = sailAngle;
    for (double sa = M_PI_2; sa <= 3*M_PI_2; sa += 0.017*0.5) {
        sailAngle = sa;
        force = summaryForce(velocity);
        if (force > maxForce){
            maxAngle = sa;
            maxForce = force;
        }
    }
    sailAngle = maxAngle;
//    qDebug() << "to" << dest << "from" << posVec << tack << velocity << sailAngle;

    QVector<double> currentState(5);
    currentState[0] = position;
    currentState[1] = velocity;
    currentState[2] = modelTime;
    currentState[3] = posVec.x();
    currentState[4] = posVec.y();

    while ( mag(dest - posVec) > 1.0){
        nextStep1();
        if (velocity < 0) {
//            qDebug() << "unable to reach dest";
            return 10.0e12;
        }
        if ( QPointF::dotProduct(diff, dest - posVec) < 0 ) {  // if we've gone too far
            // restore model state
            position = currentState[0];
            velocity = currentState[1];
            modelTime = currentState[2];
            posVec.rx() = currentState[3];
            posVec.ry() = currentState[4];
            ht *= 0.5; // halve the step length
            continue;
        }
        else if (fabs(currentState[1] - velocity) < 1.0e-1)
        {
            double s = mag(dest - posVec); // distance to go
            double timeRemain = s / fabs(velocity);
            modelTime += timeRemain;
            posVec = dest; // sorry
            position += s;
            break;
        }
        else {
            currentState[0] = position;
            currentState[1] = velocity;
            currentState[2] = modelTime;
            currentState[3] = posVec.rx();
            currentState[4] = posVec.ry();
        }
//        qDebug() << modelTime << posVec << velocity;
    }
//    qDebug() << "end" << posVec << modelTime;
    modelTime += turnTime;
    return modelTime;
}

void BoatModel::moveTo(QPointF dest, QVector<QPair<QPointF, double> >& track)
{
    if (track.size() != 0) track.clear();
    // 1) define tack as a direction from current position to destination
    QPointF diff = dest - posVec;
    tack = atan2(diff.y(), diff.x());

    // 2) optimize sail angle to reach it
    double maxForce = summaryForce(velocity);
    double force;
    double maxAngle = sailAngle;
    for (double sa = M_PI_2; sa <= 3*M_PI_2; sa += 0.017) {
        sailAngle = sa;
        force = summaryForce(velocity);
        if (force > maxForce){
            maxAngle = sa;
            maxForce = force;
        }
    }
    qDebug() << posVec << dest;
    sailAngle = maxAngle;
    QVector<double> currentState(5);
    currentState[0] = position;
    currentState[1] = velocity;
    currentState[2] = modelTime;
    currentState[3] = posVec.x();
    currentState[4] = posVec.y();

    while ( mag(dest - posVec) > 1.0 ){
        nextStep1();
        if ( QPointF::dotProduct(diff, dest - posVec) < 0 ) {  // if we've gone too far
            // restore model state
            position = currentState[0];
            velocity = currentState[1];
            modelTime = currentState[2];
            posVec.rx() = currentState[3];
            posVec.ry() = currentState[4];
            ht *= 0.5; // halve the step length
            continue;
        }
        else {
            track << QPair<QPointF, double> (posVec, modelTime);
        }
        //        qDebug() << posVec << modelTime;
    }

}

int sign(double a)
{
    return int(a / fabs(a));
}

void BoatModel::nextStep1()
{
    double v0 = summaryForce(velocity)*ht / mass + velocity;
//    qDebug() <<"v0 =" << v0;
    int n = 0;

//    qDebug() << modelTime << velocity << tack << ht;
    while(fabs(residue(v0)) > epsylon) {
        v0 = v0 - residue(v0) / ((residue(v0+0.00001) - residue(v0))/0.00001);
        n++;
        if (n > 50) {
            qDebug() << modelTime << ht << posVec << velocity << tack << sailAngle;
            exit(5);
        }
    }
    modelTime += ht;
    velocity = v0;
    position += v0 * ht;
    posVec.rx() += velocity*ht*cos(tack);
    posVec.ry() += velocity*ht*sin(tack);
}
QPointF BoatModel::getPosVec() const
{
    return posVec;
}

double BoatModel::residue(double v)
{
    return (summaryForce(v) / mass)*ht + velocity - v;
}

valarray<double> BoatModel::tackVector()
{
    valarray<double> v(2);
    v[0] = cos(tack);
    v[1] = sin(tack);
    return v;
}

valarray<double> BoatModel::sailVector()
{
    valarray<double> v(2);
    v[0] = cos(tack - sailAngle);
    v[1] = sin(tack - sailAngle);
    return v;
}

void BoatModel::initModel(QPointF pos, double initVel)
{
    modelTime = 0.0;
    position = 0.0;
    velocity = initVel;
    posVec.rx() = pos.x();
    posVec.ry() = pos.y();
}

void BoatModel::startModeling(double timeLimit)
{
    int i=0;
    qDebug() << "step " << i << ":" << posVec << velocity;

    while (modelTime <= timeLimit){
        nextStep1();
        i++;
        //        qDebug() << "step " << i << ":" << posVec << velocity << summaryForce(velocity);
    }
}

void BoatModel::startModeling(int stepsCount)
{
    startModeling(ht*(stepsCount-1));
}

double BoatModel::airResForce(double velocityValue)
{
    valarray<double> velocityVector(2);
    double attackAngle = angle(sailVector(), env->windVector());

    velocityVector[0] = velocityValue * cos(tack);
    velocityVector[1] = velocityValue * sin(tack);

    valarray<double> forceVector = kAir * sin(attackAngle) *
            mag(env->windVector() - velocityVector) * (env->windVector() - velocityVector);

    return (forceVector * tackVector()).sum();
}

double BoatModel::waterResForce(double velocityValue)
{
    return kWater * fabs(velocityValue) * 0.1*fabs(velocityValue) * (-velocityValue);
}

double BoatModel::ascensionalForce(double velocityValue)
{

    double attackAngle = angle(sailVector(), env->windVector());

    //    cout << "angle: " << attackAngle << endl;
    if (fabs(attackAngle) < 0.26 /*|| fabs(attackAngle - M_PI) < 0.26*/) {  // 15 deg


        double forceAngle;
        valarray<double> testv(2);
        double absSailAngle = tack - sailAngle;

        valarray<double> velocityVector(2);

        velocityVector[0] = velocityValue * cos(tack);
        velocityVector[1] = velocityValue * sin(tack);

        testv[0] = cos(absSailAngle + M_PI_2);
        testv[1] = sin(absSailAngle + M_PI_2);

        if ((testv * env->windVector()).sum() > 0) {
            forceAngle = absSailAngle + M_PI_2;
        } else {
            forceAngle = absSailAngle - M_PI_2;
        }

        testv[0] = cos(forceAngle);
        testv[1] = sin(forceAngle);

        testv *= kAir * sqmag(env->windVector() - velocityVector) *
                liftSlope * attackAngle;

        return (testv*tackVector()).sum();
    }
    else return 0.0;
}

double BoatModel::summaryForce(double v)
{
    return airResForce(v) + waterResForce(v) + ascensionalForce(v);
}

double angle(valarray<double> v1, valarray<double> v2)
{
    //    qDebug() << acos( (v1*v2).sum() / (sqrt(v1[0]*v1[0] + v1[1]*v1[1]) * sqrt(v2[0]*v2[0] + v2[1]*v2[1])) );
    return acos( (v1*v2).sum() / (sqrt(v1[0]*v1[0] + v1[1]*v1[1]) * sqrt(v2[0]*v2[0] + v2[1]*v2[1])) );
}

double mag(valarray<double> v)
{
    return sqrt((v*v).sum());
}

double mag(QPointF vec)
{
    return sqrt( vec.x()*vec.x() + vec.y()*vec.y() );
}

double sqmag(valarray<double> v)
{
    return ((v*v).sum());
}
