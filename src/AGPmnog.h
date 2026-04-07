#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "../evolvent/evolvent.hpp"
#include"Grishagin/grishagin_function.hpp"
#include "GKLS/GKLSProblem.hpp"
#include"Point.h"
#include"Interval.h"

using namespace std;
enum ProblemType {
    PT_Grishagin = 0,
    PT_GKLS_Simple = 1,
    PT_GKLS_Hard = 2
};

class GlobalSearch
{
private:
    double r;
    double epsilon;
    int N;  // размерность задачи
    int m;  // параметр построения кривой Пеано (глубина)
    double a, b;
 
    vector<Point> points;
    vector<Interval> intervals;

    IOptProblem* problem;
    ags::Evolvent* evolvent; // Кривая Пеано

    Point min_point;
    vector<double> best_point_multidim; // лучшая многомерная точка
    size_t point_count;

    vector<double> true_opt_point;
    double true_opt_value;

    double mu;
    double M;

    ProblemType problemType; //пока не надо
/*
public:
    GlobalSearch(int function_number = 1, double epsilon_ = 0.001, double r_ = 3.0, int tightness_ = 10): epsilon(epsilon_), r(r_), m(tightness_)
    {
        problem = new TGrishaginProblem(function_number);
        N = problem->GetDimension();

        // Получаем границы
        vector<double> lb, ub;
        problem->GetBounds(lb, ub);

        evolvent = new ags::Evolvent(N, m, lb.data(), ub.data(), ags::Simple); //создаем кривую пеано

        best_point_multidim.resize(N);

        // Сохраняем точное решение
        vector<double> true_point = problem->GetOptimumPoint();
        true_opt_point = true_point;
        true_opt_value = problem->GetOptimumValue();

        min_point = Point(0, 1e100);

        // Добавляем граничные точки (t = 0 и t = 1)
        vector<double> point(N);
        // Точка t = 0
        evolvent->GetImage(0.0, point.data());
        double z0 = problem->ComputeFunction(point);
        points.push_back(Point(0.0, z0));

        if (z0 < min_point.z) {
            min_point = Point(0.0, z0);
            best_point_multidim = point;
        }

        // Точка t = 1
        evolvent->GetImage(1.0, point.data());
        double z1 = problem->ComputeFunction(point);
        points.push_back(Point(1.0, z1));

        if (z1 < min_point.z) {
            min_point = Point(1.0, z1);
            best_point_multidim = point;
        }

        // Сортируем точки и обновляем интервалы
        sortPoints();
        updateIntervals();

        point_count = points.size();
    }


    ~GlobalSearch()
    {
        delete problem;
        delete evolvent;
    }

    */


public:
    GlobalSearch(ProblemType type, int function_number, int dim = 2,
        double epsilon_ = 0.001, double r_ = 3.0, int tightness_ = 10)
        : epsilon(epsilon_), r(r_), m(tightness_), problemType(type)
    {
        // Создаём задачу в зависимости от типа
        if (type == PT_Grishagin) {
            problem = new TGrishaginProblem(function_number);
        }
        else if (type == PT_GKLS_Simple) {
            problem = new TGKLSProblem(function_number, dim, Simple, TD);
        }
        else if (type == PT_GKLS_Hard) {
            problem = new TGKLSProblem(function_number, dim, Hard, TD);
        }

        initializeProblem();
    }

