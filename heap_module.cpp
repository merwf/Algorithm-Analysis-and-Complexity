#include "heap_module.h"
#include "utils.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <functional>
#include <iomanip>
#include <algorithm>

using namespace std;

struct HeapStats {
    long long comps = 0;
    long long moves = 0;
};

static inline void swapCount(int& a, int& b, HeapStats& st) {
    std::swap(a, b);
    st.moves += 3;
}

class BinaryHeap {
public:
    vector<int> H;
    int N = 0;
    bool isMinHeap = true;

    explicit BinaryHeap(bool minHeap = true, int capacity = 0)
        : H(capacity + 1), N(0), isMinHeap(minHeap) {}

    void clear() { N = 0; }

    bool empty() const { return N == 0; }
    int size() const { return N; }

    // compare: min-heap => a < b ; max-heap => a > b
    bool better(int a, int b, HeapStats& st) const {
        st.comps++;
        return isMinHeap ? (a < b) : (a > b);
    }

    // findMin / findMax
    int findTop(HeapStats& st, bool& ok) const {
        (void)st;
        if (N <= 0) { ok = false; return 0; }
        ok = true;
        return H[1];
    }

    // heapify up
    void heapifyUp(int node, HeapStats& st) {
        while (node > 1) {
            int parent = node / 2;
            if (!better(H[node], H[parent], st)) break;
            swapCount(H[node], H[parent], st);
            node = parent;
        }
    }

    // heapify down (DeleteMin/DeleteMax)
    void heapifyDown(int node, HeapStats& st) {
        while (true) {
            int left = 2 * node;
            int right = left + 1;
            int best = node;

            if (left <= N && better(H[left], H[best], st)) best = left;
            if (right <= N && better(H[right], H[best], st)) best = right;

            if (best == node) break;
            swapCount(H[node], H[best], st);
            node = best;
        }
    }

    // insertHeap
    void insertKey(int key, HeapStats& st) {
        N++;
        if ((int)H.size() <= N) H.resize(N + 1);
        H[N] = key; st.moves++;
        heapifyUp(N, st);
    }

    // deleteMin/deleteMax => deleteTop
    int deleteTop(HeapStats& st, bool& ok) {
        if (N <= 0) { ok = false; return 0; }
        ok = true;

        int top = H[1]; st.moves++;
        H[1] = H[N];   st.moves++;
        N--;
        if (N > 0) heapifyDown(1, st);
        return top;
    }

    // buildHeap (Floyd)  O(N)
    void buildHeap(const vector<int>& arr, HeapStats& st) {
        N = (int)arr.size();
        H.assign(N + 1, 0);
        for (int i = 0; i < N; i++) { H[i + 1] = arr[i]; st.moves++; }

        for (int i = N / 2; i >= 1; i--) {
            heapifyDown(i, st);
        }
    }

    // decreaseKey / increaseKey:
    // MinHeap: decreaseKey -> yukarı; increaseKey -> aşağı
    // MaxHeap: increaseKey -> yukarı; decreaseKey -> aşağı
    void decreaseKey(int index, int newVal, HeapStats& st, bool& ok) {
        if (index < 1 || index > N) { ok = false; return; }
        ok = true;

        int old = H[index];
        H[index] = newVal; st.moves++;

        if (isMinHeap) {
            st.comps++;
            if (newVal < old) heapifyUp(index, st);
            else heapifyDown(index, st);
        } else {
            st.comps++;
            if (newVal < old) heapifyDown(index, st);
            else heapifyUp(index, st);
        }
    }

    void increaseKey(int index, int newVal, HeapStats& st, bool& ok) {
        decreaseKey(index, newVal, st, ok);
    }

    void deleteKey(int index, HeapStats& st, bool& ok) {
        if (index < 1 || index > N) { ok = false; return; }
        ok = true;
        if (isMinHeap) {
            H[index] = std::numeric_limits<int>::min(); st.moves++;
            heapifyUp(index, st);
        } else {
            H[index] = std::numeric_limits<int>::max(); st.moves++;
            heapifyUp(index, st);
        }
        bool ok2;
        (void)deleteTop(st, ok2);
    }

    // merge(H1,H2): elemanları birleştir + buildHeap
    void mergeHeaps(const BinaryHeap& other, HeapStats& st) {
        vector<int> merged;
        merged.reserve(N + other.N);

        for (int i = 1; i <= N; i++) merged.push_back(H[i]);
        for (int i = 1; i <= other.N; i++) merged.push_back(other.H[i]);

        buildHeap(merged, st);
    }
};

// chrono
static volatile long long g_sink = 0;

