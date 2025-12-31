#include "recursive_module.h"
#include "utils.h"

#include <iostream>
#include <vector>
#include <limits>

using namespace std;

/* -------------------- 2) Rekursif Algoritmalar (CALL COUNTER'LI) -------------------- */

// 1) 1+2+...+N
static long long sum1toN_rec(int n, long long& calls) {
    calls++;
    if (n <= 1) return 1;              // base case
    return sum1toN_rec(n - 1, calls) + n;
}

// 2) Dizi toplam
static long long sumArray_rec(const vector<int>& a, int n, long long& calls) {
    calls++;
    if (n == 1) return a[0];           // base case
    return sumArray_rec(a, n - 1, calls) + a[n - 1];
}

// 3) a^n (lineer)
static long long power_rec_linear(long long a, int n, long long& calls) {
    calls++;
    if (n == 0) return 1;              // base case
    return a * power_rec_linear(a, n - 1, calls);
}

// 3) a^n (speed)
static long long power_rec_fast(long long a, int n, long long& calls) {
    calls++;
    if (n == 0) return 1;
    if (n == 1) return a;
    long long half = power_rec_fast(a, n / 2, calls);
    if (n % 2 == 0) return half * half;
    return half * half * a;
}

// 4) Fibonacci
static long long fib_rec(int n, long long& calls) {
    calls++;
    if (n == 0) return 0;
    if (n == 1) return 1;
    return fib_rec(n - 1, calls) + fib_rec(n - 2, calls);
}

// 5) Tower of Hanoi
static void hanoi_rec(int n, char from, char to, char aux,
                      long long& calls, long long& moves, bool printMoves) {
    calls++;
    if (n == 0) return;
    hanoi_rec(n - 1, from, aux, to, calls, moves, printMoves);
    moves++;
    if (printMoves) {
        cout << "Move disk " << n << " : " << from << " -> " << to << "\n";
    }
    hanoi_rec(n - 1, aux, to, from, calls, moves, printMoves);
}

// 6) Recursive Digit Sum
static int digitSum_rec(long long n, long long& calls) {
    calls++;
    if (n < 10) return (int)n;
    return digitSum_rec(n / 10, calls) + (int)(n % 10);
}

