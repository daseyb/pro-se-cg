#pragma once
#pragma once
#include <stdexcept>
#include <assert.h>
#include <engine/memory/Allocator.hpp>

template<size_t size>
class PoolAllocator : Allocator {
public:
  PoolAllocator(size_t maxObjects) {
    //Allocate storage for the data and the PoolNodes
    storage = (PoolNode*)malloc(sizeof(PoolNode) * maxObjects);
    objects = (char*)malloc(maxObjects * size);

    if (!objects || !storage) {
      //We couldn't allocate enough memory for the pool.
      throw std::bad_alloc();
    }

    //Build the initial linked list of free nodes
    //It's important that the index of a node is the same as the index of the object it links to.
    for (int i = 0; i < maxObjects; i++) {
      storage[i].object = &objects[i];
      storage[i].next = i < maxObjects - 1 ? &storage[i + 1] : nullptr;
      storage[i].isAllocated = false;
    }

    //Point the head of the list to the start of our node storage memory.
    freeListHead = &storage[0];
    freeObjectCount = maxObjects;
    poolSize = maxObjects;
  }

  ~PoolAllocator() {
    //Free all of the memory we allocated.
    if (objects) {
      free(objects);
    }

    if (storage) {
      free(storage);
    }

    storage = nullptr;
    freeListHead = nullptr;
  }


  Blk allocate(size_t n) override {
    assert(n == size);

    //If there is no element in the list of free nodes we have reached capacity.
    if (freeListHead == nullptr) {
      throw std::runtime_error("There is no more space in this pool.");
    }

    //Mark the first free element as allocated.
    PoolNode* oldFirst = freeListHead;
    oldFirst->isAllocated = true;

    //Move the free list to the next element.
    freeListHead = freeListHead->next;

    freeObjectCount--;

    return{ oldFirst->object, size };
  }

  void deallocate(Blk blk) {
    //Find the PoolNode that corresponds to the given object.
    //Null if it's not in the pool.
    PoolNode* correspondingNode = findPoolNodeAndRemove(blk.ptr);

    if (correspondingNode == nullptr) {
      throw std::runtime_error("The given object is not in this pool.");
    }

    //If we aren't pointing to the right object something is seriously wrong.
    assert(correspondingNode->object == blk.ptr);

    //We shouldn't try to free objects that aren't marked as allocated.
    if (!correspondingNode->isAllocated) {
      throw std::runtime_error("The given object is in this pool, but not allocated.");
    }

    freeObjectCount++;
    //Append the object we just freed to the head of the free object list.
    correspondingNode->next = freeListHead;
    correspondingNode->isAllocated = false;
    freeListHead = correspondingNode;
  }

  bool owns(Blk blk) override {
    int offset = ((char*)blk.ptr - objects);
    if (offset < 0 || offset >= poolSize) {
      return false;
    }
    return true;
  }

private:
  struct PoolNode {
    void* object;
    PoolNode* next;
    bool isAllocated;
  };

  int freeObjectCount;
  int poolSize;

  char* objects;

  PoolNode* storage;
  PoolNode* freeListHead;

  PoolNode* findPoolNodeAndRemove(void* val) {
    //Get index of val into our objects array. This is really an index, because pointer math takes data structure size into account.
    int offset = ((char*)val - objects);
    //If the given pointer wasn't pointing into the memory we assigned we didn't allocate it.
    if (offset < 0 || offset >= poolSize) {
      return nullptr;
    }
    //Otherwise, get the associated PoolNode (the offset is the same, we allocated it in the same order)
    return &storage[offset];
  }
};