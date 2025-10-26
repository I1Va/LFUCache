#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <limits.h>
#include <random>
#include <vector>

namespace test {

inline constexpr int RAND_INT_MAX = INT_MAX / 2;
inline constexpr int RAND_INT_MIN = INT_MIN / 2;

inline double randomDouble() {
    static std::random_device rd;
    static std::mt19937 generator(rd());
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}

inline double randomInt(int min, int max) {
    return min + (int)((max - min) * randomDouble());
}

inline double randomDouble(double min, double max) {
    return min + (max - min) * randomDouble();
}

inline std::vector<int> randomIntVector(const size_t size, const int min,
                                        const int max) {
    std::vector<int> vec;
    vec.reserve(size);

    for (size_t i = 0; i < size; ++i)
        vec.push_back(randomInt(min, max));

    return vec;
}

} // namespace test

#endif // GENERATOR_HPP