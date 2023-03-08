//
// Created by Dysprosium on 2023/2/25.
//

#ifndef DEVELDB_UTIL_ARENA_HPP_
#define DEVELDB_UTIL_ARENA_HPP_

#include <stddef.h>
#include <vector>

namespace develdb {
class Arena {
 public:
  Arena();
  ~Arena();

  char *Allocate(size_t bytes);

  char *AllocateAligned(size_t bytes);

  size_t MemoryUsage() const {
    return block_memory_ + blocks_.capacity() * sizeof(char *);
  }
 private:
  char *AllocateFallback(size_t bytes);
  char *AllocateNewBlock(size_t block_bytes);

  char *alloc_ptr_;
  size_t alloc_bytes_remaining_;

  std::vector<char *> blocks_;

  size_t block_memory_;

  Arena(const Arena &);
  void operator=(const Arena &);

};

inline char *Arena::Allocate(size_t bytes) {
  assert(bytes > 0);
  if (bytes <= alloc_bytes_remaining_) {
    char *result = alloc_ptr_;
    alloc_ptr_ += bytes;
    alloc_bytes_remaining_ -= bytes;
    return result;
  }
  return AllocateFallback(bytes);
}

} // develdb


#endif //DEVELDB_UTIL_ARENA_HPP_
