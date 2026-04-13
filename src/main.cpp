/*#include<iostream>
#include<fstream>
#include"AGP.h"
using namespace std;

int main()
{
    setlocale(LC_ALL, "Russian");
    double a, b;
    double epsilon = 0.001;

    ofstream file("results.csv");
    if (!file.is_open())
    {
        cout << "Ошибка с файлом";
        return 1;
    }
    file << "Номер ф-ции; Точка минимума; Знач ф-ции; Кол-во точек; Ист.знач.минимума; Ист.знач.ф-ции; Погрешность по x;Погрешность по f " << endl;
    double min_x, min_value;
    size_t point_count;
    double true_x, true_f;

    for (int function_number = 0; function_number < 999; function_number++)
    {
        cout << "Обработка функции " << function_number << endl;

        AGP method(epsilon, 0, 0, function_number);
        method.algorithm();
        method.getResults(min_x, min_value, point_count, true_x, true_f);

        double error_x = abs(min_x - true_x);
        double error_f = abs(min_value - true_f);

        file << function_number << ";" << min_x << ";" << min_value << ";"
            << point_count << ";" << true_x << ";" << true_f << ";"
            << error_x << ";" << error_f << endl;

        cout << "Функция " << function_number << " завершена" << endl;
    }

    file.close();
    cout << "Файл успешно создан и заполнен";
    return 0;
}*/


/*
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include "AGPmnog.h"
#include "Grishagin/grishagin_function.hpp"
using namespace std;

int main()
{
    setlocale(LC_ALL, "Russian");

    ofstream file("results_grishagin.csv");
    if (!file.is_open())
    {
        cout << "Ошибка создания файла" << endl;
        return 1;
    }
    file << "Номер;Найденный x;Найденный y;Найденный f;Точек;Ист. x;Ист. y;Ист. f;Погр x;Погр y;Погр f" << endl;

    double r_param = 2.7;
    double epsilon = 0.01;
    int tightness = 10;
    double total_error_x = 0;
    double total_error_y = 0;
    double total_error_f = 0;
    //int solved_count = 0;

    for (int i = 1; i <= 100; i++)
    {
        cout << "\n========================================" << endl;
        cout << "Обработка функции" << i << endl;
        cout << "========================================" << endl;
        GlobalSearch search(i, epsilon, r_param, tightness);
        search.algorithm();
        search.printResults();

        vector<double> point;
        double value;
        size_t points_count;
        vector<double> true_point;
        double true_value;

        search.getResults(point, value, points_count, true_point, true_value);

        // Вычисляем погрешности
        double error_x = abs(point[0] - true_point[0]);
        double error_y = abs(point[1] - true_point[1]);
        double error_f = abs(value - true_value);

        //if (error_f < 1e-6) solved_count++;

        cout << "\nПогрешность по x: " << error_x << endl;
        cout << "Погрешность по y: " << error_y << endl;
        cout << "Погрешность по f: " << error_f << endl;

        // Записываем в файл
        file << i << ";"
            << point[0] << ";" << point[1] << ";" << value << ";"
            << points_count << ";"
            << true_point[0] << ";" << true_point[1] << ";" << true_value << ";"
            << error_x << ";" << error_y << ";" << error_f << endl;
    }

    file.close();

    return 0;
}
*/



#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "AGPmnog.h"

using namespace std;

struct TestParams {
    ProblemType type;    // тип задачи
    int dim;
    int func_count;      // кол-во ф-ций
    double epsilon;
    double r;
    int tightness;
    string name;
};

