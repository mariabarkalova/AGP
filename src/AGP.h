#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
//#include"Shekel/ShekelProblem.hpp"
#include"Hill/HillProblem.hpp"
using namespace std;

class AGP
{
    double mu;
    double r;
    double epsilon;
    double a, b;
    vector<double> x_values;
    vector<double> z_values;
    IOptProblem* problem;
    double min_x;
    double min_value;
    size_t point_count;
    double true_opt_point;
    double true_opt_value;

public:
    double objective_function(double x)
    {
        return problem->ComputeFunction({ x });
    }

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
        x_values.push_back(a);
        x_values.push_back(b);

        z_values.push_back(objective_function(a));
        z_values.push_back(objective_function(b));

        vector<double> true_point = problem->GetOptimumPoint();
        true_opt_point = true_point[0];
        true_opt_value = problem->GetOptimumValue();
    }
    ~AGP()
    {
        delete problem;
    }
    void getResults(double& x_min, double& f_min, size_t& count, double& true_x, double& true_f)
    {
        x_min = min_x;
        f_min = min_value;
        count = point_count;
        true_x = true_opt_point;
        true_f = true_opt_value;
    }

    double ocenkaConstL()
    {
        double M = 0.0;
        for (size_t i = 1; i < x_values.size(); i++)
        {
            double delta_x = x_values[i] - x_values[i - 1];
            double delta_z = abs(z_values[i] - z_values[i - 1]);
            double m = delta_z / delta_x;
            if (m > M)
            {
                M = m;
            }
            if (m == 0.0) {
                r = 1.0;
                M = 1.0;
            }
        }
        return M;
    }
    /*double ocenkaConstL()
    {
        double M = 0.0;
        for (size_t i = 1; i < x_values.size(); i++)
        {
            double delta_x = x_values[i] - x_values[i - 1];
            double delta_z = abs(z_values[i] - z_values[i - 1]);

            double m = delta_z / delta_x;
            if (m > M)
            {
                M = m;
            }
        }
        return M;
    }*/

    vector<double> kharakteristika_R()
    {
        vector<double> R_values;
        mu = ocenkaConstL();
        int k = x_values.size() - 1;  // кол-во интервалов

        for (int i = 1; i <= k; i++)
        {
            double delta_i = x_values[i] - x_values[i - 1];
            double z_i = z_values[i];
            double z_im1 = z_values[i - 1];
            double R = 0.0;

            if (i == 1)
            {
                R = 2 * delta_i - 4 * z_i / mu;
            }
            else if (i == k)
            {
                R = 2 * delta_i - 4 * z_im1 / mu;
            }
            else
            {
                double delta_z = z_i - z_im1;
                R = delta_i + (delta_z * delta_z) / (r * r * mu * mu * delta_i) - 2.0 * (z_i + z_im1) / (r * mu);
            }
            R_values.push_back(R);
        }
        return R_values;
    }

    void findMinimumPoint()
    {
        size_t min_index = 0;
        min_value = z_values[0];
        for (size_t i = 1; i < z_values.size(); i++)
        {
            if (z_values[i] < min_value) {
                min_value = z_values[i];
                min_index = i;
            }
        }
        min_x = x_values[min_index];
        point_count = x_values.size();
    }
    
    void algorithm()
    {
        int iteration = 0;
        const int MAX_ITERATIONS = 50000; 
        const int MAX_POINTS = 5000; 

        while ((iteration < MAX_ITERATIONS) && (x_values.size() < MAX_POINTS))
        {
            iteration++;
            vector<double> R_values = kharakteristika_R();

            double max_val = R_values[0];
            size_t t = 0;
            for (size_t i = 0;i < R_values.size(); i++)
            {
                if (R_values[i] > max_val)
                {
                    max_val = R_values[i];
                    t = i;
                }
            }

            double x_k1 = (x_values[t] + x_values[t + 1]) / 2.0 - (z_values[t + 1] - z_values[t]) / (2.0 * mu * r);
            double z_k1 = objective_function(x_k1);    // Зн-е функции в новой точке
            
            // Вставляем новую точку и зн-е ф-и в вектора
            x_values.insert(x_values.begin() + t + 1, x_k1);
            sort(x_values.begin(), x_values.end());

            z_values.insert(z_values.begin() + t + 1, z_k1);

            //усл-е остановки
            if (x_values[t + 1] - x_values[t] <= epsilon)
            {
                findMinimumPoint();
                break;
            }
        }
    }
};
