#include "Clove/Guid.hpp"

#include <random>

namespace clove {
    Guid::Guid(){
        std::random_device rd{};
        std::mt19937_64 generator{ rd() };
        std::uniform_int_distribution<Type> distribution{ 1u, std::numeric_limits<Type>::max() };

        guid = distribution(generator);
    }
}