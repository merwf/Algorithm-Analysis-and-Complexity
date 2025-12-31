#include "sorting_module.h"
#include "utils.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <limits>
#include <cstdlib>
#include <chrono>
#include <functional>
#include <iomanip>

using namespace std;

struct SortStats {
    long long comps = 0;
    long long swaps = 0; // insertion/merge/counting/radix'te "move" gibi düþünebilirsin
};

static inline void doSwap(int& x, int& y, SortStats& st) {
    std::swap(x, y);
    st.swaps++;
}

/* =========================================================
   1) Bubble Sort
   ========================================================= */
static void bubbleSort(vector<int>& A, SortStats& st) {
    int N = (int)A.size();
    for (int i = 0; i < N; i++) {
        for (int j = 1; j < (N - i); j++) {
            st.comps++;
            if (A[j - 1] > A[j]) {
                doSwap(A[j - 1], A[j], st);
            }
        }
    }
}

/* =========================================================
   2) Selection Sort
   ========================================================= */
static void selectionSort(vector<int>& A, SortStats& st) {
    int N = (int)A.size();
    for (int i = 0; i < N - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < N; j++) {
            st.comps++;
            if (A[j] < A[minIndex]) minIndex = j;
        }
        if (i != minIndex) doSwap(A[i], A[minIndex], st);
    }
}

/* =========================================================
   3) Insertion Sort
   ========================================================= */
static void insertionSort(vector<int>& A, SortStats& st) {
    int N = (int)A.size();
    for (int p = 1; p < N; p++) {
        int tmp = A[p];
        int j = p;
        while (j > 0) {
            st.comps++;
            if (A[j - 1] > tmp) {
                A[j] = A[j - 1]; // shift
                st.swaps++;      // move
                j--;
            } else break;
        }
        A[j] = tmp;
    }
}

/* =========================================================
   4) Merge Sort
   ========================================================= */
static void mergeRange(vector<int>& A, vector<int>& tmp, int L, int mid, int R, SortStats& st) {
    int i = L, j = mid + 1, k = L;
    while (i <= mid && j <= R) {
        st.comps++;
        if (A[i] <= A[j]) tmp[k++] = A[i++];
        else              tmp[k++] = A[j++];
        st.swaps++; // move
    }
    while (i <= mid) { tmp[k++] = A[i++]; st.swaps++; }
    while (j <= R)   { tmp[k++] = A[j++]; st.swaps++; }
    for (int t = L; t <= R; t++) A[t] = tmp[t];
}

static void mergeSortRec(vector<int>& A, vector<int>& tmp, int L, int R, SortStats& st) {
    if (L >= R) return;
    int mid = (L + R) / 2;
    mergeSortRec(A, tmp, L, mid, st);
    mergeSortRec(A, tmp, mid + 1, R, st);
    mergeRange(A, tmp, L, mid, R, st);
}

static void mergeSort(vector<int>& A, SortStats& st) {
    if (A.empty()) return;
    vector<int> tmp(A.size());
    mergeSortRec(A, tmp, 0, (int)A.size() - 1, st);
}

/* =========================================================
   5) QuickSort (Pivot = First)  -- SLAYT mantýðý (pointer QS)
   ========================================================= */
static int partitionFirstPivot(int A[], int N, SortStats& st) {
    if (N <= 1) return 0;

    int pivot = A[0];
    int i = 1, j = N - 1;

    while (true) {
        while (j >= 1) { st.comps++; if (A[j] > pivot) j--; else break; }
        while (i < N)  { st.comps++; if (A[i] < pivot && i < j) i++; else break; }

        if (i >= j) break;
        std::swap(A[i], A[j]); st.swaps++;
        i++; j--;
    }

    std::swap(A[j], A[0]); st.swaps++; // pivot restore
    return j; // pivot index
}

static void quickSortFirstPivot(vector<int>& V, SortStats& st) {
    function<void(int*, int)> QS = [&](int* A, int N) {
        if (N <= 1) return;
        int index = partitionFirstPivot(A, N, st);
        QS(A, index);
        QS(A + index + 1, N - index - 1);
    };
    if (!V.empty()) QS(V.data(), (int)V.size());
}

/* =========================================================
   6) QuickSort (Median-of-3)
   ========================================================= */
static int medianOf3(vector<int>& A, int left, int right, SortStats& st) {
    int center = (left + right) / 2;

    st.comps++; if (A[left] > A[center]) doSwap(A[left], A[center], st);
    st.comps++; if (A[left] > A[right])  doSwap(A[left], A[right], st);
    st.comps++; if (A[center] > A[right]) doSwap(A[center], A[right], st);

    doSwap(A[center], A[right - 1], st); // hide pivot
    return A[right - 1];
}