void runTest(const TestParams& params, const string& filename) {
    cout << "\n========================================" << endl;
    cout << "Запуск: " << params.name << endl;
    cout << "Параметры: tightness=" << params.tightness
        << ", epsilon=" << params.epsilon
        << ", r=" << params.r << endl;
    cout << "========================================\n" << endl;

    ofstream file(filename);

    if (params.type == PT_Grishagin) {
        file << "func_num;type;iterations;x1_found;x2_found;f_found;"
            << "x1_true;x2_true;f_true;error_f" << endl;
    }
    else if (params.dim == 2) {
        file << "func_num;dim;class;iterations;"
            << "x1_found;x2_found;f_found;"
            << "x1_true;x2_true;f_true;"
            << "error_f" << endl;
    }
    else if (params.dim == 3) {
        file << "func_num;dim;class;iterations;"
            << "x1_found;x2_found;x3_found;f_found;"
            << "x1_true;x2_true;x3_true;f_true;"
            << "error_f" << endl;
    }

    double total_iterations = 0;
    double total_error_f = 0;
    vector<double> errors;

    // Счётчик решённых функций (погрешность меньше epsilon)
    int solved_count = 0;

    int max_func = params.func_count;

    for (int func_num = 1; func_num <= max_func; func_num++) {
        GlobalSearch search(params.type, func_num, params.dim, params.epsilon, params.r, params.tightness);
        search.algorithm();

        vector<double> point;
        double value;
        size_t points_count;
        vector<double> true_point;
        double true_value;

        search.getResults(point, value, points_count, true_point, true_value);

        double error_f = abs(value - true_value);
        total_iterations += points_count;
        total_error_f += error_f;
        errors.push_back(error_f);

        // Проверяем, решена ли функция (погрешность меньше epsilon)
        if (error_f < params.epsilon) {
            solved_count++;
        }

        // Запись в файл
        if (params.type == PT_Grishagin) {
            file << func_num << ";Grishagin;" << points_count << ";"
                << point[0] << ";" << point[1] << ";" << value << ";"
                << true_point[0] << ";" << true_point[1] << ";" << true_value << ";"
                << error_f << endl;
        }
        else if (params.dim == 2) {
            string className = (params.type == PT_GKLS_Simple) ? "simple" : "hard";
            file << func_num << ";" << params.dim << ";" << className << ";" << points_count << ";"
                << point[0] << ";" << point[1] << ";" << value << ";"
                << true_point[0] << ";" << true_point[1] << ";" << true_value << ";"
                << error_f << endl;
        }
        else if (params.dim == 3) {
            string className = (params.type == PT_GKLS_Simple) ? "simple" : "hard";
            file << func_num << ";" << params.dim << ";" << className << ";" << points_count << ";"
                << point[0] << ";" << point[1] << ";" << point[2] << ";" << value << ";"
                << true_point[0] << ";" << true_point[1] << ";" << true_point[2] << ";" << true_value << ";"
                << error_f << endl;
        }

        if (func_num % 10 == 0) {
            cout << "  Выполнено " << func_num << "/" << max_func << " функций" << endl;
        }
    }

    file.close();

    if (!errors.empty()) {
        sort(errors.begin(), errors.end());
        cout << "\n========== ИТОГОВЫЕ РЕЗУЛЬТАТЫ ДЛЯ " << params.name << " ==========\n";
        //cout << "Решено функций (error < eps): " << solved_count << " из " << max_func << " (" << fixed << setprecision(2) << (100.0 * solved_count / max_func) << "%)" << endl;
        cout << "Среднее количество итераций: " << fixed << setprecision(2) << total_iterations / max_func << "\n";
        cout << "Средняя погрешность: " << scientific << setprecision(6) << total_error_f / max_func << "\n";
        cout << "Результаты сохранены в файл: " << filename << endl;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    //int tightness = 10;
    vector<TestParams> tests = {
        {PT_Grishagin, 2, 100, 0.001, 2.7, 10, "Grishagin"},
        {PT_GKLS_Simple, 2, 100, 0.001, 4.5, 10, "2D_Simple_GKLS"},
        {PT_GKLS_Hard, 2, 100, 0.001, 5.0, 8, "2D_Hard_GKLS"},
        {PT_GKLS_Simple, 3, 100, 0.01, 4.0, 8, "3D_Simple_GKLS"},
        {PT_GKLS_Hard, 3, 100, 0.01, 4.0, 8, "3D_Hard_GKLS"}
    };

    for (const auto& test : tests) {
        string filename = "results_" + test.name + ".csv";
        runTest(test, filename);
    }

    return 0;
}


/*
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "AGPmnog.h"

using namespace std;

struct TestParams {
    ProblemType type;
    int dim;
    int func_count;
    double epsilon;
    double r;
    int tightness;
    string name;
};

void runTest(const TestParams& params, const string& filename) {
    cout << "\n========================================" << endl;
    cout << "Запуск: " << params.name << endl;
    cout << "Параметры: tightness=" << params.tightness
        << ", epsilon=" << params.epsilon
        << ", r=" << params.r << endl;
    cout << "========================================\n" << endl;

    ofstream file(filename);
    file << "func_num;dim;class;iterations;"
        << "x1_found;x2_found;f_found;"
        << "x1_true;x2_true;f_true;error_f" << endl;

    double total_iterations = 0;
    double total_error_f = 0;
    vector<double> errors;
    int solved_count = 0;
    int max_func = params.func_count;

    // Для отслеживания нерешённых функций
    vector<int> failed_functions;

    for (int func_num = 1; func_num <= max_func; func_num++) {
        GlobalSearch search(params.type, func_num, params.dim,
            params.epsilon, params.r, params.tightness);
        search.algorithm();

        vector<double> point;
        double value;
        size_t points_count;
        vector<double> true_point;
        double true_value;

        search.getResults(point, value, points_count, true_point, true_value);

        double error_f = abs(value - true_value);
        total_iterations += points_count;
        total_error_f += error_f;
        errors.push_back(error_f);

        if (error_f < params.epsilon) {
            solved_count++;
        }
        else {
            failed_functions.push_back(func_num);
        }

        string className = "hard";

        file << func_num << ";" << params.dim << ";" << className << ";" << points_count << ";"
            << point[0] << ";" << point[1] << ";" << value << ";"
            << true_point[0] << ";" << true_point[1] << ";" << true_value << ";"
            << error_f << endl;

        if (func_num % 10 == 0) {
            cout << "  Выполнено " << func_num << "/" << max_func << " функций" << endl;
        }
    }

    file.close();

    if (!errors.empty()) {
        sort(errors.begin(), errors.end());
        cout << "\n========== ИТОГОВЫЕ РЕЗУЛЬТАТЫ ДЛЯ " << params.name << " ==========\n";
        cout << "Решено функций (error < eps): " << solved_count << " из " << max_func
            << " (" << fixed << setprecision(2) << (100.0 * solved_count / max_func) << "%)" << endl;
        cout << "Среднее количество итераций: " << fixed << setprecision(2) << total_iterations / max_func << "\n";
        cout << "Средняя погрешность: " << scientific << setprecision(6) << total_error_f / max_func << "\n";
        cout << "Медианная погрешность: " << scientific << setprecision(6) << errors[errors.size() / 2] << "\n";

        if (!failed_functions.empty()) {
            cout << "Не решённые функции (первые 10): ";
            for (size_t i = 0; i < min(failed_functions.size(), (size_t)10); i++) {
                cout << failed_functions[i] << " ";
            }
            cout << endl;
        }
        cout << "Результаты сохранены в файл: " << filename << endl;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    // ============================================
    // ПАРАМЕТРЫ ДЛЯ 2D HARD GKLS
    // Меняйте здесь значения для экспериментов
    // ============================================

    int tightness = 8;      // попробуйте 8, 10, 12
    double epsilon = 0.01;  // попробуйте 0.01, 0.005, 0.001     для 2 hard 0,01
    double r = 4.3;          // попробуйте 7.0, 8.0, 9.0, 10.0    для 2 hard 6.0
    int func_count = 100;    // сколько функций тестировать

    // Для быстрого теста (одна функция) - раскомментируйте:
    // func_count = 1;

    vector<TestParams> tests = {
        //{PT_GKLS_Hard, 2, func_count, epsilon, r, tightness, "2D_Hard_GKLS"}
        //{PT_GKLS_Simple, 3, func_count, epsilon, r, tightness, "3D_Simple_GKLS"}
        {PT_GKLS_Hard, 3, func_count, epsilon, r, tightness, "3D_Hard_GKLS"}
    };

    for (const auto& test : tests) {
        string filename = "results_" + test.name + ".csv";
        runTest(test, filename);
    }

    cout << "\n========================================" << endl;
    cout << "ТЕСТ ЗАВЕРШЕН!" << endl;
    cout << "========================================" << endl;

    return 0;
}*/