#ifndef FORWARD_OFFLINE_CORE_JAVA_RANDOM_H
#define FORWARD_OFFLINE_CORE_JAVA_RANDOM_H

#include <cstdint>

namespace forward_offline {

class JavaRandom {
public:
    explicit JavaRandom(std::uint64_t seed);

    float next_float();

private:
    std::uint32_t next_bits(int bits);

    std::uint64_t seed_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_CORE_JAVA_RANDOM_H
