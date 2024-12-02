// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_VALUE_SORTED_MAP_HPP
#define UHOBGOBLIN_UTIL_VALUE_SORTED_MAP_HPP

#include <Hobgoblin/HGExcept.hpp>

#include <iterator>
#include <set>
#include <unordered_map>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

//! ValueSortedMap is an associative key->value container where the elements
//! are ordered based on the values rather than the keys.
//! TODO: finish description
template <class taKey,
          class taValue,
          class taKeyHash   = std::hash<taKey>,
          class taKeyEqual  = std::equal_to<taKey>,
          class taValueLess = std::less<taValue>>
class ValueSortedMap {
public:
    using key_type        = taKey;
    using mapped_type     = taValue;
    using value_type      = std::pair<const taKey, taValue>;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

private:
    struct value_type_second_less {
        bool operator()(const value_type& aLhs, const value_type& aRhs) const {
            return taValueLess{}(aLhs.second, aRhs.second);
        }
    };

    using Set = std::multiset<value_type, value_type_second_less>;
    using Map = std::unordered_map<taKey, typename Set::iterator, taKeyHash, taKeyEqual>;

public:
    class Iterator {
    public:
        using value_type        = typename ValueSortedMap::value_type;
        using difference_type   = typename ValueSortedMap::difference_type;
        using reference         = typename ValueSortedMap::reference;
        using pointer           = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;

        Iterator(const Iterator&)            = default;
        Iterator& operator=(const Iterator&) = default;

        bool operator==(const Iterator& aRhs) const {
            return _iter == aRhs._iter;
        }

        bool operator!=(const Iterator& aRhs) const {
            return _iter != aRhs._iter;
        }

        // bool operator<(const iterator&) const; //optional
        // bool operator>(const iterator&) const; //optional
        // bool operator<=(const iterator&) const; //optional
        // bool operator>=(const iterator&) const; //optional

        Iterator& operator++() {
            ++_iter;
            return SELF;
        }

        Iterator& operator++(int) {
            Iterator old = SELF;
            operator++();
            return old;
        }

        Iterator& operator--() {
            --_iter;
            return SELF;
        }

        Iterator& operator--(int) {
            Iterator old = SELF;
            operator--();
            return old;
        }

        // iterator& operator+=(size_type); //optional
        Iterator operator+(size_type aOffset) const {
            Iterator result = SELF;
            while (aOffset > 0) {
                ++result;
                aOffset -= 1;
            }
            return result;
        }
        // friend iterator operator+(size_type, const iterator&); //optional
        // iterator& operator-=(size_type); //optional
        // iterator operator-(size_type) const; //optional
        // difference_type operator-(iterator) const; //optional

        const value_type& operator*() const {
            return *_iter;
        }

        pointer operator->() const {
            return _iter.operator->();
        }

    private:
        friend class ValueSortedMap;

        Iterator(Set::iterator aIter)
            : _iter{aIter} {}

        Set::iterator _iter;
    };

    using iterator               = const Iterator;
    using const_iterator         = const Iterator;
    using reverse_iterator       = typename std::reverse_iterator<iterator>;
    using const_reverse_iterator = typename std::reverse_iterator<const_iterator>;

    std::pair<iterator, bool> insert(const value_type& aValue) {
        const auto& key = aValue.first;

        if (_map.contains(key)) {
            return std::make_pair(end(), false);
        }

        const auto setIter = _set.insert(value_type{key, aValue.second});
        _map[key]          = setIter;

        return std::make_pair(Iterator{setIter}, true);
    }

    iterator find(const key_type& aKey) {
        const auto iter = _map.find(aKey);
        if (iter == _map.end()) {
            return end();
        }

        return Iterator{iter->second};
    }

    iterator erase(iterator aPos) {
        HG_ASSERT(aPos != end());

        const auto key  = aPos._iter->first;
        auto       next = _set.erase(aPos._iter);
        _map.erase(key);

        return Iterator{next};
    }

    size_type size() const {
        return _map.size();
    }

    bool empty() const {
        return _map.empty();
    }

    void clear() {
        _map.clear();
        _set.clear();
    }

    iterator begin() {
        return Iterator{_set.begin()};
    }

    iterator end() {
        return Iterator{_set.end()};
    }

private:
    Set _set;
    Map _map;
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_VALUE_SORTED_MAP_HPP
