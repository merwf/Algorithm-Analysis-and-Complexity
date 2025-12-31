#include "greedy_module.h"
#include "utils.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <string>
#include <chrono>
#include <iomanip>

using namespace std;

/* =======================
   Chrono ölçüm (avg ns)
   ======================= */
static volatile long long g_sink = 0;

template <class F>
static long long measureAvgNs(F&& job, int repeat) {
    using clock = chrono::steady_clock;
    long long total = 0;

    for (int r = 0; r < repeat; r++) {
        auto t1 = clock::now();
        long long v = job();
        auto t2 = clock::now();
        total += chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count();
        g_sink += v; // optimizasyon engeli
    }
    return total / repeat;
}

static int autoRepeatForN(int n) {
    if (n <= 2000) return 50;
    if (n <= 20000) return 20;
    return 10;
}

/* =========================================================
   6.1 Ýþ Zamanlama Problemi (GreedyJobScheduler)
   - GreedyJobScheduler1: iþleri süreye göre sýrala (SJF)
   - GreedyJobScheduler2: sürelerden min-heap kur, sýrayla çek
   Her ikisi de O(n log n)
   ========================================================= */
struct Job {
    int id;
    int duration;
};

// Slayt: "Ýþleri çalýþma sürelerine göre sýrala. En kýsa iþ önce."
static long long greedyJobScheduler1(vector<Job>& jobs) {
    sort(jobs.begin(), jobs.end(),
         [](const Job& a, const Job& b){ return a.duration < b.duration; });

    // çýktý üretmiyoruz; sadece bir "toplam" hesaplayýp sink'e atýyoruz
    long long totalCompletion = 0;
    long long t = 0;
    for (auto& j : jobs) {
        t += j.duration;
        totalCompletion += t;
    }
    return totalCompletion;
}

// Slayt: "Ýþlerin sürelerinden min-heap oluþtur, en küçüðü çek"
static long long greedyJobScheduler2(const vector<Job>& jobs) {
    priority_queue<int, vector<int>, greater<int>> pq;
    for (auto& j : jobs) pq.push(j.duration);

    long long totalCompletion = 0;
    long long t = 0;
    while (!pq.empty()) {
        int d = pq.top(); pq.pop();
        t += d;
        totalCompletion += t;
    }
    return totalCompletion;
}

/* =========================================================
   6.2 Aktivite Zamanlama Problemi (ActivityScheduling)
   Slayt: finish time'a göre sýrala, ilkini al, çakýþmayanlarý seç
   O(n log n) (sort) + O(n)
   ========================================================= */
struct Activity {
    int id;
    int s;
    int f;
};

static vector<Activity> activityScheduling(vector<Activity> acts) {
    sort(acts.begin(), acts.end(),
         [](const Activity& a, const Activity& b){ return a.f < b.f; });

    vector<Activity> selected;
    if (acts.empty()) return selected;

    selected.push_back(acts[0]);
    int prev = 0;

    for (int i = 1; i < (int)acts.size(); i++) {
        if (acts[i].s >= acts[prev].f) {
            selected.push_back(acts[i]);
            prev = i;
        }
    }
    return selected;
}

/* =========================================================
   6.3 Huffman Kodlama (encode/decode)
   Slayt mantýðý:
   - frekanslarý say
   - min-heap Q (freq'e göre)
   - extractMin x2, yeni z node, tekrar insert
   - aðaç oluþunca kodlarý üret (0/1)
   - encode: metni koda çevir
   - decode: bitleri aðaçta gez
   ========================================================= */
