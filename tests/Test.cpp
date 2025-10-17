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


// AI GENERATED TESTS:
TEST(LFU, BasicHit1) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page> lfu(CACHE_CAPACITY);

    // sequence: 1 (miss), 2 (miss), 1 (hit)
    std::vector<int> seq = {1, 2, 1};
    int hits = 0;
    for (int q : seq) hits += lfu.lookupUpdate(q, test::slowGetPage);

    EXPECT_EQ(hits, 1); // only the third access is a hit
}

TEST(LFU, EvictLeastFrequent2) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page> lfu(CACHE_CAPACITY);

    // Access pattern:
    // 1 (miss), 2 (miss), 1 (hit -> freq(1)=2, freq(2)=1), 3 (miss -> should evict key 2)
    std::vector<int> seq = {1, 2, 1, 3};
    for (int q : seq) lfu.lookupUpdate(q, test::slowGetPage);

    // After sequence: cache should contain keys {1,3}, key 1 still resident -> hit
    EXPECT_TRUE(lfu.lookupUpdate(1, test::slowGetPage));
    // key 2 was least-frequently used and should have been evicted -> miss
    EXPECT_FALSE(lfu.lookupUpdate(2, test::slowGetPage));
}

TEST(LFU, FrequencyDominance) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page> lfu(CACHE_CAPACITY);

    // Make key 2 dominant in frequency so key 1 gets evicted when 3 inserted.
    // Sequence: 1(miss),2(miss),2(hit),3(miss -> evict 1),2(hit)
    std::vector<int> seq = {1, 2, 2, 3, 2};
    int hits = 0;
    for (int q : seq) hits += lfu.lookupUpdate(q, test::slowGetPage);

    // After sequence, key 2 should be resident (many hits), key 1 evicted.
    EXPECT_TRUE(lfu.lookupUpdate(2, test::slowGetPage));
    EXPECT_FALSE(lfu.lookupUpdate(1, test::slowGetPage));
}

// --- Ideal cache tests ------------------------------------------------------

TEST(Ideal, BasicHit) {
    const size_t CACHE_CAPACITY = 2;
    // queries passed to constructor so IdealCache knows future accesses
    std::vector<int> queries = {1, 2, 1};
    test::IdealCache<test::Page> ideal(CACHE_CAPACITY, queries);

    int hits = 0;
    for (int q : queries) hits += ideal.lookupUpdate(q, test::slowGetPage);

    EXPECT_EQ(hits, 1); // same pattern: only the third access is a hit
}

TEST(Ideal, SequentialUniqueCapacityOne) {
    const size_t CACHE_CAPACITY = 1;
    // unique sequential accesses -> no hits for capacity 1
    std::vector<int> queries = {1, 2, 3};
    test::IdealCache<test::Page> ideal(CACHE_CAPACITY, queries);

    int hits = 0;
    for (int q : queries) hits += ideal.lookupUpdate(q, test::slowGetPage);

    EXPECT_EQ(hits, 0);
}

TEST(Ideal, RepeatedPatternHighHitRate2) {
    const size_t CACHE_CAPACITY = 2;
    // repeated alternation: 1,2,1,2,1,2 -> after warmup should hit frequently
    std::vector<int> queries = {1, 2, 1, 2, 1, 2};
    test::IdealCache<test::Page> ideal(CACHE_CAPACITY, queries);

    int hits = 0;
    for (int q : queries) hits += ideal.lookupUpdate(q, test::slowGetPage);

    // First two accesses are misses; remaining 4 are hits
    EXPECT_EQ(hits, 4);
}

// Test 2: Uniform random workload
TEST(Compare, UniformRandom) {
    const size_t QUERIES_COUNT = 10000;
    const size_t CACHE_CAPACITY = 1000;

    // use project generator for reproducibility if available
    std::vector<int> queries = test::randomIntVector(QUERIES_COUNT, -1000, 1000);

    cache::LFUCache<test::Page> lfu(CACHE_CAPACITY);
    test::IdealCache<test::Page> ideal(CACHE_CAPACITY, queries);

    int lfu_hits   = countCacheHits(lfu, queries, test::slowGetPage);
    int ideal_hits = countCacheHits(ideal, queries, test::slowGetPage);

    double lfu_pct = 100.0 * lfu_hits / (double)QUERIES_COUNT;
    double ideal_pct = 100.0 * ideal_hits / (double)QUERIES_COUNT;

    std::cout << "[UniformRandom] LFU_hits=" << lfu_hits << " (" << lfu_pct << "%)"
              << " Ideal_hits=" << ideal_hits << " (" << ideal_pct << "%)\n";

    EXPECT_LE(lfu_hits, ideal_hits);
}

