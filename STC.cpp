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


void two_sums(double& sum, double& err, const double& new_elem) { // Вспомогательная функция для алгоритма Rump–Ogita–Oishi
    sum += new_elem;
    double bs = sum - new_elem;
    double as = sum - bs;
    err += (new_elem - bs) + (sum - as);
}


void sum_rump(double& sum_cur, const double& new_elem) { // Алгоритм Rump–Ogita–Oishi для первых отсчетов
    double err = 0.0;
    two_sums(sum_cur, err, new_elem);
    sum_cur += err; // все "хвостики" складываем в отдельную переменную
}


void sum_rump(double& sum_cur, const double& sum_prev, const double& new_elem, const double& old_elem, const int& len_window) { // Алгоритм Rump–Ogita–Oishi
    double err = 0.0;
    sum_cur = sum_prev;
    double sum_elems = -old_elem;
    two_sums(sum_elems, err, new_elem);
    two_sums(sum_cur, err, sum_elems / len_window);
    sum_cur += err; // все "хвостики" складываем в отдельную переменную
}


void new_element(double func(double i), double& signal, double& noise, double& reading, const int& i) {
    signal = func((double)i/16); // получаем координату "чистого сигнала"
    bool sign = rand() % 2; // генерируем знак координаты шума
    noise = pow(-1, sign) * (rand() % 1024) / 4096; //генерируем координату шума
    reading = signal + noise; //получаем отсчеты
}


int main()
{
    srand((int)time(NULL)); // задаем сид для генерирования псевдослучайных чисел
    int len_window = 32;// pow(2, (rand() % 5));
    int len_readings = 200; // pow(2, (rand() % 15))* len_window;
    double* readings = new double[len_readings]; // отсчеты
    double* signal = new double[len_readings]; // "чистый сигнал"
    double* noise = new double[len_readings]; // шум

    double* SMA = new double[len_readings]; // результат простого среднего скользящего

    double sum_cur_buffer = 0.0; // буфер-сумма для первых len_window шт. отсчетов
    for (int i = 0; i < len_window; i++) { // вычисление первых len_window шт. отсчетов
        new_element(sin, signal[i], noise[i], readings[i], i); // получаем "в реальном времени" отсчет
        sum_rump(sum_cur_buffer, readings[i]); // заносим его в буфер-сумму
        SMA[i] = sum_cur_buffer / (i + (double)1); // считаем значение ПМС в данной кооординате
    }
    
    for (int i = len_window; i < len_readings; i++) { // вычисление остальных len_window шт. отсчетов
        new_element(sin, signal[i], noise[i], readings[i], i);
        sum_rump(SMA[i], SMA[i-1], readings[i], readings[i-len_window], len_window); // считаем значение ПМС "рекурсивно" (через значение прошлой координаты)
    }

    print_array(signal, 200);
    std::cout << "\n";
    //print_array(noise, 200);
    std::cout << "\n";
    print_array(readings, 200);
    std::cout << "\n";
    print_array(SMA, 200);

    delete[] SMA;
    delete[] noise;
    delete[] signal;
    delete[] readings;
}