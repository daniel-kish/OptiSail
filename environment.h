#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <valarray>
using namespace std;

class Environment
{
public:
    Environment();
    double windAngle;
    double windVelocity;
    double waterDensity;
    double airDensity;
    double frictionFactor;
    valarray<double> windVector() const;
};

#endif // ENVIRONMENT_H
