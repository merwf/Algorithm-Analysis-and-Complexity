#include "dp_module.h"
#include "utils.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <climits>
#include <string>

using namespace std;
static volatile long long g_sink = 0;

template <class F>
static long long measureAvgNsBatch(F&& job, int repeat, int batch) {
    using clock = chrono::steady_clock;

    long long total = 0;
    for (int r = 0; r < repeat; r++) {
        auto t1 = clock::now();
        long long acc = 0;
        for (int b = 0; b < batch; b++) {
            acc += job();
        }
        auto t2 = clock::now();
        total += chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count();
        g_sink += acc;
    }
    // ortalama: (total / repeat) / batch
    long long avg = total / repeat;
    return avg / batch;
}


static void pickMeasureParams(int n, int& repeat, int& batch) {
    if (n >= 10000) { repeat = 10; batch = 1; return; }
    repeat = 30;
    batch = 200;
}

/* =========================
   TABLO YAZDIRMA (kirpma)
   ========================= */
static void print2DTableCropped(const vector<vector<int>>& T,
                               const string& title,
                               int maxRows, int maxCols) {
    cout << "\n--- " << title << " ---\n";
    int R = (int)T.size();
    int C = (int)T[0].size();

    int rShow = min(R, maxRows);
    int cShow = min(C, maxCols);

    cout << "Table size: " << R << "x" << C
         << " (showing " << rShow << "x" << cShow << ")\n\n";

    cout << setw(6) << " ";
    for (int j = 0; j < cShow; j++) cout << setw(5) << j;
    if (cShow < C) cout << " ...";
    cout << "\n";

    for (int i = 0; i < rShow; i++) {
        cout << setw(6) << i;
        for (int j = 0; j < cShow; j++) cout << setw(5) << T[i][j];
        if (cShow < C) cout << " ...";
        cout << "\n";
    }
    if (rShow < R) cout << "...\n";
}

static void print1DTableWrapped(const vector<long long>& T,
                               const string& title,
                               int perLine = 10) {
    cout << "\n--- " << title << " ---\n";
    int n = (int)T.size() - 1;

    for (int i = 0; i <= n; i += perLine) {
        int jEnd = min(n, i + perLine - 1);

        cout << "Index: ";
        for (int j = i; j <= jEnd; j++) cout << setw(6) << j;
        cout << "\nValue: ";
        for (int j = i; j <= jEnd; j++) cout << setw(6) << T[j];
        cout << "\n\n";
    }
}

/* =========================================================
   7.1 Bottom-Up DP (Fibonacci SolTable)
   ========================================================= */
static long long fibBottomUpTable(int n, vector<long long>& SolTable) {
    SolTable.assign(n + 1, 0);
    if (n >= 1) SolTable[1] = 1;
    for (int i = 2; i <= n; i++) SolTable[i] = SolTable[i - 1] + SolTable[i - 2];
    return SolTable[n];
}

/* =========================================================
   7.2 Minimum Cost Path Top-Down
   ========================================================= */
static int mcpTopDownAux(const vector<vector<int>>& M, vector<vector<int>>& dp, int i, int j) {
    int N = (int)M.size();
    if (i == N - 1 && j == N - 1) return M[i][j];

    if (dp[i][j] != -1) return dp[i][j];

    int down = INT_MAX;
    int right = INT_MAX;
    if (i + 1 < N) down = mcpTopDownAux(M, dp, i + 1, j);
    if (j + 1 < N) right = mcpTopDownAux(M, dp, i, j + 1);

    dp[i][j] = M[i][j] + min(down, right);
    return dp[i][j];
}

static int minimumCostPathTopDown(const vector<vector<int>>& M, vector<vector<int>>& dp) {
    int N = (int)M.size();
    dp.assign(N, vector<int>(N, -1));
    return mcpTopDownAux(M, dp, 0, 0);
}

/* =========================================================
   7.3 0/1 Knapsack Bottom-Up DP (Table)
   ========================================================= */
static int knapsackBottomUp(const vector<int>& v, const vector<int>& w, int n, int W,
                            vector<vector<int>>& dp) {
    dp.assign(n + 1, vector<int>(W + 1, 0));

    for (int i = 1; i <= n; i++) {
        for (int cap = 0; cap <= W; cap++) {
            dp[i][cap] = dp[i - 1][cap]; // leave
            if (w[i] <= cap) {          // take
                dp[i][cap] = max(dp[i][cap], v[i] + dp[i - 1][cap - w[i]]);
            }
        }
    }
    return dp[n][W];
}

/* =========================
   DP MENU
   ========================= */
static void printDpMenu() {
    cout << "\nNe gormek istersiniz?\n";
    cout << "1 - Sadece zaman olcumu tablosu\n";
    cout << "2 - Bottom-Up DP Tablosu (Fibonacci SolTable)\n";
    cout << "3 - Min Maliyet Yol (Grid + Memo Tablosu)\n";
    cout << "4 - Knapsack DP Tablosu (ornek kesit)\n";
    cout << "5 - Hepsini goster (onerilmez, uzun)\n";
    cout << "0 - Geri don\n";
}

