#include "core/java_random.h"

namespace forward_offline {

namespace {

const std::uint64_t kMultiplier = 0x5DEECE66DULL;
const std::uint64_t kAddend = 0xBULL;
const std::uint64_t kMask = (1ULL << 48) - 1ULL;

}  // namespace

JavaRandom::JavaRandom(std::uint64_t seed)
    : seed_((seed ^ kMultiplier) & kMask) {
}

float JavaRandom::next_float() {
    return static_cast<float>(next_bits(24)) / static_cast<float>(1 << 24);
}

std::uint32_t JavaRandom::next_bits(int bits) {
    seed_ = (seed_ * kMultiplier + kAddend) & kMask;
    return static_cast<std::uint32_t>(seed_ >> (48 - bits));
}

}  // namespace forward_offline
