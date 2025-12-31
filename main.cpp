#include <iostream>

#include "max_subsequence.h"
#include "recursive_module.h"
#include "searching_module.h"
#include "sorting_module.h"
#include "heap_module.h"
#include "utils.h"
#include "greedy_module.h"
#include "dp_module.h"

using namespace std;

static void printMenu() {
    cout << "==============================\n";
    cout << "Algoritma Analizi Proje Menusu\n";
    cout << "==============================\n";
    cout << "1 - Maximum Subsequence Problem\n";
    cout << "2 - Rekursif Algoritmalar\n";
    cout << "3 - Arama Algoritmalari\n";
    cout << "4 - Siralama Algoritmalari\n";
    cout << "5 - Heap / Oncelikli Kuyruklar\n";
    cout << "6 - Greedy Algoritmalar\n";
    cout << "7 - Dinamik Programlama\n";
    cout << "0 - Cikis\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    while (true) {
        printMenu();

        int choice = readInt("Secim: ", 0, 7);
        switch (choice) {
            case 1: moduleMaxSubsequence(); break;
            case 2: moduleRecursive(); break;
            case 3: moduleSearching(); break;
            case 4: moduleSorting(); break;
            case 5: moduleHeap(); break;
            case 6: moduleGreedy(); break;
            case 7: moduleDP(); break;
            case 0:
                cout << "Cikis yapiliyor...\n";
                return 0;
            default:
                cout << "Gecersiz secim.\n";
                break;
        }
    }
}
