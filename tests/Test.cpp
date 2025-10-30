#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Cache.hpp"
#include "Generator.hpp"
#include "TestCache.hpp"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Auto, BeladyCacheComprasion) {
    const size_t QUERIES_COUNT = 10000;
    const size_t CACHE_CAPACITY = 1000;

    std::vector<int> queries =
        test::randomIntVector(QUERIES_COUNT, -1000, 1000);

    cache::LFUCache<test::Page, int> LFUcache(CACHE_CAPACITY);
    cache::BeladyCache<test::Page, int> BeladyCache(
        CACHE_CAPACITY, queries.begin(), queries.end());

    int LFUHits = countCacheHits(LFUcache, queries.begin(), queries.end(),
                                 test::slowGetPage);
    int BeladyHits = countCacheHits(BeladyCache, queries.begin(), queries.end(),
                                    test::slowGetPage);

    std::cout << "queries count : " << QUERIES_COUNT << '\n';
    std::cout << "LFU / Belady hits: " << LFUHits << " / " << BeladyHits
              << '\n';
}

// AI GENERATED TESTS:
TEST(LFU, BasicHit1) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page, int> lfu(CACHE_CAPACITY);

    // sequence: 1 (miss), 2 (miss), 1 (hit)
    std::vector<int> seq = {1, 2, 1};
    int hits = 0;
    for (int q : seq)
        hits += lfu.lookupUpdate(q, test::slowGetPage);

    EXPECT_EQ(hits, 1); // only the third access is a hit
}

TEST(LFU, EvictLeastFrequent2) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page, int> lfu(CACHE_CAPACITY);

    // Access pattern:
    // 1 (miss), 2 (miss), 1 (hit -> freq(1)=2, freq(2)=1), 3 (miss -> should
    // evict key 2)
    std::vector<int> seq = {1, 2, 1, 3};
    for (int q : seq)
        lfu.lookupUpdate(q, test::slowGetPage);

    // After sequence: cache should contain keys {1,3}, key 1 still resident ->
    // hit
    EXPECT_TRUE(lfu.lookupUpdate(1, test::slowGetPage));
    // key 2 was least-frequently used and should have been evicted -> miss
    EXPECT_FALSE(lfu.lookupUpdate(2, test::slowGetPage));
}

TEST(LFU, FrequencyDominance) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page, int> lfu(CACHE_CAPACITY);

    // Make key 2 dominant in frequency so key 1 gets evicted when 3 inserted.
    // Sequence: 1(miss),2(miss),2(hit),3(miss -> evict 1),2(hit)
    std::vector<int> seq = {1, 2, 2, 3, 2};
    int hits = 0;
    for (int q : seq)
        hits += lfu.lookupUpdate(q, test::slowGetPage);

    // After sequence, key 2 should be resident (many hits), key 1 evicted.
    EXPECT_TRUE(lfu.lookupUpdate(2, test::slowGetPage));
    EXPECT_FALSE(lfu.lookupUpdate(1, test::slowGetPage));
}

// --- Belady cache tests ------------------------------------------------------

TEST(Belady, BasicHit) {
    const size_t CACHE_CAPACITY = 2;
    // queries passed to constructor so BeladyCache knows future accesses
    std::vector<int> queries = {1, 2, 1};
    cache::BeladyCache<test::Page, int> Belady(CACHE_CAPACITY, queries.begin(),
                                               queries.end());

    int hits = 0;
    for (int q : queries)
        hits += Belady.lookupUpdate(q, test::slowGetPage);

    EXPECT_EQ(hits, 1); // same pattern: only the third access is a hit
}

TEST(Belady, SequentialUniqueCapacityOne) {
    const size_t CACHE_CAPACITY = 1;
    // unique sequential accesses -> no hits for capacity 1
    std::vector<int> queries = {1, 2, 3};
    cache::BeladyCache<test::Page, int> Belady(CACHE_CAPACITY, queries.begin(),
                                               queries.end());

    int hits = 0;
    for (int q : queries)
        hits += Belady.lookupUpdate(q, test::slowGetPage);

    EXPECT_EQ(hits, 0);
}

TEST(Belady, RepeatedPatternHighHitRate2) {
    const size_t CACHE_CAPACITY = 2;
    // repeated alternation: 1,2,1,2,1,2 -> after warmup should hit frequently
    std::vector<int> queries = {1, 2, 1, 2, 1, 2};
    cache::BeladyCache<test::Page, int> Belady(CACHE_CAPACITY, queries.begin(),
                                               queries.end());

    int hits = 0;
    for (int q : queries)
        hits += Belady.lookupUpdate(q, test::slowGetPage);

    // First two accesses are misses; remaining 4 are hits
    EXPECT_EQ(hits, 4);
}

