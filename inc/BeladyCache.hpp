#ifndef BELADY_CACHE_HPP
#define BELADY_CACHE_HPP

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <list>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cache {

using QueryIteration = int;
constexpr inline QueryIteration MAX_QUERY_ITERATION =
    std::numeric_limits<QueryIteration>::max();

template <typename DataT, typename KeyT, typename Hash = std::hash<KeyT>,
          typename Eq = std::equal_to<KeyT>>
class BeladyCache {
    using ListIt = typename std::list<DataT>::iterator;

    std::unordered_set<KeyT, Hash, Eq> keyStorage_;

    size_t capacity_ = 0;
    std::list<DataT> cache_;

    std::unordered_map<const KeyT *, ListIt> hashTable_;
    std::unordered_map<const KeyT *, std::queue<QueryIteration>>
        queryTable_; // queryTable_[key].front - actual next key query index
    std::priority_queue<std::pair<QueryIteration, const KeyT *>> keyQueue_;

  private:
    bool valid() const {
        return cache_.size() <= capacity_ && hashTable_.size() <= capacity_;
    }
    bool full() const { return (cache_.size() == capacity_); }

    const KeyT *getKeyPtr(const KeyT &key) {
        auto [it, _] = keyStorage_.insert(key);
        return &*it;
    }

    void updateQueryTable(const KeyT *key) {
        auto it = queryTable_.find(key);
        assert(it != queryTable_.end());

        std::queue<QueryIteration> &queryTableQueue = it->second;

        assert(!queryTableQueue.empty());
        queryTableQueue.pop();
    }

    QueryIteration getActualKeyNextQueryIteration(const KeyT *key) {
        assert(queryTable_.contains(key));

        std::queue<QueryIteration> &queryTableQueue =
            queryTable_.find(key)->second;
        assert(!queryTableQueue.empty());

        return queryTableQueue.front();
    }

    void refreshKey(const KeyT *key) {
        QueryIteration ActualKeyNextQueryIteration =
            getActualKeyNextQueryIteration(key);
        keyQueue_.push({ActualKeyNextQueryIteration, key});
    }

    const KeyT *getSubKey() {
        while (!keyQueue_.empty()) {
            auto &top = keyQueue_.top();
            QueryIteration storedNext = top.first;
            const KeyT *subKey = top.second;

            if (storedNext == getActualKeyNextQueryIteration(subKey) &&
                hashTable_.contains(subKey))
                return subKey;

            keyQueue_.pop();
        }

        assert(0 && "keyQueue_ doesn't contain valid values");

        return nullptr;
    }

  public:
    template <typename IterT>
        requires std::same_as<typename std::iterator_traits<IterT>::value_type,
                              KeyT>
    BeladyCache(const size_t capacity, const IterT beginIt, const IterT endIt)
        : capacity_(capacity) {
        QueryIteration i = 0;
        for (IterT it = beginIt; it != endIt; it++) {
            const KeyT *key = getKeyPtr(*it);
            if (!queryTable_.contains(key))
                queryTable_[key] = std::queue<QueryIteration>();
            queryTable_[key].push(i++);
        }

        for (auto &[key, container] : queryTable_)
            container.push(MAX_QUERY_ITERATION);
    }

    void printCache(
        std::priority_queue<std::pair<QueryIteration, KeyT>> keyQueue) const {
        std::cout << "cache : ";
        while (keyQueue_.size()) {
            std::cout << keyQueue.top().second << " ";
            keyQueue.pop();
        }
        std::cout << '\n';
    }

    template <typename F> bool lookupUpdate(const KeyT &key, F slowGetPage) {
        if (capacity_ == 0)
            return false;
        assert(valid());

        const KeyT *keyPtr = getKeyPtr(key);

        updateQueryTable(keyPtr);

        if (!hashTable_.contains(keyPtr)) {
            if (full()) {
                const KeyT *subKey = getSubKey();
                if (getActualKeyNextQueryIteration(subKey) <=
                    getActualKeyNextQueryIteration(keyPtr))
                    return false;

                ListIt subPos = hashTable_[subKey];
                *subPos = slowGetPage(key);
                hashTable_.erase(subKey);
                hashTable_[keyPtr] = subPos;
            } else {
                cache_.push_front(slowGetPage(key));
                hashTable_[keyPtr] = cache_.begin();
            }

            keyQueue_.push({getActualKeyNextQueryIteration(keyPtr), keyPtr});
            return false;
        }

        refreshKey(keyPtr);
        return true;
    }
};

} // namespace cache

#endif // Belady_CACHE_HPP