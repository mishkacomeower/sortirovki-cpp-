#include <iostream>
#include <vector>
#include <algorithm>
#include <clocale>
#include <cstdlib>
#include <windows.h>
#include <thread>
#include <chrono>
#include <limits>
#include <fstream>
#include <string>
using namespace std;
using namespace chrono;

// счётчики, обнулюятся перед каждой соритровкой
long long comparisons = 0;
long long swaps = 0;

//функция сохранения массива в файл
void saveArrayToFile(const vector<double>& arr, const string& filename) {
    ofstream file(filename);

    for (size_t i = 0; i < arr.size(); i++) {
        file << arr[i];
        if (i < arr.size() - 1) file << " ";
    }

    file.close();
    cout << "Массив сохранен в \033[32m" << filename << "\033[0m";
}

// функция нахождения опорного элемента ( в данном случае медианы трех )
double findPivot(const vector<double>& arr) {
    double low = arr[0];
    double mid = arr[arr.size() / 2];
    double high = arr[arr.size() - 1];

    if ((low <= mid && mid <= high) || (high <= mid && mid <= low))
        return mid;
    if ((mid <= low && low <= high) || (high <= low && low <= mid))
        return low;
    return high;
}

// функции сортировки
vector<double> QuickSort(vector<double> arr) {
    if (arr.size() <= 1) return arr;

    double pivot = findPivot(arr);
    vector<double> left;
    vector<double> middle;
    vector<double> right;

    for (double i : arr) { // O(n) - проходим по всему массиву
        comparisons++;
        if (i < pivot) left.push_back(i);
        else if (i == pivot) middle.push_back(i);
        else right.push_back(i);
    }

    left = QuickSort(left);
    right = QuickSort(right);

    // Сборка результата - O(n)
    vector<double> new_vector;
    new_vector.reserve(left.size() + middle.size() + right.size());
    new_vector.insert(new_vector.end(), left.begin(), left.end());
    new_vector.insert(new_vector.end(), middle.begin(), middle.end());
    new_vector.insert(new_vector.end(), right.begin(), right.end());

    return new_vector;
    // Итог: На каждом уровне рекурсии делаем О(n) работы, уровней всего log n (потому что делим массив пополам) Значит QuickSort работает за O(n log n)
}

vector<double> InsertionSort(vector<double> arr) {
    for (int next = 1; next < arr.size(); next++) {  // O(n) - внешний цикл
        double current = arr[next];
        int previous = next - 1;

        while (previous >= 0) { // В ХУДШЕМ: O(n) для каждого next
            comparisons++;                         
            if (arr[previous] > current) {         
                arr[previous + 1] = arr[previous]; 
                swaps++;   
                previous--;
            }
            else break;
        }

        if (previous + 1 != next) {
            arr[previous + 1] = current;
            swaps++; 
        }
    }
    return arr;
    // Итог: O(n) * O(n) = O(n²) в худшем
}

vector<double> CountingSort(vector<double>& arr) {
    vector<int> int_arr(arr.begin(), arr.end());

    // Поиск min/max - O(n)
    auto min_it = min_element(int_arr.begin(), int_arr.end());
    int min_val = *min_it;                                    
    auto max_it = max_element(int_arr.begin(), int_arr.end());
    int max_val = *max_it;                                    
    int range = max_val - min_val + 1;                        

    vector<int> count(range, 0);  // O(range)

    for (int number : int_arr) {  // O(n)
        count[number - min_val]++;
    }

    vector<int> new_int_vector(int_arr.size());
    int current_index = 0;

    for (int i = 0; i < range; i++) { // O(range)
        while (count[i] > 0) { // O(n)
            new_int_vector[current_index++] = i + min_val;
            count[i]--;                                   
        }
    }

    vector<double> new_vector(new_int_vector.begin(), new_int_vector.end());
    return new_vector;
    // Итог: O(n) + O(range) + O(n) + O(range) + O(n) = O(n + range)
}

