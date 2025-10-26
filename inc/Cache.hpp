#ifndef CACHE_HPP
#define CACHE_HPP

#include "BeladyCache.hpp"
#include "LFUCache.hpp"

template <typename T> struct CacheKeyType;

template <typename DataT, typename KeyT>
struct CacheKeyType<cache::LFUCache<DataT, KeyT>> {
    using type = KeyT;
};

template <typename DataT, typename KeyT>
struct CacheKeyType<cache::BeladyCache<DataT, KeyT>> {
    using type = KeyT;
};

template <typename T> struct isCacheType : std::false_type {};

template <typename DataT, typename KeyT>
struct isCacheType<cache::LFUCache<DataT, KeyT>> : std::true_type {};

template <typename DataT, typename KeyT>
struct isCacheType<cache::BeladyCache<DataT, KeyT>> : std::true_type {};

template <typename T>
concept CacheType = isCacheType<T>::value;

template <CacheType CacheT, typename F, typename IterT>
    requires std::same_as<typename std::iterator_traits<IterT>::value_type,
                          typename CacheKeyType<CacheT>::type>
int countCacheHits(CacheT &cache, const IterT beginIt, const IterT endIt,
                   F slowGetPage) {
    int result = 0;
    for (IterT start = beginIt; start != endIt; start++) {
        result += cache.lookupUpdate(*start, slowGetPage);
    }
    return result;
}

#endif // CACHE_HPP