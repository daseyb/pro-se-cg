#pragma once
#include <engine/memory/Allocator.hpp>

template<class P, class F>
class FallbackAllocator : private P, private F {
public:
  Blk allocate(size_t n) {
    Blk r = P::allocate(n);
    if (!r.ptr) r = F::allocate(n);
    return r;
  }

  void deallocate(Blk blk) {
    if (P::owns(blk)) {
      P::deallocate(blk);
    } else {
      F::deallocate(blk);
    }
  }

  bool owns(Blk blk) {
    return P::owns(blk) || F::owns(blk);
  }
};