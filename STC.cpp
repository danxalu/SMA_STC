#include <iostream>
#include <random>
#include <cmath>
#include <chrono>


template <typename num_type>
void print_array(const num_type* array, const size_t& length) //функция вывода массива в строку
{
    for (size_t i = 0; i < length; i++) {
        std::cout << array[i] << " ";
    }
    std::cout << "\n";
}


template <typename num_type>
num_type avarage_abs(const num_type* array, const size_t& length) { //функция вычисления среднего арифметического модулей
    num_type norm = 0;
    for (size_t i = 0; i < length; i++) {
        norm += abs(array[i]);
    }
    return norm / length;
}


template <typename num_type>
void two_sums(num_type& sum, num_type& err, const num_type& new_elem) { // Вспомогательная функция для алгоритма Rump–Ogita–Oishi
    sum += new_elem;
    num_type s = sum + new_elem;
    num_type bs = s - new_elem;
    num_type as = s - bs;
    err += (new_elem - bs) + (sum - as); // все "хвостики" складываем в отдельную переменную
}


template <typename num_type>
void sum_rump(num_type& sum_cur, const num_type& new_elem) { // Алгоритм Rump–Ogita–Oishi для первых отсчетов
    num_type err = 0.0;
    two_sums(sum_cur, err, new_elem);
    sum_cur += err; // добавляем "хвостик"
}


template <typename num_type>
void sum_rump(num_type& sum_cur, const num_type& sum_prev, const num_type& new_elem, const num_type& old_elem, const int& len_window) { // Алгоритм Rump–Ogita–Oishi
    num_type err = 0.0;
    sum_cur = sum_prev;
    num_type sum_elems = -old_elem;
    two_sums<num_type>(sum_elems, err, new_elem);
    two_sums<num_type>(sum_cur, err, (num_type)(sum_elems / len_window));
    sum_cur += err; // добавляем "хвостик"
}


template <typename num_type>
void new_element(num_type func(num_type i), num_type& signal, num_type& noise, num_type& reading, const int& i) {
    signal = func((num_type)i/16); // получаем координату "чистого сигнала"
    bool sign = rand() % 2; // генерируем знак координаты шума
    noise = pow(-1, sign) * (rand() % 1024) / 4096; //генерируем координату шума
    reading = signal + noise; //получаем отсчеты
}


template <typename num_type>
num_type sin_cos(num_type x) {
    return sin(cos(x));
}


template <typename num_type>
num_type SMA(num_type func(num_type i), int len_window, int len_readings) { // неподсредственно функция простого скользящего среднего
    num_type* readings; // отсчеты
    num_type* signal; // "чистый сигнал"
    num_type* noise; // шум
    num_type* sma; // результат простого среднего скользящего
    num_type* errors; // ошибки

    num_type sum_cur_buffer; // буфер-сумма для первых len_window шт. отсчетов

    readings = new num_type[len_readings];
    signal = new num_type[len_readings];
    noise = new num_type[len_readings];
    sma = new num_type[len_readings];
    errors = new num_type[len_readings];

    sum_cur_buffer = 0.0;
    for (int i = 0; i < len_window; i++) { // вычисление первых len_window шт. отсчетов
        new_element<num_type>(func, signal[i], noise[i], readings[i], i); // получаем "в реальном времени" отсчет
        sum_rump<num_type>(sum_cur_buffer, readings[i]); // заносим его в буфер-сумму
        sma[i] = sum_cur_buffer / (i + (num_type)1); // считаем значение ПCС в данной кооординате
        errors[i] = signal[i] - sma[i]; // считаем ошибку 
    }

    for (int i = len_window; i < len_readings; i++) { // вычисление остальных len_window шт. отсчетов
        new_element<num_type>(func, signal[i], noise[i], readings[i], i);
        sum_rump<num_type>(sma[i], sma[i - 1], readings[i], readings[i - len_window], len_window); // считаем значение ПCС "рекурсивно" (через значение прошлой координаты)
        errors[i] = signal[i] - sma[i];
    }

    num_type error = avarage_abs<num_type>(errors, len_readings); // считаем общую ошибку через среднее арифметическое модулей

    delete[] errors;
    delete[] sma;
    delete[] noise;
    delete[] signal;
    delete[] readings;

    return error;
}



template <typename num_type>
num_type run_SMA(num_type func(num_type i), int num_for_rand) { // функция, генерирующая псевдослучайные входные данные для ПСС
    srand((int)time(NULL) * num_for_rand); // задаем сид для генерирования псевдослучайных чисел
    int len_window = pow(2, (rand() % 5)); // длина окна
    int len_readings = pow(2, (rand() % 20)) * len_window; // количество отсчетов
    return SMA(func, len_window, len_readings);
}



template <typename num_type>
void run_tests() {
    //посчитаем ошибки ПСС для функций sin(x) и sin(cos(x))
    num_type errs_sin[10];
    num_type errs_sin_cos[10];
    for (size_t i = 0; i < 10; i++) {
        errs_sin[i] = run_SMA<num_type>(sin, i);
        errs_sin_cos[i] = run_SMA<num_type>(sin_cos, i);
    }

    //средние ошибки
    num_type err_sin = avarage_abs<num_type>(errs_sin, 10);
    num_type err_sin_cos = avarage_abs<num_type>(errs_sin_cos, 10);

    printf("%12s    %12s          error (10 tests each with random length of window and array of readings)\n", "func", "avg_err");
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
    
    run_tests<float>();

    for (int len_window = 4; len_window < 256; len_window *= 2) {
        auto begin = std::chrono::steady_clock::now();
        SMA<float>(sin, len_window, 1e6);
        auto end = std::chrono::steady_clock::now();

        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        std::cout << len_window << ", float, " << 1e9 / elapsed_ms.count() << "\n";

        begin = std::chrono::steady_clock::now();
        SMA<double>(sin, len_window, 1e6);
        end = std::chrono::steady_clock::now();

        elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        std::cout << len_window << ", double, " << 1e9 / elapsed_ms.count() << "\n";
    }
        

}