// Test 2: Uniform random workload
TEST(Compare, UniformRandom) {
    const size_t QUERIES_COUNT = 10000;
    const size_t CACHE_CAPACITY = 1000;

    // use project generator for reproducibility if available
    std::vector<int> queries =
        test::randomIntVector(QUERIES_COUNT, -1000, 1000);

    cache::LFUCache<test::Page, int> lfu(CACHE_CAPACITY);
    cache::BeladyCache<test::Page, int> Belady(CACHE_CAPACITY, queries.begin(),
                                               queries.end());

    int lfu_hits =
        countCacheHits(lfu, queries.begin(), queries.end(), test::slowGetPage);
    int Belady_hits = countCacheHits(Belady, queries.begin(), queries.end(),
                                     test::slowGetPage);

    double lfu_pct = 100.0 * lfu_hits / (double)QUERIES_COUNT;
    double Belady_pct = 100.0 * Belady_hits / (double)QUERIES_COUNT;

    std::cout << "[UniformRandom] LFU_hits=" << lfu_hits << " (" << lfu_pct
              << "%)"
              << " Belady_hits=" << Belady_hits << " (" << Belady_pct << "%)\n";

    EXPECT_LE(lfu_hits, Belady_hits);
}

TEST(LFU, BasicHit) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page, int> lfu(CACHE_CAPACITY);

    std::vector<int> queries = {1, 2, 1}; // miss, miss, hit
    int hits =
        countCacheHits(lfu, queries.begin(), queries.end(), test::slowGetPage);

    EXPECT_EQ(hits, 1);
}

TEST(LFU, EvictLeastFrequent) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page, int> lfu(CACHE_CAPACITY);

    // 1 (miss), 2 (miss), 1 (hit), 3 (miss -> should evict key 2)
    std::vector<int> queries = {1, 2, 1, 3};
    int hits =
        countCacheHits(lfu, queries.begin(), queries.end(), test::slowGetPage);

    EXPECT_EQ(hits,
              1); // only the 3rd access (second '1') is a hit in this sequence
    // additional sanity: 1 should still be a hit, 2 should be miss
    EXPECT_TRUE(lfu.lookupUpdate(1, test::slowGetPage));
    EXPECT_FALSE(lfu.lookupUpdate(2, test::slowGetPage));
}

TEST(LFU, RepeatedPattern) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page, int> lfu(CACHE_CAPACITY);

    // pattern alternates 1 and 2 -> after warmup most accesses hit
    std::vector<int> queries = {1, 2, 1, 2, 1, 2};
    int hits =
        countCacheHits(lfu, queries.begin(), queries.end(), test::slowGetPage);

    // first two accesses are misses, remaining four are hits
    EXPECT_EQ(hits, 4);
}

// ---------------- Belady tests ----------------

TEST(Belady, BasicHit2) {
    const size_t CACHE_CAPACITY = 2;
    std::vector<int> queries = {1, 2, 1};
    cache::BeladyCache<test::Page, int> Belady(CACHE_CAPACITY, queries.begin(),
                                               queries.end());

    int hits = countCacheHits(Belady, queries.begin(), queries.end(),
                              test::slowGetPage);
    EXPECT_EQ(hits, 1);
}

TEST(Belady, EvictByBelady1) {
    const size_t CACHE_CAPACITY = 2;
    std::vector<int> queries = {1, 2, 1, 3};
    cache::BeladyCache<test::Page, int> Belady(CACHE_CAPACITY, queries.begin(),
                                               queries.end());

    int hits = countCacheHits(Belady, queries.begin(), queries.end(),
                              test::slowGetPage);
    EXPECT_EQ(hits, 1);
}

TEST(Belady, RepeatedPatternHighHitRate1) {
    const size_t CACHE_CAPACITY = 2;
    std::vector<int> queries = {1, 2, 1, 2, 1, 2};
    cache::BeladyCache<test::Page, int> Belady(CACHE_CAPACITY, queries.begin(),
                                               queries.end());

    int hits = countCacheHits(Belady, queries.begin(), queries.end(),
                              test::slowGetPage);
    EXPECT_EQ(hits, 4);
}

// ---------------- Deterministic query generators ----------------
std::vector<int> uniformQueries(int n, int minV, int maxV, unsigned seed = 42) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dist(minV, maxV);
    std::vector<int> q(n);
    for (int &x : q)
        x = dist(gen);
    return q;
}

