#include <iostream>
#include <vector>

#include "Cache.hpp"

int slowGetPage(int key) { return 0; }

int main() {
    int cacheCapacity = 0;
    std::cout << "Enter cache capacity : ";
    std::cin >> cacheCapacity;
    std::cout << "Enter queries (Ctrl+D to end): \n";

    if (cacheCapacity < 0) {
        std::cout << "Cache capacity should be > 0";
        return 1;
    }

    std::vector<int> queries;
    int q = 0;
    while (std::cin >> q) queries.push_back(q);

    cache::LFUCache<int> LFUCache(cacheCapacity);
    cache::IdealCache<int> idealCache(cacheCapacity, queries);

    int idealHits = countCacheHits(idealCache, queries, slowGetPage);
    int LFUHits   = countCacheHits(LFUCache, queries, slowGetPage);

    std::cout << "Count of       queries    : " << queries.size() << "\n";
    std::cout << "Count of       cache hits : " << LFUHits << "\n";
    std::cout << "Count of ideal cache hits : " << idealHits << "\n";
}