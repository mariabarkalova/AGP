/*#pragma once
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

};*/


/*
#pragma once
#include "Point.h"
#include <cmath>
using namespace std;

class Interval
{
public:
    Point left;
    Point right;
    double length;
    double R;
    int index;

    Interval() : left(Point(0, 0)), right(Point(1, 0)), length(1.0), R(0.0), index(0) {}

    Interval(const Point& left_gran, const Point& right_gran, int idx = 0)
        : left(left_gran), right(right_gran), index(idx)
    {
        updateInterv();
    }

    void updateInterv()
    {
        length = right.x - left.x;
    }

    double getDelta(int N) const {
        return pow(length, 1.0 / N);
    }

    double getNewPoint(double mu, double r, int N, int total_intervals) const
    {
        if (index == 0 || index == total_intervals - 1){ // для первого и последнего интервала
            return (left.x + right.x) / 2.0;
        }
        else {
            double diff = right.z - left.z;
            double sign = (diff > 0) ? 1.0 : ((diff < 0) ? -1.0 : 0.0);
            double absDiff = abs(diff);

            if (mu == 0) mu = 1.0;

            double term = pow(absDiff / mu, N) / (2.0 * r);
            return (left.x + right.x) / 2.0 - sign * term;
        }
    }
};*/




#pragma once
#include "Point.h"
#include <cmath>
#include <algorithm>
using namespace std;

class Interval
{
public:
    Point left;
    Point right;
    double length;
    double R;
    int index;

    Interval() : left(Point(0, 0)), right(Point(1, 0)), length(1.0), R(0.0), index(0) {}

    Interval(const Point& left_gran, const Point& right_gran, int idx = 0)
        : left(left_gran), right(right_gran), index(idx)
    {
        updateInterv();
    }

    void updateInterv()
    {
        length = right.x - left.x;
    }

    double getDelta(int N) const {
        return pow(length, 1.0 / N);
    }

    double getNewPoint(double mu, double r, int N, int total_intervals) const
    {
        // Для граничных интервалов - точка в середине
        if (index == 0 || index == total_intervals - 1) {
            return (left.x + right.x) / 2.0;
        }

        // Для внутренних интервалов
        double diff = right.z - left.z;
        double delta_z = abs(diff);

        // Защита от деления на ноль
        if (mu < 1e-10) mu = 1.0;

        // Знак: если diff > 0, то sign = +1, иначе -1
        double sign = (diff > 0) ? 1.0 : -1.0;

        // Формула из статьи:
        // x_new = (x_left + x_right)/2 - sign * (1/(2r)) * (|z_right - z_left| / mu)^N
        double pow_val = pow(delta_z / mu, N);
        double new_x = (left.x + right.x) / 2.0 - sign * (1.0 / (2.0 * r)) * pow_val;

        // Ограничиваем точку, чтобы она не выходила за пределы интервала
        new_x = max(left.x + 1e-10, min(right.x - 1e-10, new_x));

        return new_x;
    }
};