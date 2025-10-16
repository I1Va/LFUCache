#ifndef IDEAL_CACHE_HPP
#define IDEAL_CACHE_HPP

#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <climits>
#include <algorithm>
#include <iostream>
#include <queue>

namespace test
{

using QueryIteration = int;
const QueryIteration MAX_QUERY_ITERATION = std::numeric_limits<QueryIteration>::max();

template <typename T, typename KeyT = int>

class IdealCache {
    using ListIt = typename std::list<T>::iterator;

    QueryIteration currentQueryIteration_ = -1;
    size_t size_ = 0;
    std::list<T> cache_;
    std::unordered_map<KeyT, ListIt> hashTable_;

    std::unordered_map<KeyT, std::queue<QueryIteration>> queryTable_; // queryTable_[key].front - actual next key query index
    std::priority_queue<std::pair<QueryIteration, KeyT>, std::vector<std::pair<QueryIteration, KeyT>>> keyQueue_;

private:
    bool valid() const {
        return cache_.size() <= size_ && hashTable_.size() <= size_;
    }

    bool full() const { return (cache_.size() == size_); }
    
    void updateQueryTable(const KeyT key, const QueryIteration currentQueryIteration) {
        auto it = queryTable_.find(key);
        assert(it != queryTable_.end());
        
        std::queue<QueryIteration> &queryTableQueue = it->second;
        queryTableQueue.pop();      
    }   

    QueryIteration getActualKeyNextQueryIteration(const KeyT key) {
        assert(queryTable_.find(key) != queryTable_.end());
        std::queue<QueryIteration> queryTableQueue = queryTable_.find(key)->second;
        assert(!queryTableQueue.empty());
       
        return queryTableQueue.front();
    }

    void refreshKey(const KeyT key) {
        QueryIteration ActualKeyNextQueryIteration = getActualKeyNextQueryIteration(key);
        keyQueue_.push({ActualKeyNextQueryIteration, key});
    }


    KeyT popSubstitutionKey() {
        while (!keyQueue_.empty()) {
            auto top = keyQueue_.top();
            QueryIteration storedNext = top.first;
            KeyT key = top.second;
            keyQueue_.pop();
            if (storedNext == getActualKeyNextQueryIteration(key)) {
                return key;
            }
        }
        assert(0 && "cache queue doesn't contain valid values");
        return 0;
    }

public:
    IdealCache(const size_t size, const std::vector<KeyT> &queries): size_(size) {
        
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
                queryTable_[queries[i]].push(MAX_QUERY_ITERATION);
            }
        }
    }

    template <typename F>
    bool lookupUpdate(KeyT key, F slowGetPage) {
        assert(valid());

        currentQueryIteration_++;
        updateQueryTable(key, currentQueryIteration_);

        auto hit = hashTable_.find(key);
    
        if (hit == hashTable_.end()) {
            if (!full()) {
                cache_.push_front(slowGetPage(key));
                hashTable_[key] = cache_.begin();
            } else {
                KeyT subKey = popSubstitutionKey();
                ListIt subPos = hashTable_[subKey];
            
                *subPos = slowGetPage(key);

                hashTable_.erase(subKey);
                hashTable_[key] = subPos;
            }
            keyQueue_.push({getActualKeyNextQueryIteration(key), key});

            return false;          
        }    
        
        refreshKey(key);
        return true;
    }
};


} // namespace test


#endif // IDEAL_CACHE_HPP