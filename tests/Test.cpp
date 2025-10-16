#include <gtest/gtest.h>

#include "Generator.hpp"
#include "TestCache.hpp"
#include "LIRSCache.hpp"


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}


TEST(Manual, LIRSCacheCheck) {
    Cache::Cache<test::Page> cache(100);

    cache.lookupUpdate(5, test::slowGetPage);
    cache.lookupUpdate(1, test::slowGetPage);
    cache.lookupUpdate(52, test::slowGetPage);
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
