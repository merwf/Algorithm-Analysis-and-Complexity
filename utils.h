#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <functional>

void clearInput();
int readInt(const std::string& prompt, int minVal, int maxVal);

std::vector<int> makeRandomArray(int n, int lo, int hi, unsigned seed = 42);
void printVectorFirst(const std::vector<int>& a, int k = 10);
void printVector(const std::vector<int>& a);

// Ölçüm (MaxSubsequence için kullanýyorsun)
long long measureUsRepeat(const std::function<int()>& job, int repeat, int& lastResult);

#endif