static int partitionMedianOf3(vector<int>& A, int left, int right, SortStats& st) {
    int pivot = medianOf3(A, left, right, st);
    int i = left, j = right - 1;

    while (true) {
        while (true) { i++; st.comps++; if (!(A[i] < pivot)) break; }
        while (true) { j--; st.comps++; if (!(A[j] > pivot)) break; }
        if (i < j) doSwap(A[i], A[j], st);
        else break;
    }

    doSwap(A[i], A[right - 1], st); // restore pivot
    return i;
}

static void quickSortMedianRec(vector<int>& A, int left, int right, SortStats& st) {
    int N = right - left + 1;
    if (N <= 1) return;

    if (N < 3) { // mini case
        st.comps++; if (A[left] > A[right]) doSwap(A[left], A[right], st);
        return;
    }

    int pivotIndex = partitionMedianOf3(A, left, right, st);
    quickSortMedianRec(A, left, pivotIndex - 1, st);
    quickSortMedianRec(A, pivotIndex + 1, right, st);
}

static void quickSortMedianOf3(vector<int>& A, SortStats& st) {
    if (!A.empty())
        quickSortMedianRec(A, 0, (int)A.size() - 1, st);
}

/* =========================================================
   7) Heap Sort
   ========================================================= */
static void heapifyDown(vector<int>& A, int n, int i, SortStats& st) {
    while (true) {
        int l = 2 * i + 1, r = 2 * i + 2;
        int largest = i;

        if (l < n) { st.comps++; if (A[l] > A[largest]) largest = l; }
        if (r < n) { st.comps++; if (A[r] > A[largest]) largest = r; }

        if (largest == i) break;
        doSwap(A[i], A[largest], st);
        i = largest;
    }
}

static void heapSort(vector<int>& A, SortStats& st) {
    int n = (int)A.size();
    for (int i = n / 2 - 1; i >= 0; i--) heapifyDown(A, n, i, st);
    for (int end = n - 1; end > 0; end--) {
        doSwap(A[0], A[end], st);
        heapifyDown(A, end, 0, st);
    }
}

/* =========================================================
   8) Counting Sort (range)
   ========================================================= */
static bool countingSortRange(vector<int>& A, int minVal, int maxVal, SortStats& st) {
    if (minVal > maxVal) return false;
    long long Bll = 1LL * maxVal - minVal + 1;
    if (Bll <= 0 || Bll > 5'000'000LL) return false;
    int B = (int)Bll;

    vector<int> C(B, 0);
    for (int x : A) {
        if (x < minVal || x > maxVal) return false;
        C[x - minVal]++;
        st.swaps++;
    }

    int k = 0;
    for (int v = 0; v < B; v++) {
        while (C[v]-- > 0) {
            A[k++] = v + minVal;
            st.swaps++;
        }
    }
    return true;
}

/* =========================================================
   9) Radix Sort (>=0)
   ========================================================= */
static void radixCountByExp(vector<int>& A, int exp, SortStats& st) {
    const int base = 10;
    vector<int> out(A.size());
    int cnt[base] = {0};

    for (int x : A) {
        int digit = (x / exp) % 10;
        cnt[digit]++;
        st.swaps++;
    }
    for (int i = 1; i < base; i++) cnt[i] += cnt[i - 1];

    for (int i = (int)A.size() - 1; i >= 0; i--) {
        int digit = (A[i] / exp) % 10;
        out[--cnt[digit]] = A[i];
        st.swaps++;
    }
    A.swap(out);
}

static bool radixSortNonNegative(vector<int>& A, SortStats& st) {
    int mx = 0;
    for (int x : A) {
        if (x < 0) return false;
        mx = max(mx, x);
    }
    for (int exp = 1; mx / exp > 0; exp *= 10) {
        radixCountByExp(A, exp, st);
    }
    return true;
}

/* =========================================================
   10) External Sorting (simulation)
   ========================================================= */
