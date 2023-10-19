#include <iostream>
#include <random>
#include <cmath>


void print_array(const double* array, int length)
{
    for (size_t i = 0; i < length; i++) {
        std::cout << array[i] << " ";
    }
    std::cout << "\n";
}


double two_sum(double& t, const double& a, const double& b) {
    double s = a + b;
    double bs = s - a;
    double as = s - bs;
    t = (b - bs) + (a - as);
    return s;
}


double sum_rump(const double* X, const int& n) { //Алгоритм Rump–Ogita–Oishi
    double s = 0.0, c = 0.0, e;
    for (int i = 0; i < n; i++) {
        s = two_sum(e, s, X[i]);
        c += e; //все "хвостики" складываем в отдельную переменную
    }
    return s + c;
}


void new_sum_rump(double& sum, double& err, const double& new_elem, const int& len_window) {
    sum += new_elem / len_window;
    double bs = sum - new_elem;
    double as = sum - bs;
    err += (new_elem - bs) + (sum - as);
}


void sum_rump(double& sum_cur, const double& new_elem) { //Алгоритм Rump–Ogita–Oishi для первых отсчетов
    double err = 0.0;
    new_sum_rump(sum_cur, err, new_elem, 1);
    sum_cur += err; //все "хвостики" складываем в отдельную переменную
}


void sum_rump(double& sum_cur, const double& sum_prev, const double& new_elem, const double& old_elem, const int& len_window) { //Алгоритм Rump–Ogita–Oishi
    double err = 0.0;
    sum_cur = sum_prev;
    new_sum_rump(sum_cur, err, new_elem, len_window);
    new_sum_rump(sum_cur, err, old_elem, len_window);
    sum_cur += err; //все "хвостики" складываем в отдельную переменную
}



void new_element(double func(double i), double& signal, double& noise, double& reading, const int& i) {
    signal = func((double)i/16);
    bool sign = rand() % 2;
    noise = pow(-1, sign) * (rand() % 1024) / 4096;
    reading = signal + noise;
}


int main()
{
    srand((int)time(NULL));
    int len_window = 32;//pow(2, (rand() % 5));
    int len_readings = 200; // pow(2, (rand() % 15))* len_window;
    double* readings = new double[len_readings]; //отсчеты
    double* signal = new double[len_readings];
    double* noise = new double[len_readings];
    //double* window = new double[len_window];
    double* SMA = new double[len_readings];

    double sum_cur_buffer = 0.0;
    for (int i = 0; i < len_window; i++) {
        new_element(sin, signal[i], noise[i], readings[i], i);
        sum_rump(sum_cur_buffer, readings[i]);
        SMA[i] = sum_cur_buffer / (i + (double)1);
        //SMA[i] = (sum_cur_buffer + readings[i]) / (i + (double)1);
    }
    
    for (int i = len_window; i < len_readings; i++) {
        new_element(sin, signal[i], noise[i], readings[i], i);
        sum_rump(SMA[i], SMA[i-1], readings[i], readings[i-len_window], len_window);
        //SMA[i] = SMA[i - 1] + (readings[i] - readings[i - len_window]) / len_window;
    }

    print_array(signal, 200);
    std::cout << "\n";
    //print_array(noise, 200);
    std::cout << "\n";
    print_array(readings, 200);
    std::cout << "\n";
    print_array(SMA, 200);

    delete[] noise;
    delete[] signal;
    delete[] readings;
}