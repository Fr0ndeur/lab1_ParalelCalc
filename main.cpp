#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <iomanip>
#include <Windows.h>

using namespace std;
using namespace std::chrono;

void fillMatrix(vector<vector<int>>& matrix) { 
    random_device rd;
    mt19937 gen(rd()); // використовуємо mt19997 для псевдо-рандому
    uniform_int_distribution<> dis(0, 99); // використовуємо рівномірний розподіл від 0 до 99

    int n = matrix.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = dis(gen);
        }
    }
}

void printMatrix(const vector<vector<int>>& matrix) {
    int n = matrix.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << setw(4) << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

void sequentialProcessMatrix(vector<vector<int>>& matrix) {
    int n = matrix.size();
    vector<int> minValues(n);

    for (int i = 0; i < n; i++) { // спочатку розраховуємо мінімальні значення
        int col = n - 1 - i;
        int minVal = matrix[0][col];
        for (int j = 1; j < n; j++) {
            if (matrix[j][col] < minVal)
                minVal = matrix[j][col];
        }
        minValues[i] = minVal;
    }
    for (int i = 0; i < n; i++) { // а потім замінюємо у матриці
        int col = n - 1 - i;
        matrix[i][col] = minValues[i];
    }
}

void parallelProcessMatrix(vector<vector<int>>& matrix, int numThreads) {
    int n = matrix.size();
    vector<int> minValues(n);
    vector<thread> threads;

    int rowsPerThread = n / numThreads;
    int remainder = n % numThreads;
    int start = 0;

    for (int t = 0; t < numThreads; t++) {
        int count = rowsPerThread + (t < remainder ? 1 : 0); // якщо не ділиться порівну, то перші потоки отримують на 1 колонку більше.
        int end = start + count;

        threads.push_back(thread([start, end, n, &matrix, &minValues]() {
            for (int i = start; i < end; i++) {
                int col = n - 1 - i;
                int minVal = matrix[0][col];
                for (int j = 1; j < n; j++) {
                    if (matrix[j][col] < minVal)
                        minVal = matrix[j][col];
                }
                minValues[i] = minVal;
            }
            }));
        start = end;
    }

    for (auto& t : threads) {
        t.join();
    }

    for (int i = 0; i < n; i++) { // а потім замінюємо у матриці
        int col = n - 1 - i;
        matrix[i][col] = minValues[i];
    }
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    int n;
    int numThreads;

    do {
        cout << "Введіть розмір матриці (n x n): ";
        cin >> n;
    } while (n > 40000);
    
    
    cout << "Введіть кількість потоків для паралельної версії: ";
    cin >> numThreads;


    vector<vector<int>> matrixSeq(n, vector<int>(n));
    fillMatrix(matrixSeq);

    if (n <= 10) {
        cout << "\nПочаткова матриця:" << endl;
        printMatrix(matrixSeq);
    }

    vector<vector<int>> matrixPar = matrixSeq;  // просто копіюємо матриці, аби не генерувати двічі.

    auto startSeq = high_resolution_clock::now();
    sequentialProcessMatrix(matrixSeq);
    auto endSeq = high_resolution_clock::now();
    auto durationSeq = duration_cast<milliseconds>(endSeq - startSeq).count();

    cout << "\nЧас послідовної обробки: " << durationSeq << " мілісекунд" << endl;


    auto startPar = high_resolution_clock::now();
    parallelProcessMatrix(matrixPar, numThreads);
    auto endPar = high_resolution_clock::now();
    auto durationPar = duration_cast<milliseconds>(endPar - startPar).count();

    cout << "Час паралельної обробки (" << numThreads << " потоків): " << durationPar << " мілісекунд" << endl;

    if (n <= 10) {
        cout << "\nМатриця після обробки:" << endl;
        printMatrix(matrixPar);

    }

    return 0;
}
