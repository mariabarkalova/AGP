#include<iostream>
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
    /*for (int function_number = 0;function_number < 999;function_number++)
    {
        AGP method(epsilon, 0, 0, function_number);
        method.algorithm();
        method.getResults(min_x, min_value, point_count, true_x, true_f);
        double error_x = abs(min_x - true_x);
        double error_f = abs(min_value - true_f);
        file << function_number << ";" << min_x << ";" << min_value << ";" << point_count << ";" << true_x << ";" << true_f << ";" << error_x << ";" << error_f << endl;
    }*/
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
}