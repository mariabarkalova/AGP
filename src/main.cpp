#include<iostream>
#include<fstream>
#include<string>
#include"Piyavsky_method.h"
using namespace std;

void processAllFunctions(double a, double b, double epsilon, ofstream& file)
{
    file << "Номер ф-ции 1; Точка минимума 1; Знач ф-ции 1; Кол-во точек 1; Ист.знач.минимума 1; Ист.знач.ф-ции 1; Погрешность по x 1;Погрешность по f 1;"
        << "Номер ф-ции 2; Точка минимума 2; Знач ф-ции 2; Кол-во точек 2; Ист.знач.минимума 2; Ист.знач.ф-ции 2; Погрешность по x 2;Погрешность по f 2"
        << endl;
    double min_x1, min_value1;
    size_t point_count1;
    double true_x1, true_f1;
    double min_x2, min_value2;
    size_t point_count2;
    double true_x2, true_f2;

    for (int function_number = 0; function_number < 999; function_number++)
    {
        // ф-я 1 (Shekel)
        MethodPiyavsky method1(epsilon, 0, 0, function_number, 0);
        method1.algorithm();
        method1.getResults(min_x1, min_value1, point_count1, true_x1, true_f1);
        double error_x1 = abs(min_x1 - true_x1);
        double error_f1 = abs(min_value1 - true_f1);
        file << function_number << ";" << min_x1 << ";" << min_value1 << ";" << point_count1 << ";" << true_x1 << ";" << true_f1 << ";" << error_x1 << ";" << error_f1 << ";";

        // ф-я 2 (Hill)
        MethodPiyavsky method2(epsilon, 0, 0, function_number, 1);
        method2.algorithm();
        method2.getResults(min_x2, min_value2, point_count2, true_x2, true_f2);
        double error_x2 = abs(min_x2 - true_x2);
        double error_f2 = abs(min_value2 - true_f2);
        file << function_number << ";" << min_x2 << ";" << min_value2 << ";" << point_count2 << ";" << true_x2 << ";" << true_f2 << ";" << error_x2 << ";" << error_f2 << endl;
    }
}


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
    /*file << "Номер ф-ции; Точка минимума; Знач ф-ции; Кол-во точек; Ист.знач.минимума; Ист.знач.ф-ции; Погрешность по x;Погрешность по f " << endl;
    for(flag=0;flag<2;flag++)
    {
        for (function_number = 0;function_number < 999;function_number++)
        {
            MethodPiyavsky method(epsilon, a, b, function_number, flag);
            method.algorithm();
            method.getResults(min_x, min_value, point_count, true_x, true_f);
            double error_x = abs(min_x - true_x);
            double error_f = abs(min_value - true_f);
            file << function_number << ";" << min_x << ";" << min_value << ";" << point_count << ";" << true_x << ";" << true_f << ";" << error_x << ";" << error_f << endl;
        }
    }*/

    processAllFunctions(0, 0, epsilon, file);

    file.close();
    cout << "Файл успешно создан и заполнен";
    return 0;
}