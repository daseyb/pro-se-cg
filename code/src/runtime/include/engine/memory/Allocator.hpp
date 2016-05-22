#pragma once
#include <stdint.h>

struct Blk {
  void* ptr;
  size_t size;
};

class Allocator {
public:
  virtual Blk allocate(size_t n) = 0;
  virtual void deallocate(Blk blk) = 0;
  virtual bool owns(Blk blk) = 0;
};

constexpr size_t roundToAligned(size_t n, size_t align) {
  return n + ((n % align == 0) ? 0 : (align - n % align));
}

constexpr size_t megabytes(size_t n) {
  return n * 1024 * 1024;
}

constexpr size_t kilobytes(size_t n) {
  return n * 1024;
}