#ifndef TEST_CACHE_HPP
#define TEST_CACHE_HPP

#include "Generator.hpp"
#include <climits>

namespace test {
struct Page {
    int val_;
    Page(int val) : val_(val) {};
};

inline Page slowGetPage([[maybe_unused]] int key) {
    return Page(randomInt(RAND_INT_MIN, RAND_INT_MAX));
}
} // namespace test

#endif // TEST_CACHE_FUNCS_HPP