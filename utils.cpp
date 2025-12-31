#include "utils.h"
#include <iostream>
#include <limits>
#include <random>
#include <chrono>

using namespace std;

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int readInt(const string& prompt, int minVal, int maxVal) {
    while (true) {
        cout << prompt << flush;
        int x;
        if (cin >> x) {
            if (x >= minVal && x <= maxVal) return x;
            cout << "Hata: " << minVal << " ile " << maxVal << " arasinda gir.\n";
            clearInput();
        } else {
            cout << "Hata: Sayi girmen lazim.\n";
            clearInput();
        }
    }
}

vector<int> makeRandomArray(int n, int lo, int hi, unsigned seed) {
    mt19937 rng(seed);
    uniform_int_distribution<int> dist(lo, hi);
    vector<int> a(n);
    for (int i = 0; i < n; i++) a[i] = dist(rng);
    return a;
}

void printVectorFirst(const vector<int>& a, int k) {
    cout << "[";
    for (int i = 0; i < (int)a.size() && i < k; i++) {
        cout << a[i];
        if (i != k - 1 && i != (int)a.size() - 1) cout << ", ";
    }
    if ((int)a.size() > k) cout << ", ...";
    cout << "]\n";
}

void printVector(const vector<int>& a) {
    cout << "[";
    for (size_t i = 0; i < a.size(); i++) {
        cout << a[i];
        if (i + 1 != a.size()) cout << ", ";
    }
    cout << "]\n";
}

long long measureUsRepeat(const function<int()>& job, int repeat, int& lastResult) {
    auto start = chrono::high_resolution_clock::now();
    int r = 0;
    for (int i = 0; i < repeat; i++) r = job();
    auto end = chrono::high_resolution_clock::now();
    lastResult = r;
    return chrono::duration_cast<chrono::microseconds>(end - start).count();
}