    ~GlobalSearch()
    {
        delete problem;
        delete evolvent;
    }

private:
    void initializeProblem() {
        N = problem->GetDimension();

        // Получаем границы
        vector<double> lb, ub;
        problem->GetBounds(lb, ub);

        evolvent = new ags::Evolvent(N, m, lb.data(), ub.data(), ags::Simple);

        best_point_multidim.resize(N);

        // Сохраняем точное решение
        vector<double> true_point = problem->GetOptimumPoint();
        true_opt_point = true_point;
        true_opt_value = problem->GetOptimumValue();

        min_point = Point(0, 1e100);

        // Добавляем граничные точки (t = 0 и t = 1)
        vector<double> point(N);
        // Точка t = 0
        evolvent->GetImage(0.0, point.data());
        double z0 = problem->ComputeFunction(point);
        points.push_back(Point(0.0, z0));

        if (z0 < min_point.z) {
            min_point = Point(0.0, z0);
            best_point_multidim = point;
        }

        // Точка t = 1
        evolvent->GetImage(1.0, point.data());
        double z1 = problem->ComputeFunction(point);
        points.push_back(Point(1.0, z1));

        if (z1 < min_point.z) {
            min_point = Point(1.0, z1);
            best_point_multidim = point;
        }

        // Сортируем точки и обновляем интервалы
        sortPoints();
        updateIntervals();

        point_count = points.size();
    }

public:

    // Сортировка точек по x
    void sortPoints() {
        sort(points.begin(), points.end());
    }

    // Обновление интервалов после добавления новых точек
    void updateIntervals() {
        intervals.clear();
        sortPoints();

        for (size_t i = 0; i < points.size() - 1; i++) {
            Interval interval(points[i], points[i + 1], i);
            intervals.push_back(interval);
        }
    }

    //Вычисление mu
    double ocenkaConstL() {
        double M_val = 0.0;
        for (size_t i = 0; i < intervals.size(); i++) {
            double delta = intervals[i].getDelta(N);
            double delta_z = abs(intervals[i].right.z - intervals[i].left.z);
            double m_val = delta_z / delta;
            if (m_val > M_val) {
                M_val = m_val;
            }
        }

        if (M_val == 0.0) {
            M_val = 1.0;
        }
        return M_val;
    }

    vector<double> kharakteristika_R() {
        vector<double> R_values;
        mu = ocenkaConstL();
        M = r * mu;
        int k = intervals.size();

        for (int i = 0; i < k; i++) {
            double delta = intervals[i].getDelta(N);
            double z_i = intervals[i].right.z;
            double z_im1 = intervals[i].left.z;
            double R = 0.0;

            if (i == 0) {
                R = 2 * delta - 4 * z_i / M;
            }
            else if (i == k - 1) {
                R = 2 * delta - 4 * z_im1 / M;
            }
            else {
                double delta_z = z_i - z_im1;
                R = delta + (delta_z * delta_z) / (M * M * delta) - 2.0 * (z_i + z_im1) / M;
            }
            intervals[i].R = R;
            R_values.push_back(R);
        }
        return R_values;
    }

    //Поиск интервала с макс хар
    size_t findMaxRInterval() {
        size_t max_index = 0;
        double max_R = intervals[0].R;

        for (size_t i = 1; i < intervals.size(); i++) {
            if (intervals[i].R > max_R) {
                max_R = intervals[i].R;
                max_index = i;
            }
        }
        return max_index;
    }

    // Обновление точки минимума
    void updateMinimumPoint(const Point& new_point, const vector<double>& multidim_point) {
        if (new_point.z < min_point.z) {
            min_point = new_point;
            best_point_multidim = multidim_point;
        }
    }

    // Основной алгоритм
    void algorithm() {
        int iteration = 0;
        const int MAX_ITERATIONS = 10000000;
        const int MAX_POINTS = 10000000;

        while ((iteration < MAX_ITERATIONS) && (points.size() < MAX_POINTS)) {
            iteration++;
            updateIntervals();

            vector<double> R_values = kharakteristika_R();  //Вычисляем mu и R

            //Находим интервал с макс хар
            size_t t = findMaxRInterval();
            Interval& max_interval = intervals[t];

            //Вычисляем новую точку
            double new_x = max_interval.getNewPoint(mu, r, N, intervals.size());
            //значение функции в новой точке
            vector<double> multidim_point(N);
            evolvent->GetImage(new_x, multidim_point.data());
            double new_z = problem->ComputeFunction(multidim_point);

            Point new_point(new_x, new_z);
            points.push_back(new_point);

            // Обновляем точку мин
            updateMinimumPoint(new_point, multidim_point);

            // остановка
            double delta = max_interval.getDelta(N);
            if (delta <= epsilon) {
                break;
            }

        }

        point_count = points.size();
    }

