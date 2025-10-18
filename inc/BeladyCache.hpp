#ifndef BELADY_CACHE_HPP
#define BELADY_CACHE_HPP

#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cassert>
#include <limits>
#include <algorithm>
#include <iostream>
#include <queue>

namespace cache
{

using QueryIteration = int;
const QueryIteration MAX_QUERY_ITERATION = std::numeric_limits<QueryIteration>::max();

template <typename T, typename KeyT = int>

class BeladyCache {
    using ListIt = typename std::list<T>::iterator;

    size_t capacity_ = 0;
    std::list<T> cache_;
    std::unordered_map<KeyT, ListIt> hashTable_;

    std::unordered_map<KeyT, std::queue<QueryIteration>> queryTable_; // queryTable_[key].front - actual next key query index
    std::priority_queue<std::pair<QueryIteration, KeyT>, std::vector<std::pair<QueryIteration, KeyT>>> keyQueue_;

private:
    bool valid() const {
        return cache_.size() <= capacity_ && hashTable_.size() <= capacity_;
    }

    bool full() const { return (cache_.size() == capacity_); }
    
    void updateQueryTable(const KeyT key) {
        auto it = queryTable_.find(key);
        assert(it != queryTable_.end());
        
        std::queue<QueryIteration> &queryTableQueue = it->second;
    
        assert(!queryTableQueue.empty());
        queryTableQueue.pop();      
    }   

    QueryIteration getActualKeyNextQueryIteration(const KeyT key) {
        assert(queryTable_.find(key) != queryTable_.end());
        std::queue<QueryIteration> &queryTableQueue = queryTable_.find(key)->second;
        assert(!queryTableQueue.empty());
       
        return queryTableQueue.front();
    }

    void refreshKey(const KeyT key) {
        QueryIteration ActualKeyNextQueryIteration = getActualKeyNextQueryIteration(key);
        keyQueue_.push({ActualKeyNextQueryIteration, key});
    }


    KeyT getSubstitutionKey() {
        while (!keyQueue_.empty()) {
            auto top = keyQueue_.top();
            QueryIteration storedNext = top.first;
            KeyT key = top.second;
            
            if (storedNext == getActualKeyNextQueryIteration(key) && hashTable_.find(key) != hashTable_.end()) {
                return key;
            }
            keyQueue_.pop();
        }
        assert(0 && "keyQueue_ doesn't contain valid values");
        return 0;
    }

public:
    BeladyCache(const size_t capacity, const std::vector<KeyT> &queries): capacity_(capacity) {
        for (QueryIteration i = 0; i < queries.size(); i++) {
            if (queryTable_.find(queries[i]) == queryTable_.end())
                queryTable_[queries[i]] = std::queue<QueryIteration>();
            queryTable_[queries[i]].push(i);
        }

        std::unordered_set<KeyT> used;
        for (QueryIteration i = 0; i < queries.size(); i++) {
            KeyT key = queries[i];
            if (used.find(key) == used.end()) {
                used.insert(key);
                queryTable_[key].push(MAX_QUERY_ITERATION);
            }
        }
    }

    void printCache(std::priority_queue<std::pair<QueryIteration, KeyT>, std::vector<std::pair<QueryIteration, KeyT>>> keyQueue) {
        std::cout << "cache : ";
        while (keyQueue.size()) {
            std::cout << keyQueue.top().second << " ";
            keyQueue.pop();
        }
        std::cout << "\n";
    }

    template <typename F>
    bool lookupUpdate(KeyT key, F slowGetPage) {
        if (capacity_ == 0) return false;
        assert(valid());

        updateQueryTable(key);
        auto hit = hashTable_.find(key);
        
        if (hit == hashTable_.end()) {
            // Miss
          
            if (full()) {
                KeyT subKey = getSubstitutionKey();
                if (getActualKeyNextQueryIteration(subKey) <= getActualKeyNextQueryIteration(key)) {
                    return false;
                }
                
                
                ListIt subPos = hashTable_[subKey];
                *subPos = slowGetPage(key);
                hashTable_.erase(subKey);
                hashTable_[key] = subPos;
            } else {
                cache_.push_front(slowGetPage(key));
                hashTable_[key] = cache_.begin();
            }

            
            keyQueue_.push({getActualKeyNextQueryIteration(key), key});            
            return false;
        }
    
        refreshKey(key);
        return true;
    }
};

} // namespace test


#endif // Belady_CACHE_HPP