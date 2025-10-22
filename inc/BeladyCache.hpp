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
constexpr inline QueryIteration MAX_QUERY_ITERATION = std::numeric_limits<QueryIteration>::max();

template <typename T, typename KeyT>

class BeladyCache {
    using ListIt = typename std::list<T>::iterator;

    size_t capacity_ = 0;
    std::list<T> cache_;
    std::unordered_map<KeyT, ListIt> hashTable_;

    std::unordered_map<KeyT, std::queue<QueryIteration>> queryTable_; // queryTable_[key].front - actual next key query index
    std::priority_queue<std::pair<QueryIteration, KeyT>, std::vector<std::pair<QueryIteration, KeyT>>> keyQueue_;

private:
    bool valid() const { return cache_.size() <= capacity_ && hashTable_.size() <= capacity_; }
    bool full() const { return (cache_.size() == capacity_); }
    
    void updateQueryTable(const KeyT &key) {
        auto it = queryTable_.find(key);
        assert(it != queryTable_.end());
        
        std::queue<QueryIteration> &queryTableQueue = it->second;
    
        assert(!queryTableQueue.empty());
        queryTableQueue.pop();      
    }   

    QueryIteration getActualKeyNextQueryIteration(const KeyT &key) {
        assert(queryTable_.contains(key));
    
        std::queue<QueryIteration> &queryTableQueue = queryTable_.find(key)->second;
        assert(!queryTableQueue.empty());
       
        return queryTableQueue.front();
    }

    void refreshKey(const KeyT &key) {
        QueryIteration ActualKeyNextQueryIteration = getActualKeyNextQueryIteration(key);
        keyQueue_.push({ActualKeyNextQueryIteration, key});
    }

    const KeyT &getSubKey() {    
        while (!keyQueue_.empty()) {
            auto &top = keyQueue_.top();
            QueryIteration storedNext = top.first;
            const KeyT &subKey = top.second;
            
            if (storedNext == getActualKeyNextQueryIteration(subKey) && hashTable_.contains(subKey))
                return subKey;
            
            keyQueue_.pop();
        }

        assert(0 && "keyQueue_ doesn't contain valid values");
        
        const static KeyT defaultKey{};
        return defaultKey;
    }

public:
    template <typename IterT>
    requires std::same_as<typename std::iterator_traits<IterT>::value_type, KeyT>
    BeladyCache(const size_t capacity, const IterT beginIt, const IterT endIt): capacity_(capacity) 
    {
        QueryIteration i = 0;
        for (IterT it = beginIt; it != endIt; it++) {
            const KeyT &key = *it;
            if (!queryTable_.contains(key))
                queryTable_[key] = std::queue<QueryIteration>();
            queryTable_[key].push(i++);
        }

        for (auto &[key, container] : queryTable_) container.push(MAX_QUERY_ITERATION);
    }

    void printCache(std::priority_queue<std::pair<QueryIteration, KeyT>, std::vector<std::pair<QueryIteration, KeyT>>> keyQueue) const {
        std::cout << "cache : ";
        while (keyQueue.size()) {
            std::cout << keyQueue.top().second << " ";
            keyQueue.pop();
        }
        std::cout << '\n';
    }

    template <typename F>
    bool lookupUpdate(const KeyT &key, F slowGetPage) {
        if (capacity_ == 0) return false;
        assert(valid());

        updateQueryTable(key);

        if (!hashTable_.contains(key)) {
            if (full()) {
                const KeyT &subKey = getSubKey();
                if (getActualKeyNextQueryIteration(subKey) <= getActualKeyNextQueryIteration(key))
                    return false;

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


#endif // Belady_CACHE_HPPs