std::vector<int> skewedQueries(int n, int hotKey, double hotFrac = 0.7) {
    std::vector<int> q(n);
    int hotCount = static_cast<int>(n * hotFrac);
    for (int i = 0; i < hotCount; ++i)
        q[i] = hotKey;
    for (int i = hotCount; i < n; ++i)
        q[i] = i; // cold keys
    std::shuffle(q.begin(), q.end(), std::mt19937(12345));
    return q;
}

TEST(Compare, SmallCache) {
    const int CACHE_CAP = 5;

    std::vector<int> queries = {1, 2, 3, 1, 2, 3, 1, 2, 3, 4, 5, 6};

    cache::LFUCache<test::Page, int> lfu(CACHE_CAP);
    cache::BeladyCache<test::Page, int> Belady(CACHE_CAP, queries.begin(),
                                               queries.end());

    int lfu_hits =
        countCacheHits(lfu, queries.begin(), queries.end(), test::slowGetPage);
    int Belady_hits = countCacheHits(Belady, queries.begin(), queries.end(),
                                     test::slowGetPage);

    EXPECT_LE(lfu_hits, Belady_hits);
    EXPECT_GE(lfu_hits, 2); // should get at least some hits
}

// ---------------- LFU-only tests ----------------
TEST(LFU, RepeatedPattern1) {
    cache::LFUCache<test::Page, int> lfu(2);
    std::vector<int> queries = {1, 2, 1, 2, 1, 2};
    int hits =
        countCacheHits(lfu, queries.begin(), queries.end(), test::slowGetPage);
    EXPECT_EQ(hits, 4);
}

TEST(LFU, EvictHotCold) {
    cache::LFUCache<test::Page, int> lfu(2);
    std::vector<int> queries = {1, 2, 1, 3, 1, 3, 1};
    int hits =
        countCacheHits(lfu, queries.begin(), queries.end(), test::slowGetPage);
    EXPECT_EQ(hits, 4); // hits on repeated 1s and 3s
}

TEST(Belady, EvictByBelady) {
    std::vector<int> queries = {1, 2, 1, 3};
    cache::BeladyCache<test::Page, int> Belady(2, queries.begin(),
                                               queries.end());
    int hits = countCacheHits(Belady, queries.begin(), queries.end(),
                              test::slowGetPage);
    EXPECT_EQ(hits, 1); // only second 1 is a hit
}

// Test 1: Basic over-hitting detection with careful state tracking
TEST(LFUCacheOverhit, BasicOverhitTest_8294) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page, int> lfu(CACHE_CAPACITY);

    // Initial state: empty cache
    EXPECT_FALSE(lfu.lookupUpdate(1, test::slowGetPage)); // miss, cache: [1]
    EXPECT_FALSE(lfu.lookupUpdate(2, test::slowGetPage)); // miss, cache: [1,2]

    // Build frequency: 1 has higher frequency than 2
    EXPECT_TRUE(lfu.lookupUpdate(
        1, test::slowGetPage)); // hit, cache: [1(freq=2), 2(freq=1)]

    // Add 3 - should evict 2 (lower frequency than 1)
    EXPECT_FALSE(lfu.lookupUpdate(
        3, test::slowGetPage)); // miss, evicts 2, cache: [1(freq=2), 3(freq=1)]

    // CRITICAL: 2 was evicted, so this MUST be a miss
    // This will also insert 2 and evict someone
    EXPECT_FALSE(lfu.lookupUpdate(
        2, test::slowGetPage)); // miss, evicts 3, cache: [1(freq=2), 2(freq=1)]

    // Now verify final state carefully:
    EXPECT_TRUE(lfu.lookupUpdate(
        1, test::slowGetPage)); // hit, cache: [1(freq=3), 2(freq=1)]
    EXPECT_FALSE(lfu.lookupUpdate(
        3, test::slowGetPage)); // miss (3 was evicted), evicts 2, cache:
                                // [1(freq=3), 3(freq=1)]
}