    void getResults(vector<double>& x_min, double& f_min, size_t& count,
        vector<double>& true_x, double& true_f) {
        x_min = best_point_multidim;
        f_min = min_point.z;
        count = points.size();
        true_x = true_opt_point;
        true_f = true_opt_value;
    }


    void printResults()
    {
        cout << "\n========== РЕЗУЛЬТАТЫ ==========" << endl;
        cout << "Найденный минимум: f(";
        for (size_t i = 0; i < best_point_multidim.size(); i++)
        {
            cout << best_point_multidim[i];
            if (i < best_point_multidim.size() - 1) cout << ", ";
        }
        cout << ") = " << min_point.z << endl;
    }



    // Добавьте этот public метод в класс GlobalSearch
    void debugAlgorithm() {
        int iteration = 0;
        const int MAX_ITERATIONS = 100000000;
        const int MAX_POINTS = 100000000;

        cout << "Начало алгоритма, epsilon = " << epsilon << endl;

        while ((iteration < MAX_ITERATIONS) && (points.size() < MAX_POINTS)) {
            iteration++;
            updateIntervals();

            vector<double> R_values = kharakteristika_R();

            size_t t = findMaxRInterval();
            Interval& max_interval = intervals[t];

            double new_x = max_interval.getNewPoint(mu, r, N, intervals.size());
            vector<double> multidim_point(N);
            evolvent->GetImage(new_x, multidim_point.data());
            double new_z = problem->ComputeFunction(multidim_point);

            Point new_point(new_x, new_z);
            points.push_back(new_point);

            updateMinimumPoint(new_point, multidim_point);

            double delta = max_interval.getDelta(N);

            // Выводим информацию каждые 50 итераций
            if (iteration % 50 == 0 || iteration == 1) {
                cout << "Итер " << iteration
                    << ": delta=" << delta
                    << ", len=" << max_interval.length
                    << ", points=" << points.size()
                    << ", min_z=" << min_point.z << endl;
            }

            if (delta <= epsilon) {
                cout << "СТОП: delta=" << delta << " <= epsilon=" << epsilon << endl;
                break;
            }
        }

        point_count = points.size();
        cout << "Алгоритм завершен. Всего итераций: " << iteration << ", точек: " << point_count << endl;
    }
    void debugAlgorithmDetailed() {
        int iteration = 0;
        const int MAX_ITERATIONS = 5000;

        cout << "=== ДЕТАЛЬНАЯ ОТЛАДКА ===" << endl;
        cout << "N=" << N << ", r=" << r << ", epsilon=" << epsilon << endl;

        // Выводим начальные точки
        cout << "Начальные точки:" << endl;
        for (size_t i = 0; i < points.size(); i++) {
            cout << "  x=" << points[i].x << ", f=" << points[i].z << endl;
        }

        while (iteration < MAX_ITERATIONS) {
            iteration++;
            updateIntervals();

            vector<double> R_values = kharakteristika_R();
            size_t t = findMaxRInterval();
            Interval& max_interval = intervals[t];

            double delta = max_interval.getDelta(N);
            double delta_z = abs(max_interval.right.z - max_interval.left.z);

            cout << "\n--- Итерация " << iteration << " ---" << endl;
            cout << "Выбран интервал [" << max_interval.left.x << ", " << max_interval.right.x << "]";
            cout << ", длина=" << max_interval.length << ", delta=" << delta << endl;
            cout << "  left.z=" << max_interval.left.z << ", right.z=" << max_interval.right.z;
            cout << ", delta_z=" << delta_z << endl;
            cout << "  mu=" << mu << ", M=" << M << ", r=" << r << endl;

            double pow_val = pow(delta_z / max(mu, 1e-10), N);
            cout << "  (|dz|/mu)^N = " << pow_val << endl;

            double new_x = max_interval.getNewPoint(mu, r, N, intervals.size());
            cout << "  new_x = " << new_x << endl;

            // Проверяем, что new_x внутри интервала
            if (new_x <= max_interval.left.x || new_x >= max_interval.right.x) {
                cout << "  ВНИМАНИЕ: new_x вне интервала!" << endl;
            }

            vector<double> multidim_point(N);
            evolvent->GetImage(new_x, multidim_point.data());
            double new_z = problem->ComputeFunction(multidim_point);
            cout << "  new_z = " << new_z << endl;

            Point new_point(new_x, new_z);
            points.push_back(new_point);
            updateMinimumPoint(new_point, multidim_point);

            cout << "  текущий минимум = " << min_point.z << endl;
            cout << "  истинный минимум = " << true_opt_value << endl;

            if (delta <= epsilon) {
                cout << "СТОП: delta <= epsilon" << endl;
                break;
            }

            if (iteration >= 100) break;
        }

        cout << "\n=== РЕЗУЛЬТАТ ===" << endl;
        cout << "Итераций: " << iteration << endl;
        cout << "Точек: " << points.size() << endl;
        cout << "Найденный минимум: " << min_point.z << endl;
        cout << "Истинный минимум: " << true_opt_value << endl;
        cout << "Погрешность: " << abs(min_point.z - true_opt_value) << endl;
    }
    void printTrueInfo() {
        cout << "Истинное значение: " << true_opt_value << endl;
        cout << "Истинная точка: (" << true_opt_point[0];
        for (size_t i = 1; i < true_opt_point.size(); i++) {
            cout << ", " << true_opt_point[i];
        }
        cout << ")" << endl;
    }
    void checkBounds() {
        vector<double> lb, ub;
        problem->GetBounds(lb, ub);
        cout << "\n=== ПРОВЕРКА ГРАНИЦ ===" << endl;
        cout << "Нижняя граница: ";
        for (int i = 0; i < N; i++) cout << lb[i] << " ";
        cout << endl;
        cout << "Верхняя граница: ";
        for (int i = 0; i < N; i++) cout << ub[i] << " ";
        cout << endl;

        vector<double> point(N);
        evolvent->GetImage(0.0, point.data());
        cout << "x=0 -> (";
        for (int i = 0; i < N; i++) cout << point[i] << " ";
        cout << ")" << endl;

        evolvent->GetImage(1.0, point.data());
        cout << "x=1 -> (";
        for (int i = 0; i < N; i++) cout << point[i] << " ";
        cout << ")" << endl;
    }

    void checkTruePoint() {
        cout << "\n=== ПРОВЕРКА ИСТИННОЙ ТОЧКИ ===" << endl;
        cout << "Истинная точка: (";
        for (int i = 0; i < N; i++) cout << true_opt_point[i] << " ";
        cout << ")" << endl;
        cout << "Истинное значение: " << true_opt_value << endl;

        // Проверяем, что функция в истинной точке возвращает правильное значение
        double computed = problem->ComputeFunction(true_opt_point);
        cout << "ComputeFunction в истинной точке: " << computed << endl;
        cout << "Разница: " << abs(computed - true_opt_value) << endl;
    }

    void printFirstPoints(int count = 20) {
        cout << "\n=== ПЕРВЫЕ " << count << " ТОЧЕК ИСПЫТАНИЙ ===" << endl;
        for (size_t i = 0; i < min(points.size(), (size_t)count); i++) {
            vector<double> multidim(N);
            evolvent->GetImage(points[i].x, multidim.data());
            cout << "x=" << points[i].x << " -> (";
            for (int j = 0; j < N; j++) cout << multidim[j] << " ";
            cout << "), f=" << points[i].z << endl;
        }
    }
};