void CountingSortForRadix(vector<int>& arr, int exp) {
    vector<int> count(10, 0);
    vector<int> output(arr.size());

    for (int number : arr) {  // O(n)
        int digit = (number / exp) % 10;
        count[digit]++;
    }

    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    for (int i = arr.size() - 1; i >= 0; i--) { // O(n)
        int digit = (arr[i] / exp) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }

    arr = output;
    // промежуточный итог O(n) + O(n) = O(n), промежуточный потому что буду учитывать этот результат в RadixSort
}

vector<double> RadixSort(vector<double>& arr) {
    vector<int> int_arr(arr.begin(), arr.end());

    auto max_it = max_element(int_arr.begin(), int_arr.end());
    int max_val = *max_it;

    for (int exp = 1; max_val / exp > 0; exp *= 10) {  // O(d) где d - число разрядов
        CountingSortForRadix(int_arr, exp); // Вызов другой сортировки (в ней промежуточный итог O(n)), тоесть каждый вызов это O(n)
    }

    vector<double> new_vector(int_arr.begin(), int_arr.end());
    return new_vector;
    // Итог: O(n) + O(d * n) = O(d * n)
}

vector<double> CocktailShaker(vector<double> arr) {
    int left = 0;
    int right = arr.size() - 1;

    while (left < right) { // В ХУДШЕМ: O(n) проходов
        int new_right = left;

        // Проход слева направо
        for (int i = left; i < right; i++) {  // O(n) для каждого прохода
            comparisons++;
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                swaps++;
                new_right = i;
            }
        }
        right = new_right;

        if (left >= right) break;

        int new_left = right;

        // Проход справа налево
        for (int i = right; i > left; i--) {  // O(n) для каждого прохода
            comparisons++;
            if (arr[i - 1] > arr[i]) {
                swap(arr[i - 1], arr[i]);
                swaps++;
                new_left = i;
            }
        }
        left = new_left;
    }

    return arr;
    // Итог: O(n) * O(n) = O(n²)
}

//Функция вывода массива
void PrintVector(const vector<double>& vector, const string& sortName, double timeMs) {

    if (vector.size() <= 20) {
        cout << "\033[32mРезультат " << sortName << ": \033[0m{";

        for (double i : vector) {
            if (i != vector[vector.size() - 1]) {
                cout << i << ", ";
            }
            else cout << i << "}";
        }
    }

    else {
        cout << "\033[32mРезультат " << sortName << ": \033[0m";
        saveArrayToFile(vector, "result.txt");
    }

    cout << " | \033[33mВремя: " << timeMs << " мс\033[0m";

    if (sortName != "RadixSort" && sortName != "CountingSort") {
        cout << " | \033[36mСравнений: " << comparisons << "\033[0m";
        cout << " | \033[35mОбменов: " << swaps << "\033[0m";
    }

    //сброс счетчиков
    comparisons = 0;
    swaps = 0;

    cout << endl;
}

