#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>

#include "MemoryPoolsAllocator.hpp"

int main() {
    MemoryPoolsAllocator<int> alloc(2, {800, 400}, {400, 200});
    std::vector<int, MemoryPoolsAllocator<int>> vec1(alloc);
    for (int i = 0; i < 50; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        vec1.push_back(i);
        auto end = std::chrono::high_resolution_clock::now();
    }
}