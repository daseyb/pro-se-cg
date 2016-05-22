#pragma once
#include <engine/memory/GlobalStackAllocator.hpp>
#include <assert.h>
#include <memory>

template <class T> class Stack {
private:
  Blk m_mem;
  size_t m_maxSize;
  size_t m_currentSize;
  GlobalStackAllocator<sizeof(T)> m_allocator;

public:
  Stack(size_t maxSize) : m_allocator(maxSize* sizeof(T)) {
    m_mem = {nullptr, 0};
    m_currentSize = 0;
    m_maxSize = 0;
    reserve(maxSize);
  }

  Stack(const Stack& other) : m_allocator(other.m_maxSize* sizeof(T)) {
    m_mem = { nullptr, 0 };
    m_currentSize = 0;
    m_maxSize = 0;
    reserve(other.m_maxSize);
  }

  Stack(Stack&& other) : m_allocator(std::move(other.m_allocator)) {
    m_mem = std::move(other.m_mem);
    other.m_mem = { nullptr, 0 };
    m_currentSize = other.m_currentSize;
    m_maxSize = other.m_maxSize;
  }

  ~Stack() { m_allocator.deallocate(m_mem); }

  void reserve(size_t num) {
    if (num > 0 && m_maxSize < num) {
      if (m_mem.ptr != nullptr) {
        m_allocator.deallocate(m_mem);
      }
      m_mem = m_allocator.allocate(num * sizeof(T));
      assert(m_mem.ptr != nullptr);
      m_maxSize = num;
    }
  }

  void push(T el) {
    assert(m_currentSize < m_maxSize);
    memcpy((T*)m_mem.ptr + m_currentSize, &el, sizeof(T));
    m_currentSize++;
  }

  T pop() {
    assert(m_currentSize > 0);
    T res = *((T *)m_mem.ptr + m_currentSize - 1);
    m_currentSize--;
    return res;
  }

  T &at(const size_t index) {
    assert(m_currentSize > index);
    T &res = *((T *)m_mem.ptr + index);
    return res;
  }

  T &operator[](const size_t index) { return at(index); }

  size_t size() { return m_currentSize; }

  void reset() { m_currentSize = 0; }
};