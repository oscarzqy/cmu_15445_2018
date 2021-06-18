/*
 * extendible_hash.h : implementation of in-memory hash table using extendible
 * hashing
 *
 * Functionality: The buffer pool manager must maintain a page table to be able
 * to quickly map a PageId to its corresponding memory location; or alternately
 * report that the PageId does not match any currently-buffered page.
 */

#pragma once

#include <cstdlib>
#include <vector>
#include <string>
#include <tuple>
#include <unordered_map>
#include <memory>
#include <mutex>

#include "hash/hash_table.h"

namespace cmudb {


template <typename K, typename V>
class ExtendibleHash : public HashTable<K, V> {

typedef struct Bucket {
  std::unordered_map<K, V> data;
  int local_depth{};
  Bucket(int depth) : local_depth(depth) {}
} bucket_t;

public:
  // constructor
  ExtendibleHash(size_t size);
  // helper function to generate hash addressing
  size_t HashKey(const K &key) const;
  // helper function to get global & local depth
  int GetGlobalDepth() const;
  int GetLocalDepth(int bucket_id);
  int GetNumBuckets() const;
  // lookup and modifier
  bool Find(const K &key, V &value) override;
  bool Remove(const K &key) override;
  void Insert(const K &key, const V &value) override;

private:
  // add your own member variables here€
  int global_depth_{};
  int max_bucket_size_{};
  std::mutex mutex_;
  std::vector<std::shared_ptr<bucket_t>> buckets_;

  int GetBucketIndex(const K &key) const;
};
} // namespace cmudb
