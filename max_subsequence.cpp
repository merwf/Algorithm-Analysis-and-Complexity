// max_subsequence.cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>

#include "utils.h"
#include "max_subsequence.h"

using namespace std;

// -------------------- helpers (dosya ici) --------------------
static inline int max3(int a, int b, int c) {
    return max(a, max(b, c));
}

// Divide & Conquer helper (Alg3)
static int maxSumRec(const vector<int>& a, int left, int right) {
    if (left == right) {
        return max(0, a[left]); // slayt sarti: negatifse 0
    }

    int center = (left + right) / 2;
    int maxLeftSum  = maxSumRec(a, left, center);
    int maxRightSum = maxSumRec(a, center + 1, right);

    int maxLeftBorderSum = 0, leftBorderSum = 0;
    for (int i = center; i >= left; i--) {
        leftBorderSum += a[i];
        if (leftBorderSum > maxLeftBorderSum)
            maxLeftBorderSum = leftBorderSum;
    }

    int maxRightBorderSum = 0, rightBorderSum = 0;
    for (int i = center + 1; i <= right; i++) {
        rightBorderSum += a[i];
        if (rightBorderSum > maxRightBorderSum)
            maxRightBorderSum = rightBorderSum;
    }

    return max3(maxLeftSum, maxRightSum, maxLeftBorderSum + maxRightBorderSum);
}

// Tek iþi: job'i repeat kez calistir, toplam nanosecond ver
static long long measureNsRepeat(const function<int()>& job, int repeat, int& lastResult) {
    auto start = chrono::high_resolution_clock::now();
    int r = 0;
    for (int i = 0; i < repeat; i++) r = job();
    auto end = chrono::high_resolution_clock::now();
    lastResult = r;
    return chrono::duration_cast<chrono::nanoseconds>(end - start).count();
}

// Pozitif avg'ler icinden en kucugunu bul (x kat yavas hesabi icin)
static double minPositive(double a, double b, double c, double d) {
    double best = 0.0;
    for (double x : {a, b, c, d}) {
        if (x > 0.0 && (best == 0.0 || x < best)) best = x;
    }
    return best; // 0 => hepsi 0 (cok kucuk N, cok hizli)
}

// -------------------- Algorithms --------------------

// Alg 1: O(n^3)
int maxSubSum1(const vector<int>& a) {
    int maxSum = 0;
    for (int i = 0; i < (int)a.size(); i++) {
        for (int j = i; j < (int)a.size(); j++) {
            int thisSum = 0;
            for (int k = i; k <= j; k++)
                thisSum += a[k];
            if (thisSum > maxSum) maxSum = thisSum;
        }
    }
    return maxSum;
}

// Alg 2: O(n^2)
int maxSubSum2(const vector<int>& a) {
    int maxSum = 0;
    for (int i = 0; i < (int)a.size(); i++) {
        int thisSum = 0;
        for (int j = i; j < (int)a.size(); j++) {
            thisSum += a[j];
            if (thisSum > maxSum) maxSum = thisSum;
        }
    }
    return maxSum;
}

// Alg 3: O(n log n) Divide & Conquer
int maxSubSum3(const vector<int>& a) {
    if (a.empty()) return 0;
    return maxSumRec(a, 0, (int)a.size() - 1);
}

// Alg 4: O(n) Kadane (slayttaki)
int maxSubSum4(const vector<int>& a) {
    int maxSum = 0, thisSum = 0;
    for (int x : a) {
        thisSum += x;
        if (thisSum > maxSum) maxSum = thisSum;
        else if (thisSum < 0) thisSum = 0;
    }
    return maxSum;
}

// -------------------- Module UI --------------------
void moduleMaxSubsequence() {
    cout << "\n--- 1) Maximum Subsequence Problem ---\n";
    cout << "Slayt sarti: maksimum deger negatif cikarsa 0 kabul edilir.\n\n";

    // Tek test tipi: kullanicidan N al (slayt ornegini istersen tekrar ekleriz)
    int n = readInt("Dizi boyutu N (10-5000 arasi onerilir): ", 1, 200000);
    vector<int> a = makeRandomArray(n, -50, 50, 42);

    cout << "\nDizi boyutu (N): " << a.size() << "\n";
    cout << "Dizi ornegi (ilk 10): ";
    printVectorFirst(a, 10);
    cout << "\n";

    // repeat ayarlari
    bool runN3 = (a.size() <= 800);
    int repeat    = (a.size() <= 2000 ? 5000 : 500);
    int repeatN3  = (a.size() <= 50 ? 2000 : 200);

    cout << "Olcum ayarlari: repeat=" << repeat << ", repeatN3=" << repeatN3 << "\n\n";

    int r1=-1, r2=-1, r3=-1, r4=-1;
    long long ns1=0, ns2=0, ns3=0, ns4=0;

    if (runN3) {
        ns1 = measureNsRepeat([&](){ return maxSubSum1(a); }, repeatN3, r1);
    }

    ns2 = measureNsRepeat([&](){ return maxSubSum2(a); }, repeat, r2);
    ns3 = measureNsRepeat([&](){ return maxSubSum3(a); }, repeat, r3);
    ns4 = measureNsRepeat([&](){ return maxSubSum4(a); }, repeat, r4);

    double avg1 = (runN3 && repeatN3>0) ? (double)ns1 / repeatN3 : 0.0;
    double avg2 = (repeat>0) ? (double)ns2 / repeat : 0.0;
    double avg3 = (repeat>0) ? (double)ns3 / repeat : 0.0;
    double avg4 = (repeat>0) ? (double)ns4 / repeat : 0.0;

    // Referans: 0'dan buyuk en kucuk avg
    double ref = minPositive(avg1, avg2, avg3, avg4);

    cout << left
         << setw(6)  << "Alg"
         << setw(12) << "Karmasik"
         << setw(10) << "Sonuc"
         << setw(18) << "Total(ns)"
         << setw(18) << "Avg(ns/run)"
         << "x (ref)\n";
    cout << string(70, '-') << "\n";

    auto printRow = [&](const string& name, const string& comp, int res,
                        long long totalNs, double avgNs) {
        cout << left
             << setw(6)  << name
             << setw(12) << comp
             << setw(10) << res
             << setw(18) << totalNs
             << setw(18) << fixed << setprecision(3) << avgNs;

        if (ref > 0.0 && avgNs > 0.0) {
            cout << fixed << setprecision(2) << (avgNs / ref) << "x";
        } else {
            cout << "-";
        }
        cout << "\n";
    };

    if (runN3) printRow("Alg1", "O(n^3)",    r1, ns1, avg1);
    else       cout << "Alg1  O(n^3)   ATLANDI (N cok buyuk)\n";

    printRow("Alg2", "O(n^2)",    r2, ns2, avg2);
    printRow("Alg3", "O(nlogn)",  r3, ns3, avg3);
    printRow("Alg4", "O(n)",      r4, ns4, avg4);

    cout << "\nKontrol: Sonuclar eslesmeli (slayt sartina gore).\n";
    bool ok = true;
    if (runN3) ok = (r1 == r2 && r2 == r3 && r3 == r4);
    else       ok = (r2 == r3 && r3 == r4);

    if (!ok) cout << "UYARI: Sonuclar farkli! (Kod/uyarlama kontrol edilmeli)\n";
    else     cout << "OK: Sonuclar ayni.\n";
}
