#include <gtest/gtest.h>

#include <unordered_map>
#include <unordered_set>
#include <queue>

#include "Generator.hpp"
#include "IdealCache.hpp"
#include "TestCache.hpp"
#include "LIRSCache.hpp"


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}


static int beladyMIN(const std::vector<int>& queries, int cacheCapacity) {
    int N = queries.size();
    if (cacheCapacity <= 0) return 0;

    // 1. Build future positions for each key
    std::unordered_map<int, std::queue<int>> futurePos;
    for (int i = 0; i < N; ++i) {
        futurePos[queries[i]].push(i);
    }

    using KeyNext = std::pair<int, int>; // (next_use_index, key)
    auto cmp = [](const KeyNext& a, const KeyNext& b) { return a.first < b.first; };
    std::priority_queue<KeyNext, std::vector<KeyNext>, decltype(cmp)> pq(cmp);

    std::unordered_set<int> cache;
    int hits = 0;

    for (int i = 0; i < N; ++i) {
        int key = queries[i];
        futurePos[key].pop(); // consume current position

        if (cache.count(key)) {
            ++hits; // hit
        } else {
            if ((int)cache.size() == cacheCapacity) {
                // Evict key with farthest next use
                while (!pq.empty()) {
                    auto [nextUse, evictKey] = pq.top();
                    pq.pop();
                    // Check if key is still in cache and nextUse is correct
                    int actualNext = futurePos[evictKey].empty() ? std::numeric_limits<int>::max() : futurePos[evictKey].front();
                    if (cache.count(evictKey) && actualNext == nextUse) {
                        cache.erase(evictKey);
                        break;
                    }
                }
            }
            cache.insert(key);
        }

        // Push next use of current key
        int nextUse = futurePos[key].empty() ? std::numeric_limits<int>::max() : futurePos[key].front();
        pq.push({nextUse, key});
    }

    return hits;
}

TEST(Auto, IdealCacheComprasion) {
    const size_t QUERIES_COUNT = 1000; 
    const size_t CACHE_CAPACITY = 100;

    std::vector<int> queries = test::randomIntVector(QUERIES_COUNT, -100, 100);
    int beladyHits= beladyMIN(queries, CACHE_CAPACITY);


    cache::Cache<test::Page> cache(CACHE_CAPACITY);
    test::IdealCache<test::Page> idealCache(CACHE_CAPACITY, queries);
    
    int idealHits = 0;

    for (auto key : queries) {
        idealHits += idealCache.lookupUpdate(key, test::slowGetPage);
    }


    std::cout << "ideal hits: " << idealHits << " / " << QUERIES_COUNT << "\n";
    std::cout << "true : " << beladyHits << "\n";
}

// TEST(Auto, Add1000) {
//     Cache::Cache cache;
    
//     for (int i = 0; i < 1000; i++) {
//         int a = gen::randomInt(-1000, 1000);
//         int b = gen::randomInt(-1000, 1000);
        
//         SCOPED_TRACE("case #" + std::to_string(i) +
//                  " a=" + std::to_string(a) +
//                  " b=" + std::to_string(b));

//         EXPECT_EQ(a + b, cache.add(a, b));
//     }
// }