static vector<int> externalSortSim(const vector<int>& input, int M, SortStats& st) {
    int N = (int)input.size();
    if (M <= 0) M = N;

    vector<vector<int>> runs;
    for (int i = 0; i < N; i += M) {
        int len = min(M, N - i);
        vector<int> chunk(input.begin() + i, input.begin() + i + len);
        sort(chunk.begin(), chunk.end());
        runs.push_back(std::move(chunk));
    }

    struct Item { int val, run, idx; };
    struct Cmp { bool operator()(const Item& a, const Item& b) const { return a.val > b.val; } };
    priority_queue<Item, vector<Item>, Cmp> pq;

    for (int r = 0; r < (int)runs.size(); r++) {
        if (!runs[r].empty()) pq.push({runs[r][0], r, 0});
    }

    vector<int> out;
    out.reserve(N);

    while (!pq.empty()) {
        auto cur = pq.top(); pq.pop();
        out.push_back(cur.val);
        st.swaps++;

        int nr = cur.run;
        int ni = cur.idx + 1;
        if (ni < (int)runs[nr].size()) pq.push({runs[nr][ni], nr, ni});
    }

    return out;
}

/* =========================================================
   CHRONO measure (ns)  -> 0us sorunu biter
   ========================================================= */
static volatile long long g_sink = 0;

template <class F>
static long long measureNsRepeatChrono(F&& job, int repeat) {
    using clock = std::chrono::steady_clock;
    long long total = 0;

    for (int r = 0; r < repeat; r++) {
        auto t1 = clock::now();
        int last = job();
        auto t2 = clock::now();
        total += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        g_sink += last; // optimizasyon engeli
    }
    return total / repeat; // avg ns
}

/* =========================================================
   Data generator (senaryo)
   ========================================================= */
enum class DataCase { Random = 1, Sorted = 2, Reverse = 3, FewUnique = 4 };

static vector<int> makeCaseArray(int n, int lo, int hi, unsigned seed, DataCase dc) {
    vector<int> a = makeRandomArray(n, lo, hi, seed);

    if (dc == DataCase::Sorted) {
        sort(a.begin(), a.end());
    } else if (dc == DataCase::Reverse) {
        sort(a.begin(), a.end());
        reverse(a.begin(), a.end());
    } else if (dc == DataCase::FewUnique) {
        // az farklý deðer üret (ör: 0..9 arasý)
        int smallLo = 0, smallHi = 9;
        a = makeRandomArray(n, smallLo, smallHi, seed);
    }
    return a;
}

/* =========================================================
   Run single algorithm (chrono + stats)
   ========================================================= */
struct RunResult {
    string name;
    long long avgNs = 0;
    long long comps = 0;
    long long swaps = 0;
    bool ok = true;
};

static RunResult runAlgoChrono(const vector<int>& base, int algoChoice, int repeat,
                              int minV, int maxV, int memM) {
    RunResult rr;
    SortStats lastStats;
    int lastResult = 0;

    auto runOnce = [&]() -> int {
        vector<int> a = base;
        SortStats st;

        switch (algoChoice) {
            case 1: rr.name = "Bubble";    bubbleSort(a, st); break;
            case 2: rr.name = "Selection"; selectionSort(a, st); break;
            case 3: rr.name = "Insertion"; insertionSort(a, st); break;
            case 4: rr.name = "Merge";     mergeSort(a, st); break;
            case 5: rr.name = "Quick(FirstPivot)";  quickSortFirstPivot(a, st); break;
            case 6: rr.name = "Quick(Median3)";     quickSortMedianOf3(a, st); break;
            case 7: rr.name = "Heap";      heapSort(a, st); break;

            case 8: {
                rr.name = "Counting";
                bool ok = countingSortRange(a, minV, maxV, st);
                if (!ok) rr.ok = false;
                break;
            }
            case 9: {
                rr.name = "Radix(>=0)";
                bool ok = radixSortNonNegative(a, st);
                if (!ok) rr.ok = false;
                break;
            }
            case 10: {
                rr.name = "ExternalSim";
                vector<int> out = externalSortSim(a, memM, st);
                a.swap(out);
                break;
            }
            default:
                rr.name = "Unknown";
                rr.ok = false;
                break;
        }

        lastStats = st;
        lastResult = a.empty() ? 0 : a.back();
        return lastResult;
    };

    rr.avgNs = measureNsRepeatChrono(runOnce, repeat);
    rr.comps = lastStats.comps;
    rr.swaps = lastStats.swaps;
    return rr;
}

/* =========================================================
   Module UI (az input + compare)
   ========================================================= */
static void printSortMenu() {
    cout << "\n=============================\n";
    cout << " 4) SIRALAMA ALGORITMALARI\n";
    cout << "=============================\n";
    cout << "1) Bubble Sort\n";
    cout << "2) Selection Sort\n";
    cout << "3) Insertion Sort\n";
    cout << "4) Merge Sort\n";
    cout << "5) QuickSort (Pivot=First)\n";
    cout << "6) QuickSort (Median-of-3)\n";
    cout << "7) Heap Sort\n";
    cout << "8) Counting Sort (range)\n";
    cout << "9) Radix Sort (>=0)\n";
    cout << "10) External Sorting (Sim)\n";
    cout << "11) Hepsini karsilastir (tablo)\n";
    cout << "0) Geri\n";
}

