//
// Created by Dysprosium on 2023/2/23.
//

#ifndef DEVELDB_SKIPLIST_HPP
#define DEVELDB_SKIPLIST_HPP

#include <cassert>
#include "util/random.h"

namespace develdb {
template<typename Key, class Comparator>
class SkipList {
 private:
  struct Node;
 public:
  explicit SkipList(Comparator cmp);

  void Insert(const Key &key);

  bool Contains(const Key &key) const;

  class Iterator {
   public:
    explicit Iterator(const SkipList *list);

    bool Valid() const;

    const Key &key() const;

    void Next();

    void Prev();

    void Seek(const Key &target);

    void SeekToFirst();

    void SeekToLast();

   private:
    const SkipList *list_;
    Node *node_;
  };

 private:
  enum {
    kMaxHeight = 12
  };
  Comparator const compare_;
  Node *const head_;
  int max_height_;

  inline int GetMaxHeight() const {
    return max_height_;
  }

  Random rnd_;

  Node *NewNode(const Key &key, int height);

  bool Equal(const Key &a, const Key &b) const {
    return compare_(a, b) == 0;
  };

  bool KeyIsAfterNode(const Key &key, Node *n) const;

  Node *FindGreaterOrEqual(const Key &key, Node **prev) const;

  Node *FindLessThan(const Key &key) const;

  Node *FindLast() const;

  SkipList(const SkipList &);

  void operator=(const SkipList &);

  int RandomHeight();
};

template<typename Key, class Comparator>
struct SkipList<Key, Comparator>::Node {
  explicit Node(const Key &k) : key(k) {};
  const Key key;

  Node *Next(int n) {
    assert(n >= 0);
    return next_[n];
  }

  void SetNext(int n, Node *x) {
    assert(n >= 0);
    next_[n] = x;
  }

 private:
  Node *next_[1];
};

template<typename Key, class Comparator>
SkipList<Key, Comparator>::SkipList(Comparator cmp)
    :compare_(cmp),
     head_(NewNode(0, kMaxHeight)),
     max_height_(1),
     rnd_(0xdeadbeef) {
  for (int i = 0; i < kMaxHeight; ++i) {
    head_->SetNext(i, nullptr);
  }
}

template<typename Key, class Comparator>
int SkipList<Key, Comparator>::RandomHeight() {
  // Increase height with probability 1 in kBranching
  static const unsigned int kBranching = 4;
  int height = 1;
  while (height < kMaxHeight && ((rnd_.Next() % kBranching) == 0)) {
    height++;
  }
  assert(height > 0);
  assert(height <= kMaxHeight);
  return height;
}

template<typename Key, class Comparator>
void SkipList<Key, Comparator>::Insert(const Key &key) {
  Node *prev[kMaxHeight];
  Node *x = FindGreaterOrEqual(key, prev);

  assert(x == nullptr || !Equal(key, x->key));

  int height = RandomHeight();
  if (height > GetMaxHeight()) {
    for (int i = GetMaxHeight(); i < height; ++i) {
      prev[i] = head_;
    }
    max_height_ = height;
  }
  x = NewNode(key, height);
  for (int i = 0; i < height; ++i) {
    x->SetNext(i, prev[i]->Next(i));
    prev[i]->SetNext(i, x);
  }
}

template<typename Key, class Comparator>
bool SkipList<Key, Comparator>::Contains(const Key &key) const {
  Node *x = FindGreaterOrEqual(key, nullptr);
  if (x != nullptr && Equal(key, x->key)) {
    return true;
  } else {
    return false;
  }
}

template<typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node *SkipList<Key, Comparator>::NewNode(const Key &key, int height) {
  return new Node(key);
}

template<typename Key, class Comparator>
inline SkipList<Key, Comparator>::Iterator::Iterator(const SkipList *list) {
  list_ = list;
  node_ = nullptr;
}

template<typename Key, class Comparator>
inline bool SkipList<Key, Comparator>::Iterator::Valid() const {
  return node_ != nullptr;
}

template<typename Key, class Comparator>
inline const Key &SkipList<Key, Comparator>::Iterator::key() const {
  assert(Valid());
  return node_->key;
}

template<typename Key, class Comparator>
inline void SkipList<Key, Comparator>::Iterator::Next() {
  assert(Valid());
  node_ = node_->Next(0);
}

template<typename Key, class Comparator>
void SkipList<Key, Comparator>::Iterator::Prev() {
  assert(Valid());
  node_ = list_->FindLessThan(node_->key);
  if (node_ == list_->head_) {
    node_ = nullptr;
  }
}

template<typename Key, class Comparator>
void SkipList<Key, Comparator>::Iterator::Seek(const Key &target) {
  node_ = list_->FindGreaterOrEqual(target, nullptr);
}

template<typename Key, class Comparator>
void SkipList<Key, Comparator>::Iterator::SeekToFirst() {
  node_ = list_->head_->Next(0);
}

template<typename Key, class Comparator>
void SkipList<Key, Comparator>::Iterator::SeekToLast() {
  node_ = list_->FindLast();
  if (node_ == list_->head_) {
    node_ = nullptr;
  }
}

template<typename Key, class Comparator>
bool SkipList<Key, Comparator>::KeyIsAfterNode(const Key &key, SkipList::Node *n) const {
  return (n != nullptr) and (compare_(n->key, key) < 0);
}

template<typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node *
SkipList<Key, Comparator>::FindGreaterOrEqual(const Key &key, SkipList::Node **prev) const {
  Node *x = head_;
  int level = GetMaxHeight() - 1;
  while (true) {
    Node *next = x->Next(level);
    if (KeyIsAfterNode(key, next)) {
      x = next;
    } else {
      if (prev != nullptr) prev[level] = x;
      if (level == 0) {
        return next;
      } else {
        level--;
      }
    }
  }
}

template<typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node *SkipList<Key, Comparator>::FindLessThan(const Key &key) const {
  Node *x = head_;
  int level = GetMaxHeight() - 1;
  while (true) {
    assert(x == head_ || compare_(x->key, key) < 0);
    Node *next = x->Next(level);
    if (next == nullptr || compare_(next->key, key) >= 0) {
      if (level == 0) {
        return x;
      } else {
        level--;
      }
    } else {
      x = next;
    }
  }
}

template<typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node *SkipList<Key, Comparator>::FindLast() const {
  Node *x = head_;
  int level = GetMaxHeight() - 1;
  while (true) {
    Node *next = x->Next(level);
    if (next == nullptr) {
      if (level == 0) {
        return x;
      } else {
        level--;
      }
    } else {
      x = next;
    }
  }
}

} // develdb


#endif //DEVELDB_SKIPLIST_HPP
