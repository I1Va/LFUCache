#ifndef LFUCACHE_HPP
#define LFUCACHE_HPP

#include <list>
#include <cassert>
#include <map>

namespace cache
{

template <typename T, typename KeyT>
struct LFUCacheNode {
    using FreqT = size_t;

    KeyT key_;
    T data_;
    FreqT freq_;
};

template <typename T, typename KeyT>
std::ostream &operator<<(std::ostream &stream, const LFUCacheNode<T, KeyT> &cacheNode) {
    stream << cacheNode.key_;
    return stream;
}

template <typename T, typename KeyT>
std::ostream &operator<<(std::ostream &stream, const std::list<LFUCacheNode<T, KeyT>> &freqList) {
    for (auto to : freqList) {
        stream << to << " ";
    }
    return stream;
}

template <typename T, typename KeyT>
class LFUCache {
    using CacheListIt = typename std::list<LFUCacheNode<T, KeyT>>::iterator;
    using FreqT = typename LFUCacheNode<T, KeyT>::FreqT;
    
    std::unordered_map<KeyT, CacheListIt> hashTable_;
    std::map<FreqT, std::list<LFUCacheNode<T, KeyT>>> freqTable_;

    FreqT minFreq_ = 0;
    size_t size_ = 0;
    size_t capacity_ = 0;

private:
    bool full() const { return (size_ == capacity_); }
    
    void recomputeMinFreq() {
        if (freqTable_.empty()) { minFreq_ = 0; return; }
        minFreq_ = freqTable_.begin()->first;
    }
    
    void refreshKey(const KeyT &key) {
        assert(hashTable_.contains(key));
        
        CacheListIt cacheListIt = hashTable_[key];
        FreqT oldFreq = cacheListIt->freq_++;
    
        assert(freqTable_.contains(oldFreq));
        assert(!freqTable_[oldFreq].empty());
        assert(cacheListIt->freq_ != oldFreq);
    
        freqTable_[cacheListIt->freq_].splice(freqTable_[cacheListIt->freq_].end(), freqTable_[oldFreq], cacheListIt);
        if (freqTable_[oldFreq].empty() && oldFreq == minFreq_) {
            freqTable_.erase(oldFreq);
            recomputeMinFreq();
        }
    }

    void removeLFUNode() {
        assert(freqTable_.contains(minFreq_));

        CacheListIt LFUIt = freqTable_[minFreq_].begin();
    
        hashTable_.erase(LFUIt->key_);
        freqTable_[minFreq_].pop_front();
        size_--;
    
        if (freqTable_[minFreq_].empty()) {
            freqTable_.erase(minFreq_);
            recomputeMinFreq();
        }
    }    

public:
    LFUCache(const size_t capacity): capacity_(capacity) {}

    template <typename F>
    bool lookupUpdate(const KeyT &key, F slowGetPage) {
        if (capacity_ == 0) return false;

        assert(hashTable_.size() <= capacity_);
        assert(size_ <= capacity_);

        if (!hashTable_.contains(key)) {
            if (full()) removeLFUNode();

            freqTable_[0].push_back({key, slowGetPage(key), 0});
            hashTable_[key] = std::prev(freqTable_[0].end()); 
            minFreq_ = 0;
            size_++;

            return false;          
        }

        refreshKey(key);

        return true;
    }

    void print() const {
        std::cout << "LFU CACHE:\n";
        std::cout << "cap     : " << capacity_ << '\n';
        std::cout << "minFreq : " << minFreq_ << '\n';
        std::cout << "FREQ TABLE : \n";

        for (auto &[key, val] : freqTable_) {
            std::cout << key << " : " << val << '\n';
        }
        std::cout << '\n';
    }
};

}

#endif // LFUCACHE_HPP
