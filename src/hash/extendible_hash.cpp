#include <list>
#include <functional>
#include "hash/extendible_hash.h"
#include "page/page.h"

namespace cmudb {

/*
 * constructor
 * array_size: fixed array size for each bucket
 */
template <typename K, typename V>
ExtendibleHash<K, V>::ExtendibleHash(size_t size) : global_depth_(0),
  max_bucket_size_(size) {
  buckets_.push_back(std::make_shared<bucket_t>());
}

/*
 * helper function to calculate the hashing address of input key
 */
template <typename K, typename V>
size_t ExtendibleHash<K, V>::HashKey(const K &key) const {
  return std::hash<K>()(key);
}

/*
 * helper function to return global depth of hash table
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetGlobalDepth() const {
  return global_depth_;
}

/*
 * helper function to return local depth of one specific bucket
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetLocalDepth(int bucket_id) const {
  if (bucket_id >= (int)buckets_.size() || bucket_id < 0) {
    std::cerr << "bucket_id out of range" << std::endl;
    return -1;
  }
  return buckets_[bucket_id]->local_depth;
}

/*
 * helper function to return current number of bucket in hash table
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetNumBuckets() const {
  return buckets_.size();
}

/*
 * helper function to return the index of the bucket.
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetBucketIndex(const K &key) const {
  size_t hashed_key = HashKey(key);
  int index = hashed_key & ((1 << GetGlobalDepth()) - 1);
  return index;
}


/*
 * lookup function to find value associate with input key
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Find(const K &key, V &value) {
  int index = GetBucketIndex(key);
  if (buckets_[index]->data.count(key)) {
    return true;
  }
  return false;
}

/*
 * delete <key,value> entry in hash table
 * Shrink & Combination is not required for this project
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Remove(const K &key) {
  int index = GetBucketIndex(key);
  auto &data = buckets_[index]->data;
  auto it = data.find(key);
  if (it == data.end()) {
    return true;
  }
  return data.erase(key);
}

/*
 * insert <key,value> entry in hash table
 * Split & Redistribute bucket when there is overflow and if necessary increase
 * global depth
 */
template <typename K, typename V>
void ExtendibleHash<K, V>::Insert(const K &key, const V &value) {
  int index = GetBucketIndex(key);
  auto &bucket = buckets_[index];
  if ((int)bucket->data.size() < max_bucket_size_) {
    bucket->data[key] = value;
  } else {
    bucket->local_depth++;
    auto new_bucket = std::make_shared<bucket_t>();
    new_bucket->local_depth = bucket->local_depth;
    size_t mask = 1 << bucket->local_depth;
    for (auto it = bucket->data.begin(); it != bucket->data.end();) {
      size_t last_bit = HashKey(it->first) & mask;
      if (last_bit) {
        new_bucket->data[it->first] = it->second;
        // a trick using lvalue to avoid iterator becoming invalid.
        bucket->data.erase(it++);
      } else {
        it++;
      }
    }
    if (bucket->local_depth <= global_depth_) {
      // no need to expand the buckets size.
      buckets_[index+1] = std::move(new_bucket);
    } else {
      global_depth_++;
      int new_size = 1 << global_depth_;
      int old_size = new_size >> 1;
      buckets_.resize(new_size);
      for (int i = old_size; i >= 0; --i) {
        buckets_[(i>>1)+1] = buckets_[i>>1] = buckets_[i];
      }
      buckets_[(index>>1) + 1] = new_bucket;
    }
  }
}

template class ExtendibleHash<page_id_t, Page *>;
template class ExtendibleHash<Page *, std::list<Page *>::iterator>;
// test purpose
template class ExtendibleHash<int, std::string>;
template class ExtendibleHash<int, std::list<int>::iterator>;
template class ExtendibleHash<int, int>;
} // namespace cmudb
