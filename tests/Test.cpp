#include <gtest/gtest.h>

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>
#include <vector>

#include "Generator.hpp"
#include "IdealCache.hpp"
#include "TestCache.hpp"
#include "LFUCache.hpp"

template <typename T>
struct isCacheType : std::false_type {};

template <typename DataT>
struct isCacheType<cache::LFUCache<DataT>> : std::true_type {};

template <typename DataT>
struct isCacheType<test::IdealCache<DataT>> : std::true_type {};

template <typename T>
concept CacheType = isCacheType<T>::value;

template <CacheType CacheT, typename F>
int countCacheHits(CacheT &cache, const std::vector<int> &queries, F slowGetPage) {
    int result = 0;
    for (int q : queries)
        result += cache.lookupUpdate(q, slowGetPage);
    return result;
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}



TEST(Auto, IdealCacheComprasion) {
    const size_t QUERIES_COUNT = 10000; 
    const size_t CACHE_CAPACITY = 1000;

    std::vector<int> queries = test::randomIntVector(QUERIES_COUNT, -1000, 1000);

    cache::LFUCache<test::Page> LFUcache(CACHE_CAPACITY);
    test::IdealCache<test::Page> idealCache(CACHE_CAPACITY, queries);
    
    int LFUHits = countCacheHits(LFUcache, queries, test::slowGetPage);
    int idealHits = countCacheHits(idealCache, queries, test::slowGetPage);
   

    std::cout << "queries count : " << QUERIES_COUNT << "\n";
    std::cout << "LFU / ideal hits: " << LFUHits << " / " << idealHits << "\n";
}
