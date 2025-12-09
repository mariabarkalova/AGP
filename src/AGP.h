#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include"Hill/HillProblem.hpp"
#include"Point.h"
#include"Interval.h"
using namespace std;

class AGP
{
    double mu;
    double r;
    double epsilon;
    double a, b;
    vector<Point> points;
    vector<Interval> intervals;
    IOptProblem* problem;
    Point min_point;
    size_t point_count;
    double true_opt_point;
    double true_opt_value;

public:

    AGP(double epsilon_, double a_ = 0, double b_ = 0, int function_number = 0, double r_ = 2.0) : epsilon(epsilon_), a(a_), b(b_), r(r_)
    {
        problem = new THillProblem(function_number);
        if (a == 0 && b == 0)
        {
            vector<double> lb;
            vector<double> ub;
            problem->GetBounds(lb, ub);
            a = lb[0];
            b = ub[0];
        }

        double z1 = problem->ComputeFunction({a});
        double z2 = problem->ComputeFunction({b});
        Point p1(a, z1);
        Point p2(b, z2);
        points.push_back(p1);
        points.push_back(p2);

        vector<double> true_point = problem->GetOptimumPoint();
        true_opt_point = true_point[0];
        true_opt_value = problem->GetOptimumValue();
    }

    ~AGP()
    {
        delete problem;
    }

    void updateIntervals() {
        intervals.clear();
        sort(points.begin(), points.end());

        for (size_t i = 0; i < points.size() - 1; i++) 
        {
            Interval interval(points[i], points[i + 1]);
            intervals.push_back(interval);
        }
    }

    void getResults(double& x_min, double& f_min, size_t& count, double& true_x, double& true_f) 
    {
        x_min = min_point.x;
        f_min = min_point.z;
        count = point_count;
        true_x = true_opt_point;
        true_f = true_opt_value;
    }
  
    double ocenkaConstL() 
    {
        double M = 0.0;
        for (size_t i = 0; i < intervals.size(); i++) 
        {
            double delta_x = intervals[i].length;
            double delta_z = abs(intervals[i].right.z - intervals[i].left.z);
            double m = delta_z / delta_x;
            if (m > M)
            {
                M = m;
            }
            if (m == 0.0)
            {
                r = 1.0;
                M = 1.0;
            }
        }
        return M;
    }

    vector<double> kharakteristika_R()
    {
        vector<double> R_values;
        mu = ocenkaConstL();
        int k = intervals.size();  // кол-во интервалов

        for (int i = 0; i < k; i++)
        {
            //double delta_i = points[i].x - points[i - 1].x;
            //double z_i = points[i].z;
            //double z_im1 = points[i - 1].z;
            //double R = 0.0;
            double delta_i = intervals[i].length;
            double z_i = intervals[i].right.z;
            double z_im1 = intervals[i].left.z;
            double R = 0.0;
            if (i == 0) 
            {
                R = 2 * delta_i - 4 * z_i / mu;
            }
            else if (i == k-1)
            {
                R = 2 * delta_i - 4 * z_im1 / mu;
            }
            else
            {
                double delta_z = z_i - z_im1;
                R = delta_i + (delta_z * delta_z) / (r * r * mu * mu * delta_i) - 2.0 * (z_i + z_im1) / (r * mu);
            }
            intervals[i].R = R;
            R_values.push_back(R);
        }
        return R_values;
    }

    //инетрвал с макс R
    size_t findMaxRInterval() 
    {
        size_t max_index = 0;
        double max_R = intervals[0].R;

        for (size_t i = 1; i < intervals.size(); i++) 
        {
            if (intervals[i].R > max_R) 
            {
                max_R = intervals[i].R;
                max_index = i;
            }
        }
        return max_index;
    }

    void findMinimumPoint()
    {
        size_t min_index = 0;
        min_point = points[0];
        for (size_t i = 1; i < points.size(); i++)
        {
            if (points[i].z < min_point.z) 
            {
                min_point = points[i];
                min_index = i;
            }
        }
        point_count = points.size();
    }

    void algorithm()
    {
        int iteration = 0;
        const int MAX_ITERATIONS = 50000; 
        const int MAX_POINTS = 5000; 

        while ((iteration < MAX_ITERATIONS) && (points.size() < MAX_POINTS))
        {
            iteration++;
            updateIntervals(); //длина интер
            vector<double> R_values = kharakteristika_R();
            size_t t = findMaxRInterval();
            Interval& max_interval = intervals[t];

            double new_x = max_interval.getNewPoint(mu, r);
            double new_z = problem->ComputeFunction({new_x});
            Point new_point(new_x, new_z);
            points.push_back(new_point);
            sort(points.begin(), points.end());
            // Обн мин точки
            if (new_point.z < min_point.z) {
                min_point = new_point;
            }
            //усл-е остановки
            if (max_interval.length <= epsilon) 
            {
                break;
            }
        }
        findMinimumPoint();
    }
};
