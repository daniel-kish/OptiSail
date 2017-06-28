#include "environment.h"

valarray<double> Environment::windVector() const
{
    valarray<double> v(2);
    v[0] = windVelocity * cos(windAngle);
    v[1] = windVelocity * sin(windAngle);
    return v;
}

Environment::Environment()
{
    windAngle = -M_PI_2;
    windVelocity = 10;
    waterDensity = 1000;
    airDensity = 1.225;
    frictionFactor = 0.002;
}