// Test 2: Check that we don't get extra hits on evicted items
TEST(LFUCacheOverhit, NoExtraHits_5731) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page, int> lfu(CACHE_CAPACITY);

    // Fill cache
    EXPECT_FALSE(lfu.lookupUpdate(1, test::slowGetPage)); // miss
    EXPECT_FALSE(lfu.lookupUpdate(2, test::slowGetPage)); // miss

    // Access pattern to make frequencies different
    EXPECT_TRUE(lfu.lookupUpdate(1, test::slowGetPage)); // hit
    EXPECT_TRUE(lfu.lookupUpdate(1, test::slowGetPage)); // hit

    // Add 3 - should evict 2 (1 has higher frequency)
    EXPECT_FALSE(lfu.lookupUpdate(3, test::slowGetPage)); // miss, evicts 2

    // Now 2 should NOT be in cache
    // If this returns true, we have over-hitting bug
    bool result = lfu.lookupUpdate(2, test::slowGetPage);

    // The key assertion: if result is true, that means our cache incorrectly
    // thinks 2 is still present when it was evicted
    EXPECT_FALSE(result)
        << "OVER-HITTING BUG: Item 2 was evicted but cache returned hit!";

    // Don't check further state as it's now changed by the 2 lookup
}

// Test 3: Simple capacity check - should never have more hits than physically
// possible
TEST(LFUCacheOverhit, CapacityCheck_9462) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page, int> lfu(CACHE_CAPACITY);

    // Insert 3 different items into capacity-2 cache
    EXPECT_FALSE(lfu.lookupUpdate(1, test::slowGetPage)); // miss
    EXPECT_FALSE(lfu.lookupUpdate(2, test::slowGetPage)); // miss
    EXPECT_FALSE(lfu.lookupUpdate(3, test::slowGetPage)); // miss, evicts one

    // Now try all three - at least one MUST be a miss due to capacity
    bool hit1 = lfu.lookupUpdate(1, test::slowGetPage);
    bool hit2 = lfu.lookupUpdate(2, test::slowGetPage);
    bool hit3 = lfu.lookupUpdate(3, test::slowGetPage);

    int total_hits = (hit1 ? 1 : 0) + (hit2 ? 1 : 0) + (hit3 ? 1 : 0);

    // This is the key check: with capacity 2, we cannot have 3 hits
    // If total_hits > 2, we have over-hitting bug
    EXPECT_LE(total_hits, 2)
        << "OVER-HITTING BUG: Got " << total_hits
        << " hits but cache capacity is only " << CACHE_CAPACITY;
}

// Test 4: Deterministic frequency-based eviction
TEST(LFUCacheOverhit, FrequencyEviction_3857) {
    const size_t CACHE_CAPACITY = 2;
    cache::LFUCache<test::Page, int> lfu(CACHE_CAPACITY);

    // Setup: make 1 high frequency, 2 low frequency
    EXPECT_FALSE(lfu.lookupUpdate(1, test::slowGetPage)); // miss
    EXPECT_FALSE(lfu.lookupUpdate(2, test::slowGetPage)); // miss
    EXPECT_TRUE(lfu.lookupUpdate(1, test::slowGetPage));  // hit
    EXPECT_TRUE(lfu.lookupUpdate(1, test::slowGetPage));  // hit

    // Add 3 - should evict 2 (not 1) because 1 has higher frequency
    EXPECT_FALSE(
        lfu.lookupUpdate(3, test::slowGetPage)); // miss, should evict 2

    // Check that 2 is gone (this is the over-hitting check)
    bool hit2 = lfu.lookupUpdate(2, test::slowGetPage);
    EXPECT_FALSE(hit2)
        << "OVER-HITTING BUG: Item 2 should have been evicted due "
           "to lower frequency!";

    // 1 should still be there
    EXPECT_TRUE(lfu.lookupUpdate(1, test::slowGetPage)); // hit
}

// Test 5: Pure over-hitting detection without state pollution
TEST(LFUCacheOverhit, PureOverhitTest_7126) {
    const size_t CACHE_CAPACITY = 2;

    // We'll run the same test twice to verify consistent behavior
    for (int run = 0; run < 2; run++) {
        cache::LFUCache<test::Page, int> lfu(CACHE_CAPACITY);

        // Pattern: 1, 2, 1, 3 (forces eviction of 2)
        EXPECT_FALSE(lfu.lookupUpdate(1, test::slowGetPage));
        EXPECT_FALSE(lfu.lookupUpdate(2, test::slowGetPage));
        EXPECT_TRUE(lfu.lookupUpdate(1, test::slowGetPage));
        EXPECT_FALSE(lfu.lookupUpdate(3, test::slowGetPage)); // should evict 2

        // THE CRITICAL TEST: 2 should NOT be in cache
        // If lookupUpdate(2) returns true here, we have over-hitting bug
        bool result = lfu.lookupUpdate(2, test::slowGetPage);

        if (result) {
            ADD_FAILURE()
                << "OVER-HITTING BUG DETECTED in run " << run
                << ": Item 2 was evicted but cache claims it's present!";
            return; // No point continuing this run
        }
    }
}