#pragma once
#include "Macros.hpp"
#include <iostream>

namespace utils {

class Random {
public:
    KobeBryant_NDAPI static int nextInt();

    KobeBryant_NDAPI static int nextInt(int min, int max);

    KobeBryant_NDAPI static int64_t nextLong();

    KobeBryant_NDAPI static int64_t nextLong(int64_t min, int64_t max);

    KobeBryant_NDAPI static float nextFloat();

    KobeBryant_NDAPI static float nextFloat(float min, float max);

    KobeBryant_NDAPI static double nextDouble();

    KobeBryant_NDAPI static double nextDouble(double min, double max);
};

} // namespace utils
