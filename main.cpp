#include <iostream>

#include "LFUCache.hpp"

int slowGetPage(int key) {
    std::cout << "cache miss!\n";
    return 0;
}

int main() {
    int cacheCapacity = 0;
    std::cout << "Enter cache capacity : ";
    std::cin >> cacheCapacity;
    std::cout << "Enter queries (Ctrl+D to end): \n";

    if (cacheCapacity < 0) {
        std::cout << "cacheCapacity should be > 0";
        return 1;
    }

    cache::LFUCache<int> LFUCache(cacheCapacity);
    
    int q = 0;
    int hits = 0;
    int queries = 0;
    while (std::cin >> q) {
        queries++;
        hits += LFUCache.lookupUpdate(q, slowGetPage);
    }

    std::cout << "Count of queries    : " << queries << "\n";
    std::cout << "Count of cache hits : " << hits << "\n";
}