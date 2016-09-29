#include "UUID.hpp"

#include <random>

UUID UUID::make() {
  static std::random_device rand;
  static std::mt19937_64 mersenne(rand());
  UUID uuid;
  uuid.u64[0] = mersenne();
  uuid.u64[1] = mersenne();
  return uuid;
}
