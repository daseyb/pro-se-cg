#pragma once
#include <engine/memory/Allocator.hpp>

template <size_t s, size_t align = 16> class LocalStackAllocator : Allocator {
private:
  alignas(align) char m_mem[s];
  char *m_top;

public:
  Blk allocate(size_t n) {
    auto n1 = roundToAligned(n, align);
    if ((m_top - m_mem) + n1 > s) {
      return {nullptr, 0};
    }
    Blk r = {m_top, n};
    m_top += n1;
    return r;
  }

  void deallocate(Blk blk) {
    if ((char *)blk.ptr + roundToAligned(blk.size, align) == m_top) {
      m_top = (char *)blk.ptr;
    }
  }

  void deallocateAll() { m_top = m_mem; }

  bool owns(Blk blk) { return blk.ptr > m_mem && blk.ptr < m_mem + s; }
};