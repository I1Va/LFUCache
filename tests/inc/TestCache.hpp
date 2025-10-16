#ifndef TEST_CACHE_HPP
#define TEST_CACHE_HPP

#include <climits>
#include "Generator.hpp"


namespace test 
{
    struct Page {
        int val_;
        Page(int val): val_(val) {};
    };

    inline Page slowGetPage(int key) {
        return Page(randomInt(RAND_INT_MIN, RAND_INT_MAX)); 
    }
}


#endif // TEST_CACHE_FUNCS_HPP