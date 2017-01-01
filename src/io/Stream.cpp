#include "Stream.hpp"

#include <endian.h>
#include <stdexcept>

namespace arn {
namespace io {

void OutStream::writeString(const std::string &str) {
  if (str.size() > UINT16_MAX)
    throw std::length_error("String too long");
  uint16 len = static_cast<uint16>(str.size());
  writeU16(len);
  writeData(str.c_str(), len);
}
std::string InStream::readString() {
  uint16 len = readU16();
  char *data = new char[len];
  readData(data, len);
  std::string str(data, len);
  delete[] data;
  return str;
}

void OutStream::writeBool(bool v) {
  writeU8(v);
}
bool InStream::readBool() {
  return readU8();
}

void OutStream::writeString32(const std::u32string &str) {
  if (str.size() > UINT16_MAX)
    throw std::length_error("String too long");
  uint16 len = static_cast<uint16>(str.size());
  writeU16(len);
  writeData(str.c_str(), len * sizeof(char32));
}
std::u32string InStream::readString32() {
  uint16 len = readU16();
  char32 *data = new char32[len];
  readData(data, len*sizeof(char32));
  std::u32string str(data, len);
  delete[] data;
  return str;
}

void OutStream::writeI64(int64 i) {
  const uint64 in = htobe64(reinterpret_cast<const uint64&>(i));
  writeData(&in, 8);
}
int64 InStream::readI64() {
  int64 val;
  readData(&val, 8);
  const uint64 valh = be64toh(val);
  return reinterpret_cast<const int64&>(valh);
}

void OutStream::writeU64(uint64 i) {
  const uint64 in = htobe64(i);
  writeData(&in, 8);
}
uint64 InStream::readU64() {
  uint64 val;
  readData(&val, 8);
  return be64toh(val);
}

void OutStream::writeI32(int32 i) {
  const uint32 in = htobe32(reinterpret_cast<const uint32&>(i));
  writeData(&in, 4);
}
int32 InStream::readI32() {
  int32 val;
  readData(&val, 4);
  const uint32 valh = be32toh(val);
  return reinterpret_cast<const int32&>(valh);
}

void OutStream::writeU32(uint32 i) {
  const uint32 in = htobe32(i);
  writeData(&in, 4);
}
uint32 InStream::readU32() {
  uint32 val;
  readData(&val, 4);
  return be32toh(val);
}

void OutStream::writeI16(int16 i) {
  const uint16 in = htobe16(reinterpret_cast<const uint16&>(i));
  writeData(&in, 2);
}
int16 InStream::readI16() {
  int16 val;
  readData(&val, 2);
  const uint16 valh = be16toh(val);
  return reinterpret_cast<const int16&>(valh);
}

void OutStream::writeU16(uint16 i) {
  const uint16 in = htobe16(i);
  writeData(&in, 2);
}
uint16 InStream::readU16() {
  uint16 val;
  readData(&val, 2);
  return be16toh(val);
}

void OutStream::writeI8(int8 i) {
  writeData(&i, 1);
}
int8 InStream::readI8() {
  int8 val;
  readData(&val, 1);
  return val;
}

void OutStream::writeU8(uint8 i) {
  writeData(&i, 1);
}
uint8 InStream::readU8() {
  uint8 val;
  readData(&val, 1);
  return val;
}

void OutStream::writeFloat(float f) {
  union { float f; uint32 u; } cvt = { f };
  const uint32 un = htobe32(cvt.u);
  writeData(&un, sizeof(float));
}
float InStream::readFloat() {
  uint32 un;
  readData(&un, sizeof(float));
  union { uint32 u; float f; } cvt = { be32toh(un) };
  return cvt.f;
}

void OutStream::writeDouble(double d) {
  union { double d; uint64 u; } cvt = { d };
  const uint64 un = htobe64(cvt.u);
  writeData(&un, sizeof(double));
}
double InStream::readDouble() {
  uint64 un;
  readData(&un, sizeof(double));
  union { uint64 u; double d; } cvt = { be64toh(un) };
  return cvt.d;
}

}
}
