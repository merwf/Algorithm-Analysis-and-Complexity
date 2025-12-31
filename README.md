# Algorithm Analysis and Complexity

This repository contains a comprehensive C++ project developed for the
**Algorithm Analysis and Complexity** course. The project focuses on the
implementation, analysis, and performance evaluation of fundamental
algorithms using theoretical complexity and practical time measurements.

---

## Topics Covered

### 1. Maximum Subsequence Problem
- Brute-force and optimized approaches
- Time complexity comparison

### 2. Recursive Algorithms
- Basic recursive problem solving
- Cost analysis of recursion

### 3. Searching Algorithms
- Linear Search (forward / backward)
- Binary Search
- Performance comparison

### 4. Sorting Algorithms
- Bubble Sort
- Selection Sort
- Insertion Sort
- Merge Sort
- Quick Sort (First Pivot & Median-of-Three)
- Heap Sort
- Counting Sort
- Radix Sort
- External Sort (simulation)

### 5. Heap Structures
- Binary Heap (Min-Heap / Max-Heap)
- InsertKey, DeleteMin (Heapify)
- BuildHeap
- IncreaseKey / DecreaseKey
- DeleteKey
- Heap Merge

### 6. Greedy Algorithms
- Activity Scheduling Problem
- Job Scheduling Problem
- Huffman Coding (Encoding & Decoding)

### 7. Dynamic Programming
- Bottom-Up DP (Fibonacci with table)
- Minimum Cost Path (Top-Down with Memoization)
- 0/1 Knapsack Problem (DP Table)

> All dynamic programming problems include explicit DP table visualization.

---

## Performance Measurement

- Execution times are measured using the C++ `chrono` library
- Average execution time is reported in nanoseconds (ns) and microseconds (µs)
- Algorithms are compared in terms of:
  - Time complexity
  - Practical execution performance

---

## Project Structure

├── main.cpp
├── utils.h / utils.cpp
├── searching_module.h / searching_module.cpp
├── sorting_module.h / sorting_module.cpp
├── heap_module.h / heap_module.cpp
├── greedy_module.h / greedy_module.cpp
├── dp_module.h / dp_module.cpp
├── max_subsequence.h / max_subsequence.cpp
├── recursive_module.h / recursive_module.cpp


## How to Run

1. Compile the project using a C++ compiler (C++17 recommended)
2. Run the executable
3. Use the menu to select algorithm categories and experiments