struct HuffNode {
    unsigned char ch;
    int freq;
    HuffNode* left;
    HuffNode* right;
    HuffNode(unsigned char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    HuffNode(HuffNode* l, HuffNode* r) : ch(0), freq(l->freq + r->freq), left(l), right(r) {}
};

struct HuffCmp {
    bool operator()(const HuffNode* a, const HuffNode* b) const {
        return a->freq > b->freq; // min-heap
    }
};

static void buildCodes(HuffNode* node, const string& path,
                       unordered_map<unsigned char, string>& codes) {
    if (!node) return;
    if (!node->left && !node->right) {
        // Tek karakterlik metinde boþ kod olmasýn diye "0" ver
        codes[node->ch] = path.empty() ? "0" : path;
        return;
    }
    buildCodes(node->left, path + "0", codes);
    buildCodes(node->right, path + "1", codes);
}

static void freeTree(HuffNode* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

static HuffNode* buildHuffmanTree(const string& text) {
    int C[256] = {0};
    for (unsigned char c : text) C[c]++;

    priority_queue<HuffNode*, vector<HuffNode*>, HuffCmp> Q;

    for (int i = 0; i < 256; i++) {
        if (C[i] > 0) {
            HuffNode* x = new HuffNode((unsigned char)i, C[i]);
            Q.push(x);
        }
    }

    if (Q.empty()) return nullptr;

    while ((int)Q.size() > 1) {
        HuffNode* x = Q.top(); Q.pop();
        HuffNode* y = Q.top(); Q.pop();
        HuffNode* z = new HuffNode(x, y);
        Q.push(z);
    }
    return Q.top();
}

static string huffmanEncode(const string& text,
                            unordered_map<unsigned char,string>& codes,
                            HuffNode*& rootOut) {
    rootOut = buildHuffmanTree(text);
    codes.clear();
    if (!rootOut) return "";

    buildCodes(rootOut, "", codes);

    string bits;
    bits.reserve(text.size() * 2);
    for (unsigned char c : text) bits += codes[c];
    return bits;
}

static string huffmanDecode(const string& bits, HuffNode* root) {
    if (!root) return "";
    // Tek sembol varsa: her bit ayný karakter
    if (!root->left && !root->right) {
        return string(bits.size(), (char)root->ch);
    }

    string out;
    out.reserve(bits.size() / 2);

    HuffNode* cur = root;
    for (char b : bits) {
        cur = (b == '0') ? cur->left : cur->right;
        if (!cur->left && !cur->right) {
            out.push_back((char)cur->ch);
            cur = root;
        }
    }
    return out;
}

/* =========================================================
   ModuleGreedy: tek input = n (onerilen: 20000)
   ========================================================= */
void moduleGreedy() {
    cout << "\n=============================\n";
    cout << " 6) GREEDY ALGORITMALAR\n";
    cout << "  - Is Zamanlama (2 yontem)\n";
    cout << "  - Aktivite Zamanlama\n";
    cout << "  - Huffman Kodlama (enc/dec)\n";
    cout << "=============================\n";

    int n = readInt("n (onerilen: 20000): ", 10, 200000);
    int repeat = autoRepeatForN(n);

    unsigned seed = 42;

    // ---------- 6.1 Jobs ----------
    vector<int> durs = makeRandomArray(n, 1, 1000, seed); // süreler 1..1000
    vector<Job> jobs; jobs.reserve(n);
    for (int i = 0; i < n; i++) jobs.push_back({i+1, durs[i]});

    // ---------- 6.2 Activities ----------
    // Basit üretim: start random, finish = start + duration
    vector<int> starts = makeRandomArray(n, 0, 100000, seed);
    vector<int> lens   = makeRandomArray(n, 1, 1000, seed+1);

    vector<Activity> acts; acts.reserve(n);
    for (int i = 0; i < n; i++) {
        int s = starts[i];
        int f = s + lens[i];
        acts.push_back({i+1, s, f});
    }

    // ---------- 6.3 Huffman text ----------
    // textLen = min(n, 50000) (çok büyütmeyelim)
    int textLen = min(n, 50000);
    vector<int> chars = makeRandomArray(textLen, 32, 122, seed+2); // okunabilir ASCII
    string text; text.reserve(textLen);
    for (int x : chars) text.push_back((char)x);

    cout << "\nrepeat(auto) = " << repeat << "\n";

    // ======== Ölçüm Tablosu ========
    struct Row { string name; long long ns; };
    vector<Row> rows;

    // JobScheduler1
    long long ns_js1 = measureAvgNs([&]() -> long long {
        auto tmp = jobs;
        return greedyJobScheduler1(tmp);
    }, repeat);
    rows.push_back({"JobScheduler1 (sort)", ns_js1});

    // JobScheduler2
    long long ns_js2 = measureAvgNs([&]() -> long long {
        return greedyJobScheduler2(jobs);
    }, repeat);
    rows.push_back({"JobScheduler2 (min-heap)", ns_js2});

    // ActivityScheduling
    long long ns_act = measureAvgNs([&]() -> long long {
        auto sel = activityScheduling(acts);
        return (long long)sel.size();
    }, repeat);
    rows.push_back({"ActivityScheduling", ns_act});

    // Huffman Encode
    long long ns_henc = 0;
    long long bitsLen = 0;
    ns_henc = measureAvgNs([&]() -> long long {
        unordered_map<unsigned char, string> codes;
        HuffNode* root = nullptr;
        string bits = huffmanEncode(text, codes, root);
        long long L = (long long)bits.size();
        freeTree(root);
        bitsLen = L;
        return L;
    }, repeat);
    rows.push_back({"Huffman Encode", ns_henc});

    // Huffman Decode (encode sonrasý bit string gerekli)
    // Decode ölçümünde encode'yu dýþarýda bir kez hazýrlayalým (ölçüm içine karýþmasýn)
    unordered_map<unsigned char, string> codes0;
    HuffNode* root0 = nullptr;
    string bits0 = huffmanEncode(text, codes0, root0);

    long long ns_hdec = measureAvgNs([&]() -> long long {
        string out = huffmanDecode(bits0, root0);
        return (long long)out.size();
    }, repeat);
    rows.push_back({"Huffman Decode", ns_hdec});

    freeTree(root0);

    // ---- Yazdýr ----
    cout << "\n--- Greedy Zaman Olcumu (chrono) ---\n";
    cout << left << setw(28) << "Algoritma"
         << right << setw(14) << "avg(ns)"
         << right << setw(12) << "avg(us)"
         << "\n";
    cout << string(28 + 14 + 12, '-') << "\n";
    for (auto& r : rows) {
        cout << left << setw(28) << r.name
             << right << setw(14) << r.ns
             << right << setw(12) << (r.ns / 1000)
             << "\n";
    }

    cout << "\nHuffman: textLen=" << textLen << ", encodedBits=" << bitsLen << "\n";
    cout << "(Not: GreedyJobScheduler1/2 ve ActivityScheduling teoride O(n log n) beklenir.)\n";
}
