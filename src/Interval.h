#pragma once
#include <iostream>
#include"Point.h"
using namespace std;

class Interval
{
public:
    Point left;
    Point right;
    double length;
    double R;

    Interval(const Point& left_gran, const Point& right_gran) : left(left_gran), right(right_gran)
    {
        updateInterv();
    }

    void updateInterv()
    {
        length = right.x - left.x;
    }

    double getNewPoint(double mu, double r)
    {
        return (left.x + right.x) / 2.0 - (right.z - left.z) / (2.0 * mu * r);
        //double x_k1 = (x_values[t] + x_values[t + 1]) / 2.0 - (z_values[t + 1] - z_values[t]) / (2.0 * mu * r); было раньше в algorithm
    }

};