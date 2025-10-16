#include <gtest/gtest.h>

#include "generator.hpp"
#include "LIRSCache.hpp"


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}


TEST(Manual, Add) {
    Cache::Cache cache;
    EXPECT_EQ(5, cache.add(2, 3));
    EXPECT_EQ(6, cache.add(2, 4));
    EXPECT_EQ(7, cache.add(2, 5));
    EXPECT_EQ(8, cache.add(2, 6));
}

TEST(Auto, Add1000) {
    Cache::Cache cache;
    
    for (int i = 0; i < 1000; i++) {
        int a = gen::randomInt(-1000, 1000);
        int b = gen::randomInt(-1000, 1000);
        
        SCOPED_TRACE("case #" + std::to_string(i) +
                 " a=" + std::to_string(a) +
                 " b=" + std::to_string(b));

        EXPECT_EQ(a + b, cache.add(a, b));
    }
}
