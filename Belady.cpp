#include <iostream>
#include <vector>

#include "Cache.hpp"

int slowGetPage(int key) { return 0; }

int main() {
    int cap = 0;
    int queriesCount = 0;

    std::cin >> cap >> queriesCount;
    if (queriesCount <= 0) {
        std::cout << 0 << '\n';
        return 0;
    }

    std::vector<int> queries(queriesCount);
    for (int i = 0; i < queriesCount; i++) {
        std::cin >> queries[i];
    }

    cache::BeladyCache<int, int> beladyCache(cap, queries.begin(), queries.end());
    int hits = countCacheHits(beladyCache, queries, slowGetPage);

    std::cout << hits << '\n';
}