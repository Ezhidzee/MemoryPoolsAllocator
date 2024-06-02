#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

template<typename T>
class MemoryPoolsAllocator {
public:
    typedef T value_type;

    typedef T *pointer;

    typedef typename std::pointer_traits<pointer>::template rebind<const value_type> const_pointer;

    typedef typename std::pointer_traits<pointer>::template rebind<void> void_pointer;

    typedef typename std::pointer_traits<pointer>::template rebind<const void> const_void_pointer;

    typedef typename std::pointer_traits<pointer>::difference_type difference_type;

    typedef typename std::make_unsigned<difference_type>::type size_type;
private:
    struct pool {
        size_type size;

        size_type chunksSize;

        size_type chunksCount;

        std::vector<void *> freeChunks;

        std::vector<void *> filledChunks;

        pool(size_type _size, size_type _chunksSize) {
            size = _size;
            chunksSize = _chunksSize;
            chunksCount = size / chunksSize;

            for (size_type i = 0; i < chunksCount; ++i) freeChunks.push_back(malloc(chunksSize));
        }

        pointer allocate(size_type n) {
            void *chunk = freeChunks.front();
            filledChunks.push_back(chunk);
            freeChunks.erase(freeChunks.begin());
            return reinterpret_cast<pointer>(chunk);
        }

        ~pool() {
            for (size_type i = 0; i < freeChunks.size(); ++i) free(freeChunks[i]);
            for (size_type i = 0; i < filledChunks.size(); ++i) free(filledChunks[i]);
        }
    };

    std::vector<pool *> pools;

public:
    MemoryPoolsAllocator(size_type poolsCount, std::initializer_list<size_type> poolsSizes,
                         std::initializer_list<size_type> chunksSizes) {
        if (poolsSizes.size() != poolsCount || chunksSizes.size() != poolsCount)
            throw std::length_error("Invalid number of values passed");

        for (size_type i = 0; i < poolsCount; ++i) {
            pool *pl = new pool(*(poolsSizes.begin() + i), *(chunksSizes.begin() + i));
            pools.push_back(pl);
        }

        std::sort(pools.begin(), pools.end(), [](pool *&a, pool *&b) { return a->chunksSize < b->chunksSize; });
    }

    MemoryPoolsAllocator(const MemoryPoolsAllocator &alloc) {
        for (size_type i = 0; i < alloc.pools.size(); ++i) pools.push_back(alloc.pools[i]);
    }

    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        for (size_type i = 0; i < pools.size(); ++i) {
            if (pools[i]->chunksSize >= n * sizeof(value_type) && !pools[i]->freeChunks.empty())
                return pools[i]->allocate(n * sizeof(value_type));
        }
        throw std::bad_alloc();
    }

    void deallocate(pointer p, size_type n) {
        for (size_type i = 0; i < pools.size(); ++i) {
            for (size_type j = 0; j < pools[i]->filledChunks.size(); ++j) {
                if (reinterpret_cast<pointer>(pools[i]->filledChunks[j]) == p) {
                    pools[i]->freeChunks.push_back(pools[i]->filledChunks[j]);
                    pools[i]->filledChunks.erase(pools[i]->filledChunks.begin() + j);
                    break;
                }
            }
        }
    }

    bool operator==(const MemoryPoolsAllocator<value_type> &alloc) {
        if (pools.size() != alloc.pools.size()) return false;
        for (size_type i = 0; i < pools.size(); ++i) {
            if (pools[i]->freeChunks.size() != alloc.pools[i]->freeChunks.size() ||
                pools[i]->size != alloc.pools[i]->size || pools[i]->chunksSize != alloc.pools[i]->chunksSize)
                return false;
            for (size_type j = 0; j < pools[i]->freeChunks.size(); ++j)
                if (pools[i]->freeChunks[j] != alloc.pools[i]->freeChunks[j]) return false;
            if (pools[i]->filledChunks.size() != alloc.pools[i]->filledChunks.size()) return false;
            for (size_type j = 0; j < pools[i]->filledChunks.size(); ++j)
                if (pools[i]->filledChunks[j] != alloc.pools[i]->filledChunks[j]) return false;
        }
        return true;
    }

    bool operator!=(const MemoryPoolsAllocator<value_type> &alloc) {
        return !(*this == alloc);
    }
};