void moduleRecursive() {
    cout << "\n--- 2) Rekursif Algoritmalar ---\n";
    cout << "Bu modulde her problem icin REKURSIF CAGRI SAYISI (fonksiyonun kendini kac kez cagirdigi) hesaplanir.\n";

    while (true) {
        cout << "\n[2] Rekursif Modul Menusu\n";
        cout << "1 - 1+2+...+N\n";
        cout << "2 - Dizi Toplami\n";
        cout << "3 - a^n (linear) + (fast opsiyonel)\n";
        cout << "4 - Fibonacci (naive)\n";
        cout << "5 - Tower of Hanoi\n";
        cout << "6 - Recursive Digit Sum\n";
        cout << "0 - Geri Don\n";

        int choice = readInt("Secim: ", 0, 6);
        if (choice == 0) return;

        // 1) 1+2+...+N
        if (choice == 1) {
            int n = readInt("N (1'den N'e kadar toplanir | Onerilen aralik: 10 - 100000): ", 1, 1000000);

            long long calls = 0;
            long long ans = sum1toN_rec(n, calls);

            cout << "\nProblem: 1 + 2 + ... + N (Rekursif)\n";
            cout << "N = " << n << "\n";
            cout << "Hesaplanan Toplam: " << ans << "\n";
            cout << "Rekursif cagri sayisi: " << calls << "\n";
            cout << "Teori: T(N) = N, Zaman: O(N)\n";
        }

        // 2) Dizi Toplami
        else if (choice == 2) {
            int n = readInt("Dizi boyutu N (Elle giris icin <=20 | buyuk N otomatik): ", 1, 200000);
            vector<int> a(n);

            if (n <= 20) {
                cout << "Elemanlari gir (" << n << " adet, aralarda bosluk):\n";
                for (int i = 0; i < n; i++) cin >> a[i];
            } else {
                cout << "Dizi otomatik olusturuluyor (rastgele 1-10, seed=42)...\n";
                a = makeRandomArray(n, 1, 10, 42);
            }

            long long calls = 0;
            long long ans = sumArray_rec(a, n, calls);

            cout << "\nProblem: Dizi Toplami (Rekursif)\n";
            cout << "N = " << n << "\n";
            if (n <= 20) {
                cout << "Dizi: ";
                printVector(a);
            } else {
                cout << "Dizi ornegi (ilk 10): ";
                printVectorFirst(a, 10);
            }

            cout << "Toplam: " << ans << "\n";
            cout << "Rekursif cagri sayisi: " << calls << "\n";
            cout << "Teori: T(N) = N, Zaman: O(N)\n";
        }

        // 3) a^n
        else if (choice == 3) {
            long long a = readInt("a (taban/base): ", -100000, 100000);
            int n = readInt("n (us/exp | Onerilen aralik: 0 - 40): ", 0, 60);

            long long callsLin = 0;
            long long ansLin = power_rec_linear(a, n, callsLin);

            cout << "\nProblem: a^n (Rekursif)\n";
            cout << "a = " << a << ", n = " << n << "\n";
            cout << "Sonuc (linear): " << ansLin << "\n";
            cout << "Rekursif cagri (linear): " << callsLin << "\n";
            cout << "Teori: Zaman O(n)\n";

            int wantFast = readInt("Bonus: hizli us alma (log n) hesapla? (0=Hayir, 1=Evet): ", 0, 1);
            if (wantFast == 1) {
                long long callsFast = 0;
                long long ansFast = power_rec_fast(a, n, callsFast);
                cout << "Sonuc (fast): " << ansFast << "\n";
                cout << "Rekursif cagri (fast): " << callsFast << "\n";
                cout << "Teori: Zaman O(log n)\n";
            }

            cout << "Not: buyuk degerlerde long long overflow olabilir.\n";
        }

        // 4) Fibonacci (naive)
        else if (choice == 4) {
            int n = readInt("Fibonacci icin n (Onerilen aralik: 0 - 40): ", 0, 45);

            long long calls = 0;
            long long ans = fib_rec(n, calls);

            cout << "\nProblem: Fibonacci (Naive Rekursif)\n";
            cout << "F(" << n << ") = " << ans << "\n";
            cout << "Rekursif cagri sayisi: " << calls << "\n";
            cout << "Not: naive fibonacci ustel buyur, n buyudukce cok yavaslar.\n";
        }

        // 5) Tower of Hanoi
        else if (choice == 5) {
            int n = readInt("Disk sayisi n (Onerilen aralik: 1 - 15): ", 1, 20);
            int pm = readInt("Hareketleri yazdir? (0=Hayir, 1=Evet): ", 0, 1);
            bool printMoves = (pm == 1 && n <= 12);

            long long calls = 0, moves = 0;
            hanoi_rec(n, 'A', 'C', 'B', calls, moves, printMoves);

            cout << "\nProblem: Tower of Hanoi\n";
            cout << "n = " << n << "\n";
            cout << "Toplam move: " << moves << "  (teori: 2^n - 1)\n";
            cout << "Rekursif cagri sayisi: " << calls << "\n";
            cout << "Zaman: O(2^n)\n";
        }

        // 6) Recursive Digit Sum
        else if (choice == 6) {
            cout << "Sayi gir (>=0): " << flush;
            long long x;
            cin >> x;
            if (x < 0) x = -x;

            long long calls = 0;
            int ans = digitSum_rec(x, calls);

            cout << "\nProblem: Basamak Toplami (Rekursif)\n";
            cout << "Sayi = " << x << "\n";
            cout << "Basamak toplami = " << ans << "\n";
            cout << "Rekursif cagri sayisi: " << calls << "\n";
            cout << "Not: cagri sayisi yaklasik basamak sayisi kadardir.\n";
        }
    }
}