template <class F>
static long long measureAvgNs(F&& job, int repeat) {
    using clock = chrono::steady_clock;
    long long total = 0;

    for (int r = 0; r < repeat; r++) {
        auto t1 = clock::now();
        long long val = job();
        auto t2 = clock::now();
        total += chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count();
        g_sink += val;
    }
    return total / repeat;
}

struct Row {
    string op;
    long long avgNs = 0;
    long long comps = 0;
    long long moves = 0;
};

static void printRows(const string& title, int n, int repeat, const vector<Row>& rows) {
    cout << "\n=== " << title << " ===\n";
    cout << "n = " << n << "   repeat = " << repeat << "\n\n";

    cout << left << setw(26) << "Op"
         << right << setw(14) << "avg(ns)"
         << right << setw(12) << "avg(us)"
         << right << setw(14) << "comps"
         << right << setw(14) << "moves\n";

    cout << string(26 + 14 + 12 + 14 + 14, '-') << "\n";

    for (auto& r : rows) {
        cout << left << setw(26) << r.op
             << right << setw(14) << r.avgNs
             << right << setw(12) << (r.avgNs / 1000)
             << right << setw(14) << r.comps
             << right << setw(14) << r.moves << "\n";
    }
}

void moduleHeap() {
    cout << "\n=============================\n";
    cout << " 5) HEAP YAPILARI (Binary Heap)\n";
    cout << "=============================\n";

    int n = readInt("Eleman sayisi n (onerilen: 20000): ", 1, 200000);
    int repeat = 15;

    unsigned seed = 42;
    vector<int> data = makeRandomArray(n, 0, 1'000'000, seed);

    int idx1 = max(1, n / 3);
    int idx2 = max(1, n / 2);

    // -------------------- MIN HEAP --------------------
    vector<Row> minRows;
    {
        // buildHeap
        {
            HeapStats st;
            long long ns = measureAvgNs([&]() -> long long {
                BinaryHeap h(true, n);
                HeapStats lst;
                h.buildHeap(data, lst);
                st = lst;
                bool ok;
                int top = h.findTop(lst, ok);
                return ok ? top : 0;
            }, repeat);
            minRows.push_back({"buildHeap (Min)", ns, st.comps, st.moves});
        }

        // insert n
        {
            HeapStats st;
            long long ns = measureAvgNs([&]() -> long long {
                BinaryHeap h(true, n);
                HeapStats lst;
                for (int x : data) h.insertKey(x, lst);
                st = lst;
                bool ok;
                return h.findTop(lst, ok);
            }, repeat);
            minRows.push_back({"insertHeap x n (Min)", ns, st.comps, st.moves});
        }

        // findMin
        {
            HeapStats st;
            long long ns = measureAvgNs([&]() -> long long {
                BinaryHeap h(true, n);
                HeapStats lst;
                h.buildHeap(data, lst);
                bool ok;
                int top = h.findTop(lst, ok);
                st = lst;
                return ok ? top : 0;
            }, repeat);
            minRows.push_back({"findMin (Min)", ns, st.comps, st.moves});
        }

        // deleteMin x n
        {
            HeapStats st;
            long long ns = measureAvgNs([&]() -> long long {
                BinaryHeap h(true, n);
                HeapStats lst;
                h.buildHeap(data, lst);
                long long sum = 0;
                for (int i = 0; i < n; i++) {
                    bool ok;
                    sum += h.deleteTop(lst, ok);
                }
                st = lst;
                return sum;
            }, repeat);
            minRows.push_back({"deleteMin x n (Min)", ns, st.comps, st.moves});
        }

        // decreaseKey / increaseKey
        {
            HeapStats st;
            long long ns = measureAvgNs([&]() -> long long {
                BinaryHeap h(true, n);
                HeapStats lst;
                h.buildHeap(data, lst);

                bool ok1, ok2;
                // decreaseKey
                h.decreaseKey(idx1, -100, lst, ok1);
                // increaseKey
                h.increaseKey(idx2, 2'000'000, lst, ok2);

                st = lst;
                bool ok;
                int top = h.findTop(lst, ok);
                return ok ? top : 0;
            }, repeat);
            minRows.push_back({"decrease/increase (Min)", ns, st.comps, st.moves});
        }

        // deleteKey
        {
            HeapStats st;
            long long ns = measureAvgNs([&]() -> long long {
                BinaryHeap h(true, n);
                HeapStats lst;
                h.buildHeap(data, lst);
                bool ok;
                h.deleteKey(idx1, lst, ok);
                st = lst;
                bool ok2;
                return h.findTop(lst, ok2);
            }, repeat);
            minRows.push_back({"deleteKey (Min)", ns, st.comps, st.moves});
        }

        // merge(H1,H2)
        {
            HeapStats st;
            long long ns = measureAvgNs([&]() -> long long {
                vector<int> a1(data.begin(), data.begin() + n/2);
                vector<int> a2(data.begin() + n/2, data.end());

                BinaryHeap h1(true, n/2);
                BinaryHeap h2(true, n/2);
                HeapStats lst;

                h1.buildHeap(a1, lst);
                h2.buildHeap(a2, lst);

                h1.mergeHeaps(h2, lst); // h1 = merge
                st = lst;

                bool ok;
                return h1.findTop(lst, ok);
            }, repeat);
            minRows.push_back({"merge(H1,H2) (Min)", ns, st.comps, st.moves});
        }
    }

    // -------------------- MAX HEAP --------------------
    vector<Row> maxRows;
    {
        // buildHeap
        {
            HeapStats st;
            long long ns = measureAvgNs([&]() -> long long {
                BinaryHeap h(false, n);
                HeapStats lst;
                h.buildHeap(data, lst);
                st = lst;
                bool ok;
                int top = h.findTop(lst, ok);
                return ok ? top : 0;
            }, repeat);
            maxRows.push_back({"buildHeap (Max)", ns, st.comps, st.moves});
        }

        // insert n
        {
            HeapStats st;
            long long ns = measureAvgNs([&]() -> long long {
                BinaryHeap h(false, n);
                HeapStats lst;
                for (int x : data) h.insertKey(x, lst);
                st = lst;
                bool ok;
                return h.findTop(lst, ok);
            }, repeat);
            maxRows.push_back({"insertHeap x n (Max)", ns, st.comps, st.moves});
        }

        // findMax
        {
            HeapStats st;
            long long ns = measureAvgNs([&]() -> long long {
                BinaryHeap h(false, n);
                HeapStats lst;
                h.buildHeap(data, lst);
                bool ok;
                int top = h.findTop(lst, ok);
                st = lst;
                return ok ? top : 0;
            }, repeat);
            maxRows.push_back({"findMax (Max)", ns, st.comps, st.moves});
        }

        // deleteMax x n
        {
            HeapStats st;
            long long ns = measureAvgNs([&]() -> long long {
                BinaryHeap h(false, n);
                HeapStats lst;
                h.buildHeap(data, lst);
                long long sum = 0;
                for (int i = 0; i < n; i++) {
                    bool ok;
                    sum += h.deleteTop(lst, ok);
                }
                st = lst;
                return sum;
            }, repeat);
            maxRows.push_back({"deleteMax x n (Max)", ns, st.comps, st.moves});
        }

        // decrease/increase (Max heap mantığı ters çalışır)
        {
            HeapStats st;
            long long ns = measureAvgNs([&]() -> long long {
                BinaryHeap h(false, n);
                HeapStats lst;
                h.buildHeap(data, lst);

                bool ok1, ok2;
                // increaseKey
                h.increaseKey(idx1, 2'000'000, lst, ok1);
                // decreaseKey
                h.decreaseKey(idx2, -100, lst, ok2);

                st = lst;
                bool ok;
                int top = h.findTop(lst, ok);
                return ok ? top : 0;
            }, repeat);
            maxRows.push_back({"decrease/increase (Max)", ns, st.comps, st.moves});
        }

        // deleteKey
        {
            HeapStats st;
            long long ns = measureAvgNs([&]() -> long long {
                BinaryHeap h(false, n);
                HeapStats lst;
                h.buildHeap(data, lst);
                bool ok;
                h.deleteKey(idx1, lst, ok);
                st = lst;
                bool ok2;
                return h.findTop(lst, ok2);
            }, repeat);
            maxRows.push_back({"deleteKey (Max)", ns, st.comps, st.moves});
        }

        // merge(H1,H2)
        {
            HeapStats st;
            long long ns = measureAvgNs([&]() -> long long {
                vector<int> a1(data.begin(), data.begin() + n/2);
                vector<int> a2(data.begin() + n/2, data.end());

                BinaryHeap h1(false, n/2);
                BinaryHeap h2(false, n/2);
                HeapStats lst;

                h1.buildHeap(a1, lst);
                h2.buildHeap(a2, lst);

                h1.mergeHeaps(h2, lst);
                st = lst;

                bool ok;
                return h1.findTop(lst, ok);
            }, repeat);
            maxRows.push_back({"merge(H1,H2) (Max)", ns, st.comps, st.moves});
        }
    }

    printRows("MIN HEAP", n, repeat, minRows);
    printRows("MAX HEAP", n, repeat, maxRows);

    cout << "\nNot: Bu modul sadece 'n' degerini kullanicidan alir (onerilen: 20000).\n";
    cout << "Diger islemler otomatik demo yapar ve chrono ile sure olcer.\n";
}
