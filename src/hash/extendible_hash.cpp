#include <list>
#include <functional>
#include <iostream>
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
  buckets_.push_back(std::make_shared<bucket_t>(0));
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
    value = buckets_[index]->data[key];
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
    return false;
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
  auto bucket = buckets_[index];
  if ((int)bucket->data.size() < max_bucket_size_) {
    bucket->data[key] = value;
  } else {
    while ((int)bucket->data.size() >= max_bucket_size_) {
      if (bucket->local_depth == global_depth_) {
        int length = buckets_.size();
        // little bit different from the tutorial
        // hash(key) & (1<<bucket->local_depth -1/) will point to the same block
        // whether those buckets are adjacent dosen't really matter.
        for (int i = 0; i < length; ++i) {
          buckets_.push_back(buckets_[i]);
        }
        global_depth_++;
      }
      size_t mask = 1 << (bucket->local_depth);
      auto bucket0 = std::make_shared<bucket_t>(bucket->local_depth+1);
      auto bucket1 = std::make_shared<bucket_t>(bucket->local_depth+1);
      for (auto &entry : bucket->data) {
        if (HashKey(entry.first) & mask) {
          bucket1->data[entry.first] = entry.second;
        } else {
          bucket0->data[entry.first] = entry.second;
        }
      }
      for (size_t i = 0; i < buckets_.size(); ++i) {
        if (buckets_[i] != bucket) {
          continue;
        }
        if (i & mask) {
          buckets_[i] = bucket1;
        } else {
          buckets_[i] = bucket0;
        }
      }
      index = GetBucketIndex(key);
      bucket = buckets_[index];
    }
    bucket->data[key] = value;
  }
}

template class ExtendibleHash<page_id_t, Page *>;
template class ExtendibleHash<Page *, std::list<Page *>::iterator>;
// test purpose
template class ExtendibleHash<int, std::string>;
template class ExtendibleHash<int, std::list<int>::iterator>;
template class ExtendibleHash<int, int>;
} // namespace cmudb
