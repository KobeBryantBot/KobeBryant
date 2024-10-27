#include "api/utils/RandomUtils.hpp"
#include <random>

namespace utils {

template <typename T>
T randomNumber(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()) {
    std::random_device               rd;
    std::mt19937                     gen(rd());
    std::uniform_int_distribution<T> distr(min, max);
    T                                random_num = distr(gen);
    return random_num;
}

int Random::nextInt() { return randomNumber<int>(); }

int Random::nextInt(int min, int max) { return randomNumber<int>(min, max); }

int64_t Random::nextLong() { return randomNumber<int64_t>(); }

int64_t Random::nextLong(int64_t min, int64_t max) { return randomNumber<int64_t>(min, max); }

float Random::nextFloat() { return randomNumber<float>(); }

float Random::nextFloat(float min, float max) { return randomNumber<float>(min, max); }

double Random::nextDouble() { return randomNumber<double>(); }

double Random::nextDouble(double min, double max) { return randomNumber<double>(min, max); }


} // namespace utils
