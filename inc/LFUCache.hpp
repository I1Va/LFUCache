#ifndef LFUCACHE_HPP
#define LFUCACHE_HPP

#include <list>
#include <unordered_map>

namespace cache
{

template <typename T>
struct LFUCacheNode {
    using FreqT = int;
    using KeyT = int;

    KeyT key_;
    T data_;
    FreqT freq_;
};

template <typename T>
class LFUCache {
    using keyT = typename LFUCacheNode<T>::KeyT;
    using CacheListIt = typename std::list<LFUCacheNode<T>>::iterator;
    using FreqT = typename LFUCacheNode<T>::FreqT;
    
    std::unordered_map<KeyT, CacheListIt> hashTable_;
    std::unordered_map<FreqT, std::list<LFUCacheNode<T>>> freqTable_;

    FreqT minFreq_ = 0;
    size_t size_ = 0;
    size_t capacity_ = 0;

private:
    bool full() const { return (size_ == capacity_); }
    
    void refreshKey(const KeyT key) {
        assert(hashTable_.find(key) != hashTable_.end());
        
        CacheListIt cacheListIt = hashTable_[key];
        FreqT oldFreq = cacheListIt->freq_++;
    
        assert(freqTable_.find(oldFreq) != freqTable_.end());
        freqTable_[oldFreq].erase(cacheListIt);

        if (freqTable_[oldFreq].empty() && oldFreq == minFreq_) minFreq_++;

        freqTable_[cacheListIt->freq_].push_back(cacheListIt);
    }

    void removeLFUNode() {
        assert(freqTable_.find(minFreq_) != freqTable_.end());

        CacheListIt LFUIt = freqTable_[minFreq_].front();
        hashTable_.erase(LFUIt->key_);
        
        freqTable_[minFreq_].pop_front();
        if (freqTable_[minFreq_].empty()) minFreq_++;
    }

    void addNewCacheNode(const KeyT key, const T &data) {
        
    }

    

public:
    LFUCache(const size_t capacity): capacity_(capacity) {}

    template <typename F>
    bool lookupUpdate(KeyT key, F slowGetPage) {
        auto hit = hashTable_.find(key);
        if (hit == hashTable_.end()) {
            if (full()) removeLFUNode();

            freqTable_[0].push_back({key, slowGetPage(key), 0});
            minFreq_ = 0;

            

            return false;          
        }

        CacheListIt hitPos = hit->second;
        refreshCachePos(hitPos);

        return true;
    }
};

}


#endif // LFUCACHE_HPP