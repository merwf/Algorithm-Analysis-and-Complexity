#include "searching_module.h"
#include "utils.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstdlib>

using namespace std;

// Ileri yonlu lineer arama
static int linearSearchForward(const vector<int>& a, int key, long long& comps) {
    comps = 0;
    for (int i = 0; i < (int)a.size(); i++) {
        comps++;
        if (a[i] == key) return i;
    }
    return -1;
}

// Geri yonlu lineer arama
static int linearSearchBackward(const vector<int>& a, int key, long long& comps) {
    comps = 0;
    for (int i = (int)a.size() - 1; i >= 0; i--) {
        comps++;
        if (a[i] == key) return i;
    }
    return -1;
}

// Ikili arama (dizi sirali olmali)
static int binarySearchIter(const vector<int>& aSorted, int key, long long& comps) {
    comps = 0;
    int left = 0, right = (int)aSorted.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;

        comps++; // == karsilastirmasi
        if (aSorted[mid] == key) return mid;

        comps++; // < karsilastirmasi
        if (key < aSorted[mid]) right = mid - 1;
        else left = mid + 1;
    }
    return -1;
}

void moduleSearching() {
    cout << "\n--- 3) Arama Algoritmalari ---\n";
    cout << "Ayni dizi ve ayni hedef eleman uzerinde 3 arama yontemi karsilastirilir.\n";
    cout << "Kiyaslama metrigi: KARSILASTIRMA SAYISI\n\n";

    int n = readInt("Dizi boyutu N (10 - 100000 arasi onerilir): ", 1, 200000);

    int keyMode = readInt(
        "Hedef eleman secimi:\n"
        "1 - Diziden sec (kesin bulunur)\n"
        "2 - Rastgele sayi (bulunmayabilir)\n"
        "Secim: ", 1, 2
    );

    vector<int> a = makeRandomArray(n, -1000, 1000, 42);

    int key;
    if (keyMode == 1) {
        key = a[rand() % n];
        cout << "Hedef (key): " << key << "  [Diziden secildi, kesin bulunur]\n";
    } else {
        key = (rand() % 4001) - 2000;
        cout << "Hedef (key): " << key << "  [Rastgele, bulunmayabilir]\n";
    }

    cout << "Dizi ornegi (ilk 10): ";
    printVectorFirst(a, 10);
    cout << "\n";

    vector<int> aSorted = a;
    sort(aSorted.begin(), aSorted.end());

    long long compsF = 0, compsB = 0, compsBin = 0;
    int idxF = linearSearchForward(a, key, compsF);
    int idxB = linearSearchBackward(a, key, compsB);
    int idxBin = binarySearchIter(aSorted, key, compsBin);

    cout << "\nSonuclar (tek kosu):\n";
    cout << left << setw(22) << "Algoritma"
         << setw(12) << "Index"
         << "Karsilastirma\n";
    cout << string(50, '-') << "\n";

    cout << left << setw(22) << "Ileri Lineer"
         << setw(12) << idxF
         << compsF << "\n";

    cout << left << setw(22) << "Geri Lineer"
         << setw(12) << idxB
         << compsB << "\n";

    cout << left << setw(22) << "Ikili Arama"
         << setw(12) << idxBin
         << compsBin << "\n";

    cout << "\nNot:\n";
    cout << "- Ikili arama sirali dizi uzerinde yapilir (sort uygulanmistir).\n";
    cout << "- Ikili aramadaki index, sirali diziye (aSorted) gore index'tir.\n";
    cout << "- Karsilastirma sayisi teorik karmasikligi yansitir.\n";
}
