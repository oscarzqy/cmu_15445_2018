/**
 * LRU implementation
 */
#include "buffer/lru_replacer.h"
#include "page/page.h"

namespace cmudb {

template <typename T> LRUReplacer<T>::LRUReplacer() {}

template <typename T> LRUReplacer<T>::~LRUReplacer() {}

/*
 * Insert value into LRU
 */
template <typename T> void LRUReplacer<T>::Insert(const T &value) {
  if (iter_map_.count(value)) {
    list_.erase(iter_map_[value]);
  }
  list_.push_front(value);
  iter_map_[value] = list_.begin();
}

/* If LRU is non-empty, pop the head member from LRU to argument "value", and
 * return true. If LRU is empty, return false
 */
template <typename T> bool LRUReplacer<T>::Victim(T &value) {
  if (list_.empty()) {
    return false;
  }
  value = list_.back();
  list_.pop_back();
  iter_map_.erase(value);
  return true;
}

/*
 * Remove value from LRU. If removal is successful, return true, otherwise
 * return false
 */
template <typename T> bool LRUReplacer<T>::Erase(const T &value) {
  if (!iter_map_.count(value)) {
    return false;
  }
  auto iter = iter_map_[value];
  list_.erase(iter);
  iter_map_.erase(value);
  return true;
}

template <typename T> size_t LRUReplacer<T>::Size() {
  return list_.size();
}

template class LRUReplacer<Page *>;
// test only
template class LRUReplacer<int>;

} // namespace cmudb
