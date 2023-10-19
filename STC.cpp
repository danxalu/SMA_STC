#include <iostream>
#include <random>
#include <cmath>


void print_array(const double* array, const size_t& length) //функция вывода массива в строку
{
    for (size_t i = 0; i < length; i++) {
        std::cout << array[i] << " ";
    }
    std::cout << "\n";
}


double avarage_abs(const double* array, const size_t& length) { //функция вычисления среднего арифметического модулей
    double norm = 0;
    for (size_t i = 0; i < length; i++) {
        norm += abs(array[i]);
    }
    return norm / length;
}


void two_sums(double& sum, double& err, const double& new_elem) { // Вспомогательная функция для алгоритма Rump–Ogita–Oishi
    sum += new_elem;
    double s = sum + new_elem;
    double bs = s - new_elem;
    double as = s - bs;
    err += (new_elem - bs) + (sum - as); // все "хвостики" складываем в отдельную переменную
}


void sum_rump(double& sum_cur, const double& new_elem) { // Алгоритм Rump–Ogita–Oishi для первых отсчетов
    double err = 0.0;
    two_sums(sum_cur, err, new_elem);
    sum_cur += err; // добавляем "хвостик"
}


void sum_rump(double& sum_cur, const double& sum_prev, const double& new_elem, const double& old_elem, const int& len_window) { // Алгоритм Rump–Ogita–Oishi
    double err = 0.0;
    sum_cur = sum_prev;
    double sum_elems = -old_elem;
    two_sums(sum_elems, err, new_elem);
    two_sums(sum_cur, err, (double)(sum_elems / len_window));
    sum_cur += err; // добавляем "хвостик"
}


void new_element(double func(double i), double& signal, double& noise, double& reading, const int& i) {
    signal = func((double)i/16); // получаем координату "чистого сигнала"
    bool sign = rand() % 2; // генерируем знак координаты шума
    noise = pow(-1, sign) * (rand() % 1024) / 4096; //генерируем координату шума
    reading = signal + noise; //получаем отсчеты
}


double sin_cos(double x) {
    return sin(cos(x));
}


double test(double func(double i), int num_for_rand) {
    int len_window; // длина окна
    int len_readings; // количество отсчетов
    double* readings; // отсчеты
    double* signal; // "чистый сигнал"
    double* noise; // шум
    double* SMA; // результат простого среднего скользящего
    double* errors; // ошибки

    double sum_cur_buffer; // буфер-сумма для первых len_window шт. отсчетов


    srand((int)time(NULL)*num_for_rand); // задаем сид для генерирования псевдослучайных чисел
    len_window = pow(2, (rand() % 5));
    len_readings = pow(2, (rand() % 15)) * len_window;

    readings = new double[len_readings];
    signal = new double[len_readings];
    noise = new double[len_readings];
    SMA = new double[len_readings];
    errors = new double[len_readings];

    sum_cur_buffer = 0.0;
    for (int i = 0; i < len_window; i++) { // вычисление первых len_window шт. отсчетов
        new_element(func, signal[i], noise[i], readings[i], i); // получаем "в реальном времени" отсчет
        sum_rump(sum_cur_buffer, readings[i]); // заносим его в буфер-сумму
        SMA[i] = sum_cur_buffer / (i + (double)1); // считаем значение ПCС в данной кооординате
        errors[i] = signal[i] - SMA[i]; // считаем ошибку 
    }

    for (int i = len_window; i < len_readings; i++) { // вычисление остальных len_window шт. отсчетов
        new_element(func, signal[i], noise[i], readings[i], i);
        sum_rump(SMA[i], SMA[i - 1], readings[i], readings[i - len_window], len_window); // считаем значение ПCС "рекурсивно" (через значение прошлой координаты)
        errors[i] = signal[i] - SMA[i];
    }

    double error = avarage_abs(errors, len_readings); // считаем общую ошибку через среднее арифметическое модулей

    delete[] errors;
    delete[] SMA;
    delete[] noise;
    delete[] signal;
    delete[] readings;

    return error;
}


void run_tests() {
    //посчитаем ошибки ПСС для функций sin(x) и sin(cos(x))
    double errs_sin[10];
    double errs_sin_cos[10];
    for (size_t i = 0; i < 10; i++) {
        errs_sin[i] = test(sin, i);
        errs_sin_cos[i] = test(sin_cos, i);
    }

    //средние ошибки
    double err_sin = avarage_abs(errs_sin, 10);
    double err_sin_cos = avarage_abs(errs_sin_cos, 10);

    printf("%12s    %12s    %12s\n", "func", "avg_err", "errors");
    printf("%12s    %12.6f          ", "sin(x)", err_sin);
    print_array(errs_sin, 10);
    printf("%12s    %12.6f          ", "sin(cos(x))", err_sin_cos);
    print_array(errs_sin_cos, 10);
}

int main()
{
    /*
    print_array(signal, 200);
    std::cout << "\n";
    print_array(readings, 200);
    std::cout << "\n";
    print_array(SMA, 200);
    */
    run_tests();


}