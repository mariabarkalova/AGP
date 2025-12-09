#pragma once
#include <iostream>
using namespace std;


class Point
{
public:
    double x;
    double z;
    Point() : x(0), z(0) {}
    Point(double x_val, double z_val) : x(x_val), z(z_val) {}

    bool operator==(const Point& other) const
    {
        return x == other.x;
    }
    bool operator<(const Point& other) const
    {
        return x < other.x;
    }
};