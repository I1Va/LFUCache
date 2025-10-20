#include <iostream>
#include <vector>

#include "Cache.hpp"
#include "Utilities.hpp"

int slowGetPage(int key) { return 0; }

int main() {
    int cap = 0;
    int queriesCount = 0;

    std::cin >> cap >> queriesCount;
    if (ut::logIfstreamError(std::cin, std::cerr)) return 1; 

    if (queriesCount <= 0) {
        std::cout << 0 << '\n';
        return 0;
    }

    std::vector<int> queries(queriesCount);
    for (int i = 0; i < queriesCount; i++) {
        std::cin >> queries[i];
        if (ut::logIfstreamError(std::cin, std::cerr)) return 1; 
    }

    cache::LFUCache<int> LFUCache(cap);
    int hits = countCacheHits(LFUCache, queries, slowGetPage);

    std::cout << hits << '\n';
}