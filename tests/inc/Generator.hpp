#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <random>



namespace gen
{
    
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

} // namespace gen


#endif // GENERATOR_HPP