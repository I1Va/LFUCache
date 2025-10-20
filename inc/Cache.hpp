#ifndef CACHE_HPP
#define CACHE_HPP

#include "BeladyCache.hpp"
#include "LFUCache.hpp"


template <typename T>
struct isCacheType : std::false_type {};

template <typename DataT>
struct isCacheType<cache::LFUCache<DataT>> : std::true_type {};

template <typename DataT, typename KeyT>
struct isCacheType<cache::BeladyCache<DataT, KeyT>> : std::true_type {};

template <typename T>
concept CacheType = isCacheType<T>::value;

template <CacheType CacheT, typename F>
int countCacheHits(CacheT &cache, const std::vector<int> &queries, F slowGetPage) {
    int result = 0;
    for (int q : queries)
        result += cache.lookupUpdate(q, slowGetPage);
    return result;
}


#endif // CACHE_HPP