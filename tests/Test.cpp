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


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Auto, IdealCacheComprasion) {
    const size_t QUERIES_COUNT = 10000; 
    const size_t CACHE_CAPACITY = 1000;

    std::vector<int> queries = test::randomIntVector(QUERIES_COUNT, -1000, 1000);

    // cache::Cache<test::Page> cache(CACHE_CAPACITY);
    test::IdealCache<test::Page> idealCache(CACHE_CAPACITY, queries);
    
    int idealHits = 0;

    for (auto key : queries) {
        idealHits += idealCache.lookupUpdate(key, test::slowGetPage);
    }


    std::cout << "ideal hits: " << idealHits << " / " << QUERIES_COUNT << "\n";
}
