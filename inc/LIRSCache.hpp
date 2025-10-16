#ifndef LIRSCACHE_H
#define LIRSCACHE_H

#include <list>
#include <unordered_map>

namespace Cache
{

template <typename T, typename KeyT = int>
class Cache {
    using ListIt = typename std::list<T>::iterator;

    size_t size_ = 0;
    std::list<T> cache_;
    std::unordered_map<KeyT, ListIt> hashTable_;

private:
    bool full() const { return (cache_.size() == size_); }
    
    void refreshCachePos(const ListIt pos) {
        return;
    }

    ListIt getSubstitutionPos() {
        assert(cache_.size());

        return cache_.begin();
    }

public:
    Cache(const size_t size): size_(size) {}

    template <typename F>
    bool lookupUpdate(KeyT key, F slowGetPage) {
    auto hit = hashTable_.find(key);
    if (hit == hashTable_.end()) {
        if (!full()) {
            cache_.push_front(slowGetPage(key));
            hashTable_[key] = cache_.begin();
        } else {
            ListIt pos = getSubstitutionPos();
            *pos = slowGetPage(key); 
            hashTable_[key] = pos;
        }
        return false;          
    }
        ListIt hitPos = hit->second;
        refreshCachePos(hitPos);
        
        return true;
    }
};

}


#endif // LIRSCACHE_H