static int pickNFromPreset() {
    cout << "\nBoyut preset:\n";
    cout << "1) 1000\n";
    cout << "2) 5000\n";
    cout << "3) 20000\n";
    cout << "4) 50000\n";
    cout << "5) 100000\n";
    int c = readInt("Preset sec: ", 1, 5);
    switch (c) {
        case 1: return 1000;
        case 2: return 5000;
        case 3: return 20000;
        case 4: return 50000;
        default: return 100000;
    }
}

static DataCase pickCase() {
    cout << "\nVeri senaryosu:\n";
    cout << "1) Random\n";
    cout << "2) Zaten sirali\n";
    cout << "3) Ters sirali\n";
    cout << "4) Az farkli deger (0..9)\n";
    int c = readInt("Secim: ", 1, 4);
    return (DataCase)c;
}

static int autoRepeatForN(int n) {
    // süre 0us olmasýn diye otomatik repeat
    if (n <= 2000) return 50;
    if (n <= 10000) return 20;
    return 10;
}

void moduleSorting() {
    while (true) {
        printSortMenu();
        int algo = readInt("Secim: ", 0, 11);
        if (algo == 0) return;

        DataCase dc = pickCase();
        int n = pickNFromPreset();

        // aralýklarý otomatik verelim (elle uðraþtýrmasýn)
        int lo = 0, hi = 1000000;
        if (dc == DataCase::FewUnique) { lo = 0; hi = 9; }

        // Radix için negatif yok:
        if (algo == 9) { lo = 0; if (hi < 0) hi = 1000000; }

        unsigned seed = 42;
        vector<int> base = makeCaseArray(n, lo, hi, seed, dc);

        cout << "\nOrnek (ilk 10): ";
        printVectorFirst(base, 10);

        // repeat otomatik, istersen yine sorabiliriz ama uzamasýn
        int repeat = autoRepeatForN(n);
        cout << "repeat (auto) = " << repeat << "\n";

        // Counting için range küçük olmalý -> fewunique seçilirse çok iyi çalýþýr
        int minV = lo, maxV = hi;
        if (algo == 8 && (long long)maxV - minV + 1 > 5'000'000LL) {
            cout << "Counting icin range cok buyuk! (FewUnique secmen daha iyi)\n";
        }

        // External sim için chunk size (M)
        int memM = (algo == 10) ? max(1, n / 10) : n;

        if (algo == 11) {
            // Hepsini karþýlaþtýr tablosu
            cout << "\n--- Karsilastirma Tablosu ---\n";
            cout << "n=" << n << "  case=" << (int)dc << "  repeat=" << repeat << "\n\n";

            cout << left
                 << setw(18) << "Algo"
                 << right << setw(14) << "avg(ns)"
                 << right << setw(12) << "avg(us)"
                 << right << setw(14) << "comps"
                 << right << setw(14) << "moves"
                 << "\n";

            cout << string(18+14+12+14+14, '-') << "\n";

            // O(n^2) olanlarý büyük n'de istersen atlayabiliriz ama þimdilik hepsini çalýþtýrýyoruz.
            for (int a = 1; a <= 10; a++) {
                // Radix >=0, Counting range kontrolü: base zaten uygun üretildi
                RunResult r = runAlgoChrono(base, a, repeat, minV, maxV, memM);

                if (!r.ok) {
                    cout << left << setw(18) << r.name
                         << right << setw(14) << "-" << setw(12) << "-"
                         << right << setw(14) << "-" << setw(14) << "-"
                         << "   (SKIP)\n";
                    continue;
                }

                cout << left << setw(18) << r.name
                     << right << setw(14) << r.avgNs
                     << right << setw(12) << (r.avgNs / 1000)
                     << right << setw(14) << r.comps
                     << right << setw(14) << r.swaps
                     << "\n";
            }
            continue;
        }

        // Tek algoritma çalýþtýr
        RunResult r = runAlgoChrono(base, algo, repeat, minV, maxV, memM);

        cout << "\n=== " << r.name << " ===\n";
        cout << "n = " << n << ", repeat = " << repeat << "\n";
        cout << "Time (avg, ns) = " << r.avgNs << "\n";
        cout << "Time (avg, us) = " << (r.avgNs / 1000) << "\n";
        cout << "Comparisons = " << r.comps << "\n";
        cout << "Swaps/Moves = " << r.swaps << "\n";
    }
}