int main() {
    setlocale(LC_ALL, "Russian");

    vector<double> test;
    int option_1 = 0;
    bool exit_while = false;

    while (!exit_while) {
        cout << "\033[35mВыберите опцию:\033[0m" << endl
            << "1 - Готовый массив" << endl
            << "2 - Генерация массива" << endl
            << "3 - Импорт массива" << endl
            << "4 - Ввод массива вручную" << endl;
        cin >> option_1;

        switch (option_1) {
        case 1:
            test = { 1245, 67, 892, 1432, 345, 978, 13, 701, 1156, 434,
                    888, 156, 999, 512, 1487, 234, 876, 0, 654, 321 };

            cout << "\033[32mВыбран тестовый массив: \033[0m";
            for (double i : test) cout << i << " ";
            cout << endl;
            exit_while = true;
            break;

        case 2: {
            srand(time(0));
            int min = 0, max = 0, count = 0; // поддерживает числа в диапазоне [-2 147 483 647; 2 147 483 648], можно конечно юзать дабл например, а не инт, но я решил что оно не надо тут

            cout << "\033[35mВведите минимальное число: \033[0m";
            cin >> min;
            cout << "\033[35mВведите максимальное число: \033[0m";
            cin >> max;
            cout << "\033[35mВведите количество чисел в массиве: \033[0m";
            cin >> count;

            // сам генератор
            for (int i = 0; i < count; i++) {
                int random_num = min + rand() % (max - min + 1);
                test.push_back(random_num);
            }

            if (test.size() <= 1) {
                cout << "\033[33mПо хуйне то не заебывайте\033[0m" << endl << endl;
                break;
            }

            else if (test.size() <= 20) {
                cout << "Сгенерированный массив: {";
                for (double num : test) {
                    cout << num << ", ";
                }
                cout << "}";
            }

            else {
                saveArrayToFile(test, "generated.txt");
            }

            exit_while = true;
            break;
        }

        case 3: {
            test.clear();
            int option_2 = 0;
            bool exit_1 = false;

            cout << "\033[35mВыберите какой массив хотите отсортировать:\033[0m" << endl
                << "1 - Целочисленный массив без отрицательных чисел" << endl
                << "2 - Массив с флотовыми и отрицательными числами" << endl
                << "3 - Целочисленный массив с 5% неотсортированных данных" << endl;

            while (!exit_1) {
                cin >> option_2;
                switch (option_2) {
                case 1: {
                    ifstream file("test1.txt");
                    string number;
                    while (file >> number) {
                        test.push_back(stod(number));
                    }
                    exit_1 = true;
                    exit_while = true;
                    break;
                }
                case 2: {
                    ifstream file("test2.txt");
                    string number;
                    while (file >> number) {
                        test.push_back(stod(number));
                    }
                    exit_1 = true;
                    exit_while = true;
                    break;
                }
                case 3: {
                    ifstream file("test3.txt");
                    string number;
                    while (file >> number) {
                        test.push_back(stod(number));
                    }
                    exit_1 = true;
                    exit_while = true;
                    break;
                }
                default:
                    cout << "\033[33mНеверный ввод, введите еще раз\033[0m" << endl;
                    break;
                }
            }
            break;
        }

        case 4: {
            cout << "\033[35mВведите числа через пробел (для завершения введите любую букву): \033[0m" << endl;

            test.clear();
            double num;
            while (cin >> num) {
                test.push_back(num);
            }

            cin.clear();
            cin.ignore(1000, '\n');

            if (test.size() <= 1) {
                cout << "\033[33mПо хуйне то не заебывайте\033[0m" << endl << endl;
                break;
            }
            else {
                cout << "\033[32mВведённый массив: \033[0m";
                for (double i : test) cout << i << " ";
                cout << endl;
                exit_while = true;
            }
            break;
        }

        default:
            cout << "\033[31mНеверный ввод, введите еще раз\033[0m" << endl;
        }
    }

    cout << endl;
    cout << "\033[35mВыберите алгоритм сортировки:\033[0m" << endl
        << "1 - Quick Sort" << endl
        << "2 - Insertion Sort" << endl
        << "3 - Radix Sort" << endl
        << "4 - Cocktail Shaker Sort" << endl
        << "5 - Counting Sort" << endl
        << "6 - Все сортировки разом" << endl
        << "7 - Выход" << endl;

    int option;
    while (true) {
        cin >> option;

        switch (option) {
        case 1: {
            auto start = high_resolution_clock::now();
            vector<double> result = QuickSort(test);
            auto end = high_resolution_clock::now();
            double timeMs = duration<double, milli>(end - start).count();
            PrintVector(result, "QuickSort", timeMs);
            cout << endl << "\033[35mВыберите следующее действие\033[0m" << endl;
            break;
        }

        case 2: {
            auto start = high_resolution_clock::now();
            vector<double> result = InsertionSort(test);
            auto end = high_resolution_clock::now();
            double timeMs = duration<double, milli>(end - start).count();
            PrintVector(result, "InsertionSort", timeMs);
            cout << endl << "\033[35mВыберите следующее действие\033[0m" << endl;
            break;
        }

        case 3: {
            bool valid = true;
            for (double num : test) {
                if (num < 0 || num != (int)num) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                auto start = high_resolution_clock::now();
                vector<double> result = RadixSort(test);
                auto end = high_resolution_clock::now();
                double timeMs = duration<double, milli>(end - start).count();
                PrintVector(result, "RadixSort", timeMs);
            }
            else {
                cout << "\033[33mRadixSort поддерживает только целые числа >= 0\033[0m" << endl;
            }
            cout << endl << "\033[35mВыберите следующее действие\033[0m" << endl;
            break;
        }

        case 4: {
            auto start = high_resolution_clock::now();
            vector<double> result = CocktailShaker(test);
            auto end = high_resolution_clock::now();
            double timeMs = duration<double, milli>(end - start).count();
            PrintVector(result, "CocktailShaker", timeMs);
            cout << endl << "\033[35mВыберите следующее действие\033[0m" << endl;
            break;
        }

        case 5: {
            bool valid = true;
            for (double num : test) {
                if (num != (int)num) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                auto start = high_resolution_clock::now();
                vector<double> result = CountingSort(test);
                auto end = high_resolution_clock::now();
                double timeMs = duration<double, milli>(end - start).count();
                PrintVector(result, "CountingSort", timeMs);
            }
            else {
                cout << "\033[33mCountingSort поддерживает только целые числа\033[0m" << endl;
            }
            cout << endl << "\033[35mВыберите следующее действие\033[0m" << endl;
            break;
        }

        case 6: {
            cout << endl;

            bool valid = true;
            for (double num : test) {
                if (num < 0 || num != (int)num) {
                    valid = false;
                    break;
                }
            }

            auto start1 = high_resolution_clock::now();
            vector<double> result1 = QuickSort(test);
            auto end1 = high_resolution_clock::now();
            double timeMs1 = duration<double, milli>(end1 - start1).count();
            PrintVector(result1, "QuickSort", timeMs1);

            auto start2 = high_resolution_clock::now();
            vector<double> result2 = InsertionSort(test);
            auto end2 = high_resolution_clock::now();
            double timeMs2 = duration<double, milli>(end2 - start2).count();
            PrintVector(result2, "InsertionSort", timeMs2);

            auto start3 = high_resolution_clock::now();
            vector<double> result3 = CocktailShaker(test);
            auto end3 = high_resolution_clock::now();
            double timeMs3 = duration<double, milli>(end3 - start3).count();
            PrintVector(result3, "CocktailShaker", timeMs3);

            if (valid) {
                auto start4 = high_resolution_clock::now();
                vector<double> result4 = RadixSort(test);
                auto end4 = high_resolution_clock::now();
                double timeMs4 = duration<double, milli>(end4 - start4).count();
                PrintVector(result4, "RadixSort", timeMs4);

                auto start5 = high_resolution_clock::now();
                vector<double> result5 = CountingSort(test);
                auto end5 = high_resolution_clock::now();
                double timeMs5 = duration<double, milli>(end5 - start5).count();
                PrintVector(result5, "CountingSort", timeMs5);
            }
            else {
                cout << "\033[33mRadixSort и CountingSort пропущены (поддерживают только целые числа >= 0)\033[0m" << endl;
            }

            auto start5 = high_resolution_clock::now();
            vector<double> result5 = test;
            sort(test.begin(), test.end());
            auto end5 = high_resolution_clock::now();
            double timeMs6 = duration<double, milli>(end5 - start5).count();
            cout << "\033[32mЭталон сортировок (sort из algorithm) завершил работу за: " << timeMs6 << " мс \033[0m";

            cout << endl << "\033[35mВыберите следующее действие\033[0m" << endl;
            break;
        }

        case 7:
            cout << "\033[31mАлгоритмы сортировок - ВСЁ." << endl;
            cout << "Гольчевский жёстко про алгоритмы сортировок...\033[0m" << endl;
            exit(0);

        default:
            cout << "\033[31mНеверный ввод, введите снова\033[0m" << endl;
            break;
        }
    }
    return 0;
}