TEST(LFU, BasicHit) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page> lfu(CACHE_CAPACITY);

    std::vector<int> queries = {1, 2, 1}; // miss, miss, hit
    int hits = countCacheHits(lfu, queries, test::slowGetPage);

    EXPECT_EQ(hits, 1);
}

TEST(LFU, EvictLeastFrequent) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page> lfu(CACHE_CAPACITY);

    // 1 (miss), 2 (miss), 1 (hit), 3 (miss -> should evict key 2)
    std::vector<int> queries = {1, 2, 1, 3};
    int hits = countCacheHits(lfu, queries, test::slowGetPage);

    EXPECT_EQ(hits, 1); // only the 3rd access (second '1') is a hit in this sequence
    // additional sanity: 1 should still be a hit, 2 should be miss
    EXPECT_TRUE(lfu.lookupUpdate(1, test::slowGetPage));
    EXPECT_FALSE(lfu.lookupUpdate(2, test::slowGetPage));
}

TEST(LFU, RepeatedPattern) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page> lfu(CACHE_CAPACITY);

    // pattern alternates 1 and 2 -> after warmup most accesses hit
    std::vector<int> queries = {1,2,1,2,1,2};
    int hits = countCacheHits(lfu, queries, test::slowGetPage);

    // first two accesses are misses, remaining four are hits
    EXPECT_EQ(hits, 4);
}

// ---------------- Ideal tests ----------------

TEST(Ideal, BasicHit2) {
    const size_t CACHE_CAPACITY = 2;
    std::vector<int> queries = {1, 2, 1};
    test::IdealCache<test::Page> ideal(CACHE_CAPACITY, queries);

    int hits = countCacheHits(ideal, queries, test::slowGetPage);
    EXPECT_EQ(hits, 1);
}

TEST(Ideal, EvictByBelady1) {
    const size_t CACHE_CAPACITY = 2;
    std::vector<int> queries = {1, 2, 1, 3};
    test::IdealCache<test::Page> ideal(CACHE_CAPACITY, queries);

    int hits = countCacheHits(ideal, queries, test::slowGetPage);
    EXPECT_EQ(hits, 1);
}

TEST(Ideal, RepeatedPatternHighHitRate1) {
    const size_t CACHE_CAPACITY = 2;
    std::vector<int> queries = {1,2,1,2,1,2};
    test::IdealCache<test::Page> ideal(CACHE_CAPACITY, queries);

    int hits = countCacheHits(ideal, queries, test::slowGetPage);
    EXPECT_EQ(hits, 4);
}


// ---------------- Deterministic query generators ----------------
std::vector<int> uniformQueries(int n, int minV, int maxV, unsigned seed = 42) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dist(minV, maxV);
    std::vector<int> q(n);
    for (int &x : q) x = dist(gen);
    return q;
}

std::vector<int> skewedQueries(int n, int hotKey, double hotFrac = 0.7) {
    std::vector<int> q(n);
    int hotCount = static_cast<int>(n * hotFrac);
    for (int i = 0; i < hotCount; ++i) q[i] = hotKey;
    for (int i = hotCount; i < n; ++i) q[i] = i; // cold keys
    std::shuffle(q.begin(), q.end(), std::mt19937(12345));
    return q;
}

TEST(Compare, SmallCache) {
    const int N = 200;
    const int CACHE_CAP = 5;

    std::vector<int> queries = {1,2,3,1,2,3,1,2,3,4,5,6};

    cache::LFUCache<test::Page> lfu(CACHE_CAP);
    test::IdealCache<test::Page> ideal(CACHE_CAP, queries);

    int lfu_hits   = countCacheHits(lfu, queries, test::slowGetPage);
    int ideal_hits = countCacheHits(ideal, queries, test::slowGetPage);

    EXPECT_LE(lfu_hits, ideal_hits);
    EXPECT_GE(lfu_hits, 2); // should get at least some hits
}

// ---------------- LFU-only tests ----------------
TEST(LFU, RepeatedPattern1) {
    cache::LFUCache<test::Page> lfu(2);
    std::vector<int> queries = {1,2,1,2,1,2};
    int hits = countCacheHits(lfu, queries, test::slowGetPage);
    EXPECT_EQ(hits, 4);
}

TEST(LFU, EvictHotCold) {
    cache::LFUCache<test::Page> lfu(2);
    std::vector<int> queries = {1,2,1,3,1,3,1};
    int hits = countCacheHits(lfu, queries, test::slowGetPage);
    EXPECT_EQ(hits, 4); // hits on repeated 1s and 3s
}


TEST(Ideal, EvictByBelady) {
    std::vector<int> queries = {1,2,1,3};
    test::IdealCache<test::Page> ideal(2, queries);
    int hits = countCacheHits(ideal, queries, test::slowGetPage);
    EXPECT_EQ(hits, 1); // only second 1 is a hit
}
