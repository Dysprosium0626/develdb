//
// Created by Dysprosium on 2023/2/25.
//

#include "util/arena.hpp"
#include <assert.h>

namespace develdb {

static const int kBlockSize = 4096;

Arena::Arena() : block_memory_(0), alloc_ptr_(NULL), alloc_bytes_remaining_(0) {

}
Arena::~Arena() {
  for (int i = 0; i < blocks_.size(); ++i) {
    delete[] blocks_[i];
  }
}

// TODO
char *Arena::AllocateAligned(size_t bytes) {
  const int align = sizeof(void *);    // We'll align to pointer size
  assert((align & (align - 1)) == 0);   // Pointer size should be a power of 2
  size_t current_mod = reinterpret_cast<uintptr_t>(alloc_ptr_) & (align - 1);
  size_t slop = (current_mod == 0 ? 0 : align - current_mod);
  size_t needed = bytes + slop;
  char *result;
  if (needed <= alloc_bytes_remaining_) {
    result = alloc_ptr_ + slop;
    alloc_ptr_ += needed;
    alloc_bytes_remaining_ -= needed;
  } else {
    // AllocateFallback always returned aligned memory
    result = AllocateFallback(bytes);
  }
  assert((reinterpret_cast<uintptr_t>(result) & (align - 1)) == 0);
  return result;
}
char *Arena::AllocateFallback(size_t bytes) {
  if (bytes > kBlockSize / 4) {
    char *result = AllocateNewBlock(bytes);
    return result;
  }
  alloc_ptr_ = AllocateNewBlock(kBlockSize);
  alloc_bytes_remaining_ = kBlockSize;

  char *result = alloc_ptr_;
  alloc_ptr_ += bytes;
  alloc_bytes_remaining_ -= bytes;
  return result;
}
char *Arena::AllocateNewBlock(size_t block_bytes) {
  char *result = new char[block_bytes];
  block_memory_ += block_bytes;
  blocks_.push_back(result);
  return result;
}
} // develdb
