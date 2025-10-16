#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <random>
#include <vector>


namespace test
{

const int RAND_INT_MAX = INT_MAX / 2;
const int RAND_INT_MIN = INT_MIN / 2;

inline double randomDouble() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double randomInt(int min, int max) {
    return min + (int) ((max-min)*randomDouble());
}

inline double randomDouble(double min, double max) {
    return min + (max-min)*randomDouble();
}

inline std::vector<int> randomIntVector(const size_t size, const int min, const int max) {
    std::vector<int> vec;
    vec.reserve(size);

    for (size_t i = 0; i < size; ++i)
        vec.push_back(randomInt(min, max));

    return vec;
}

} // namespace test


#endif // GENERATOR_HPP