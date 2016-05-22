#pragma once
#include <engine/memory/Allocator.hpp>
#include <memory>

template <size_t align = 16> class GlobalStackAllocator : Allocator {
private:
  size_t m_size;
  std::unique_ptr<char> m_mem;
  char *m_top;

public:
  GlobalStackAllocator(GlobalStackAllocator&& other) {
    m_size = other.m_size;
    m_top = other.m_top;
    m_mem = std::move(other.m_mem);
  }

  GlobalStackAllocator(size_t size) : m_size(size) {
    m_mem.reset((char *)malloc(size));
    m_top = m_mem.get();
  }

  Blk allocate(size_t n) {
    auto n1 = roundToAligned(n, align);
    if ((m_top - m_mem.get()) + n1 > m_size) {
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

  void deallocateAll() { m_top = m_mem.get(); }

  bool owns(Blk blk) {
    return blk.ptr > m_mem.get() && blk.ptr < m_mem.get() + m_size;
  }
};