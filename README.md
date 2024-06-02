# MemoryPoolsAllocator

An allocator operating on the basis of pre-allocated memory blocks that meets [requirements for allocators](https://en.cppreference.com/w/cpp/named_req/Allocator).

## Comparison with the standard allocator when interacting with containers

MemoryPoolsAllocator

**Averaged data from the results of many measurements was taken**

### Time spent adding an element to a vector, depending on the number of added elements

```c++
    std::ofstream timeStamps("data.csv");
    std::vector<int> vec;
    for (int i = 0; i < 50; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        vec.push_back(i);
        auto end = std::chrono::high_resolution_clock::now();
        timeStamps << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << ',';
    }
    timeStamps << '\n';
    MemoryPoolsAllocator<int> alloc(2, {800, 400}, {400, 200});
    std::vector<int, MemoryPoolsAllocator<int>> vec1(alloc);
    for (int i = 0; i < 50; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        vec1.push_back(i);
        auto end = std::chrono::high_resolution_clock::now();
        timeStamps << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << ',';
    }
```
![data1](https://ezhidze.su/wp-content/uploads/2023/04/data1.png)

### Time spent removing an element from a vector

```c++
    std::ofstream timeStamps("data.csv");
    std::vector<int> vec;
    for (int i = 0; i < 50; ++i) vec.push_back(i);
    for (int i = 0; i < 50; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        vec.erase(vec.begin());
        auto end = std::chrono::high_resolution_clock::now();
        timeStamps << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << ',';
    }
    timeStamps << '\n';
    MemoryPoolsAllocator<int> alloc(2, {800, 400}, {400, 200});
    std::vector<int, MemoryPoolsAllocator<int>> vec1(alloc);
    for (int i = 0; i < 50; ++i) vec1.push_back(i);
    for (int i = 0; i < 50; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        vec1.erase(vec1.begin());
        auto end = std::chrono::high_resolution_clock::now();
        timeStamps << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << ',';
    }
```

![data2](https://ezhidze.su/wp-content/uploads/2023/04/data2.png)