void moduleDP() {
    cout << "\n=============================\n";
    cout << "7) DINAMIK PROGRAMLAMA (DP)\n";
    cout << "- Bottom-Up DP (Tablo)\n";
    cout << "- Minimum Maliyet Yol (Top-Down / Memo)\n";
    cout << "- Sirt Cantasi (0/1 Knapsack DP Tablo)\n";
    cout << "=============================\n";

    int n = readInt("n (onerilen: 20): ", 5, 200000);

    // Tablo gösterimi için makul boyutlar
    int fibN   = min(n, 30);
    int gridN  = min(max(5, n / 3), 10);
    int knapN  = min(n, 20);
    int W      = 30;

    // otomatik test verileri
    vector<long long> fibTable;

    vector<vector<int>> M(gridN, vector<int>(gridN, 1));
    {
        vector<int> cells = makeRandomArray(gridN * gridN, 1, 9, 42);
        int t = 0;
        for (int i = 0; i < gridN; i++)
            for (int j = 0; j < gridN; j++)
                M[i][j] = cells[t++];
    }
    vector<vector<int>> mcpMemo;
    int mcpAns = 0;

    vector<int> v(knapN + 1, 0), wgt(knapN + 1, 0);
    {
        vector<int> vals = makeRandomArray(knapN, 5, 50, 43);
        vector<int> wgts = makeRandomArray(knapN, 1, 10, 44);
        for (int i = 1; i <= knapN; i++) {
            v[i] = vals[i - 1];
            wgt[i] = wgts[i - 1];
        }
    }
    vector<vector<int>> knapDP;
    int knapAns = 0;

    // ölçüm parametreleri (0 çikmasin diye)
    int repeat, batch;
    pickMeasureParams(n, repeat, batch);

    long long ns_fib = measureAvgNsBatch([&]() -> long long {
        return fibBottomUpTable(fibN, fibTable);
    }, repeat, batch);

    long long ns_mcp = measureAvgNsBatch([&]() -> long long {
        mcpAns = minimumCostPathTopDown(M, mcpMemo);
        return mcpAns;
    }, repeat, batch);

    long long ns_knap = measureAvgNsBatch([&]() -> long long {
        knapAns = knapsackBottomUp(v, wgt, knapN, W, knapDP);
        return knapAns;
    }, repeat, batch);

    auto printTimeTable = [&]() {
        cout << "\n--- DP Zaman Olcumu (chrono) ---\n";
        cout << "(repeat=" << repeat << ", batch=" << batch << ")\n\n";
        cout << left << setw(35) << "Problem"
             << right << setw(14) << "avg(ns)"
             << right << setw(12) << "avg(us)"
             << "\n";
        cout << string(35 + 14 + 12, '-') << "\n";

        cout << left << setw(35) << ("Bottom-Up DP (Fibonacci n=" + to_string(fibN) + ")")
             << right << setw(14) << ns_fib
             << right << setw(12) << (ns_fib / 1000) << "\n";

        cout << left << setw(35) << ("Min Cost Path Top-Down (N=" + to_string(gridN) + ")")
             << right << setw(14) << ns_mcp
             << right << setw(12) << (ns_mcp / 1000) << "\n";

        cout << left << setw(35) << ("Knapsack Bottom-Up (n=" + to_string(knapN) + ", W=" + to_string(W) + ")")
             << right << setw(14) << ns_knap
             << right << setw(12) << (ns_knap / 1000) << "\n";
    };

    // Menü döngüsü: kullanici girdisi
    while (true) {
        printDpMenu();
        int c = readInt("Secim: ", 0, 5);

        if (c == 0) return;

        if (c == 1) {
            printTimeTable();
        }
        else if (c == 2) {
            printTimeTable();
            print1DTableWrapped(fibTable, "Bottom-Up DP Tablosu: Fibonacci SolTable", 10);
        }
        else if (c == 3) {
            printTimeTable();
            print2DTableCropped(M, "Grid M (Maliyetler)", gridN, gridN);
            print2DTableCropped(mcpMemo, "Top-Down Memo Tablosu: dp[i][j]", gridN, gridN);
            cout << "\nMinimum maliyet = dp[0][0] = " << mcpAns << "\n";
        }
        else if (c == 4) {
            printTimeTable();
            int showItems = min(knapN + 1, 7);
            int showW = min(W + 1, 16);
            print2DTableCropped(knapDP, "Knapsack DP Tablosu (ornek: ilk 6 item, W<=15)", showItems, showW);
            cout << "\nKnapsack optimum = dp[" << knapN << "][" << W << "] = " << knapAns << "\n";
        }
        else if (c == 5) {
            printTimeTable();
            print1DTableWrapped(fibTable, "Bottom-Up DP Tablosu: Fibonacci SolTable", 10);
            print2DTableCropped(M, "Grid M (Maliyetler)", gridN, gridN);
            print2DTableCropped(mcpMemo, "Top-Down Memo Tablosu: dp[i][j]", gridN, gridN);
            cout << "\nMinimum maliyet = dp[0][0] = " << mcpAns << "\n";
            int showItems = min(knapN + 1, 7);
            int showW = min(W + 1, 16);
            print2DTableCropped(knapDP, "Knapsack DP Tablosu (ornek: ilk 6 item, W<=15)", showItems, showW);
            cout << "\nKnapsack optimum = dp[" << knapN << "][" << W << "] = " << knapAns << "\n";
        }
    }
}
