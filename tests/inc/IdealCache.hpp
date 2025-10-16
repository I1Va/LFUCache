#ifndef IDEAL_CACHE_HPP
#define IDEAL_CACHE_HPP

#include <list>
#include <unordered_map>
#include <vector>
#include <climits>
#include <algorithm>
#include <iostream>


namespace test
{

using QueryIteration = size_t;

template <typename T, typename KeyT = int>
class IdealCache {
    using ListIt = typename std::list<T>::iterator;

    size_t size_ = 0;
    std::list<T> cache_;
    std::unordered_map<KeyT, ListIt> hashTable_;
    std::unordered_map<KeyT, std::vector<QueryIteration>> queryTable;

private:
    bool full() const { return (cache_.size() == size_); }
    
    void refreshCachePos(const ListIt pos) {
        return;
    }

    QueryIteration getKeyClosestQueryIteration(const KeyT key, QueryIteration passedIterations) {
        assert(queryTable.find(key) != queryTable.end());

        auto it = std::upper_bound(queryTable[key].begin(), queryTable[key].end(), passedIterations);
        if (it == queryTable[key].end()) {
            return INT_MAX;
        } else {
            return *it;
        }
    }

    KeyT getSubstitutionKey() {
        assert(cache_.size());

        static QueryIteration passedQueryIterations = 0;

        std::pair<KeyT, QueryIteration> lessUsedKey = 
            {hashTable_.begin()->first, getKeyClosestQueryIteration(hashTable_.begin()->first, passedQueryIterations)};
    
        for (const auto& [key, value] : hashTable_) {
            int keyClosestQueryIteration = getKeyClosestQueryIteration(key, passedQueryIterations);
            if (keyClosestQueryIteration >= lessUsedKey.second) {
                lessUsedKey = {key, keyClosestQueryIteration};
            }
        }

        passedQueryIterations++;

        assert(hashTable_.find(lessUsedKey.first) != hashTable_.end());
        return lessUsedKey.first;
    }

public:
    IdealCache(const size_t size, const std::vector<KeyT> &queries): size_(size) {
        for (QueryIteration i = 0; i < queries.size(); i++) {
            if (queryTable.find(queries[i]) != queryTable.end()) {
                queryTable[queries[i]].push_back(i);
            } else {
                queryTable[queries[i]] = std::vector<QueryIteration>();
            }
        }
    }

    template <typename F>
    bool lookupUpdate(KeyT key, F slowGetPage) {
        assert(hashTable_.size() <= size_);

        auto hit = hashTable_.find(key);
        if (hit == hashTable_.end()) {
            if (!full()) {
                cache_.push_front(slowGetPage(key));
                hashTable_[key] = cache_.begin();
            } else {
                KeyT subKey = getSubstitutionKey();
                ListIt subPos = hashTable_[subKey];
            
                *subPos = slowGetPage(key); 
                hashTable_.erase(subKey);

                hashTable_[key] = subPos;
            }
        
            return false;          
        }

        ListIt hitPos = hit->second;
        refreshCachePos(hitPos);
        
        return true;
    }
};


} // namespace test


#endif // IDEAL_CACHE_HPP