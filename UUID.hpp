#ifndef UUID_H
#define UUID_H

#include <cstdint>

struct UUID {
  struct RFC {
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint16_t data4;
    uint8_t data5[6];
  };
  union {
    RFC rfc;
    uint64_t u64[2];
    uint32_t u32[4];
    uint16_t u16[8];
    uint8_t u8[16];
  };

  uint8_t operator[](int idx) const {
    return u8[idx];
  }

  uint8_t& operator[](int idx) {
    return u8[idx];
  }

  static UUID make();
};

#endif // UUID_H
