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
    mt19937 gen(rd()); // ������������� mt19997 ��� ������-�������
    uniform_int_distribution<> dis(0, 99); // ������������� ��������� ������� �� 0 �� 99

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

    for (int i = 0; i < n; i++) { // �������� ����������� ������� ��������
        int col = n - 1 - i;
        int minVal = matrix[0][col];
        for (int j = 1; j < n; j++) {
            if (matrix[j][col] < minVal)
                minVal = matrix[j][col];
        }
        minValues[i] = minVal;
    }
    for (int i = 0; i < n; i++) { // � ���� �������� � �������
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
        int count = rowsPerThread + (t < remainder ? 1 : 0); // ���� �� ������� ������, �� ����� ������ ��������� �� 1 ������� �����.
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

    for (int i = 0; i < n; i++) { // � ���� �������� � �������
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
        cout << "������ ����� ������� (n x n): ";
        cin >> n;
    } while (n > 40000);
    
    
    cout << "������ ������� ������ ��� ���������� ����: ";
    cin >> numThreads;


    vector<vector<int>> matrixSeq(n, vector<int>(n));
    fillMatrix(matrixSeq);

    if (n <= 10) {
        cout << "\n��������� �������:" << endl;
        printMatrix(matrixSeq);
    }

    vector<vector<int>> matrixPar = matrixSeq;  // ������ ������� �������, ��� �� ���������� ����.

    auto startSeq = high_resolution_clock::now();
    sequentialProcessMatrix(matrixSeq);
    auto endSeq = high_resolution_clock::now();
    auto durationSeq = duration_cast<milliseconds>(endSeq - startSeq).count();

    cout << "\n��� ��������� �������: " << durationSeq << " ��������" << endl;


    auto startPar = high_resolution_clock::now();
    parallelProcessMatrix(matrixPar, numThreads);
    auto endPar = high_resolution_clock::now();
    auto durationPar = duration_cast<milliseconds>(endPar - startPar).count();

    cout << "��� ���������� ������� (" << numThreads << " ������): " << durationPar << " ��������" << endl;

    if (n <= 10) {
        cout << "\n������� ���� �������:" << endl;
        printMatrix(matrixPar);

    }

    return 0;
}
