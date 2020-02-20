#ifndef SLAB_INDEXER_HPP
#define SLAB_INDEXER_HPP

#include <Hobgoblin/common.hpp>

#include <cassert>
#include <vector>
#include <stdexcept>

#include <Hobgoblin/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace util {

class SlabIndexer { 
// TODO Separate into .hpp/.cpp
// TODO Use std::size_t where appropriate
// TODO This class is a mess
public:
    // Construction
    SlabIndexer(PZInteger capacity = 1u);

    SlabIndexer(const SlabIndexer& other);
    SlabIndexer(SlabIndexer&& other);

    SlabIndexer& operator=(const SlabIndexer & other);
    SlabIndexer& operator=(SlabIndexer && other);

    // Main
    std::size_t acquire();
    bool tryAcquireSpecific(PZInteger slot);
    void free(PZInteger slot);

    // Utility
    bool isSlotEmpty(PZInteger slot) const;
    void resize(PZInteger new_size);
    void markAllAsEmpty();
    void reserve(PZInteger _size);
    void resizeToMin();
    void shrinkToFit();

    std::size_t countEmpty() const;
    std::size_t countFilled() const;
    std::size_t getCapacity() const;
    std::size_t getSize() const;

private:
    constexpr static std::size_t NULL_INDEX = std::size_t(-1);

    struct ListControlBlock {
        std::size_t head;
        std::size_t cnt;
    };

    struct Elem {
        std::size_t prev;
        std::size_t next;
        bool empty;

        Elem(bool empty = true, std::size_t prev = NULL_INDEX, std::size_t next = NULL_INDEX)
            : prev{prev}
            , next{next}
            , empty{empty}
        {
        }
    };

    ListControlBlock _empty_list;
    ListControlBlock _filled_list;
    std::vector<Elem> _elements;

    void upsize(std::size_t new_size);
    void downsize(std::size_t new_size);

    bool isListEmpty(const ListControlBlock& lcb) const;
    void resetList(ListControlBlock& lcb);
    void pushOntoList(ListControlBlock& lcb, std::size_t element_index);
    void removeFromList(ListControlBlock& lcb, std::size_t element_index);

    // Convenience methods
    void resetEmptyList();
    void resetFilledList();
    void pushOntoEmptyList(std::size_t element_index);
    void pushOntoFilledList(std::size_t element_index);
    void removeFromEmptyList(std::size_t element_index);
    void removeFromFilledList(std::size_t element_index);

};

// PUBLIC METHOD IMPLEMENTATION: //////////////////////////////////////////
inline
SlabIndexer::SlabIndexer(PZInteger capacity)
    : _elements{}
{
    if (capacity < 1) capacity = 1;

    _elements.resize(capacity);

    resetEmptyList();
    resetFilledList();

    PZInteger i = capacity;
    do {
        
        i -= 1;

        Elem* elem = &(_elements[i]);
        elem->empty = true;

        pushOntoEmptyList(i);

    } while (i > 0);
    
}

inline
SlabIndexer::SlabIndexer(const SlabIndexer& other)
    : _empty_list{other._empty_list}
    , _filled_list{other._filled_list}
    , _elements{other._elements}
{    
}

inline
SlabIndexer::SlabIndexer(SlabIndexer && other)
    : _empty_list{other._empty_list}
    , _filled_list{other._filled_list}
    , _elements{std::move(other._elements)}
{    
    other.resetEmptyList();
    other.resetFilledList();
    other._elements.resize(1u);
    other._elements[0].empty = true;
    other.pushOntoEmptyList(0u);
}

inline
SlabIndexer& SlabIndexer::operator=(const SlabIndexer & other) {
    if (&other != this) {
        _empty_list  = other._empty_list;
        _filled_list = other._filled_list;
        _elements   = other._elements;
    }
    return *this;
}

inline
SlabIndexer& SlabIndexer::operator=(SlabIndexer && other) {
    if (&other != this) {
        _empty_list  = other._empty_list;
        _filled_list = other._filled_list;
        _elements   = std::move(other._elements);

        other.resetEmptyList();
        other.resetFilledList();
        other._elements.resize(1u);
        other._elements[0].empty = true;
        other.pushOntoEmptyList(0u);
    }
    return *this;
}

inline
std::size_t SlabIndexer::acquire() {
    
    if (isListEmpty(_empty_list)) {
        resize(_elements.size() + 1u);
    }

    std::size_t retVal = _empty_list.head;

    Elem* elem = &(_elements[retVal]);

    elem->empty = false;

    removeFromEmptyList(retVal);
    pushOntoFilledList(retVal);

    return retVal;

}

inline
bool SlabIndexer::tryAcquireSpecific(PZInteger slot) {
        
    if (slot >= static_cast<PZInteger>(_elements.size())) resize(slot + 1);

    if (!isSlotEmpty(slot)) {
        return false;
    }

    Elem* elem = &(_elements[slot]);

    elem->empty = false;

    removeFromEmptyList(slot);
    pushOntoFilledList(slot);

    return true;

}

inline
void SlabIndexer::free(PZInteger slot) {
        
    if (isSlotEmpty(slot)) {
        throw std::logic_error(CURRENT_FUNCTION ": Slot is already empty!");
    }

    Elem* elem = &(_elements[slot]);

    elem->empty = true;

    removeFromFilledList(slot);
    pushOntoEmptyList(slot);

}

inline
bool SlabIndexer::isSlotEmpty(PZInteger slot) const {
    return (_elements.at(slot).empty);
}

inline
void SlabIndexer::resize(PZInteger new_size) {
        
    if (new_size < 1) new_size = 1;

    if (new_size > static_cast<PZInteger>(_elements.size())) {
        upsize(new_size);
    }
    else if (new_size < static_cast<PZInteger>(_elements.size())) {
        downsize(new_size);
    }

}

inline
void SlabIndexer::markAllAsEmpty() {
    std::size_t sz = getSize();
    for (std::size_t i = 0u; i < sz; i += 1u) {
        
        Elem* elem = &(_elements[i]);

        if (!elem->empty) {

            elem->empty = true;

            removeFromFilledList(i);
            pushOntoEmptyList(i);

        }

    }
}

inline
void SlabIndexer::reserve(PZInteger _size) {
    _elements.reserve(_size);
}

inline
void SlabIndexer::resizeToMin() {
    resize(1u);
}

inline
void SlabIndexer::shrinkToFit() {
    _elements.shrink_to_fit();
}

inline
std::size_t SlabIndexer::countEmpty() const {
    return _empty_list.cnt;
}

inline
std::size_t SlabIndexer::countFilled() const {
    return _filled_list.cnt;
}

inline
std::size_t SlabIndexer::getCapacity() const {
    return _elements.capacity();
}

inline
std::size_t SlabIndexer::getSize() const {
    return _elements.size();
}

// PRIVATE METHOD IMPLEMENTATION: /////////////////////////////////////////
inline
bool SlabIndexer::isListEmpty(const SlabIndexer::ListControlBlock& lcb) const {
    
    // Either both are true or none are, else: error!
    assert( ((lcb.head == NULL_INDEX) ^ (lcb.cnt == 0u)) == false );

    return (lcb.cnt == 0u);

}

inline
void SlabIndexer::resetList(SlabIndexer::ListControlBlock& lcb) {
    lcb.head = NULL_INDEX;
    lcb.cnt  = 0u;
}

inline
void SlabIndexer::pushOntoList(SlabIndexer::ListControlBlock& lcb, std::size_t element_index) {
    
    Elem* elem = &(_elements[element_index]);

    if (isListEmpty(lcb)) {
        
        elem->next = NULL_INDEX;
        elem->prev = NULL_INDEX;

        lcb.head = element_index;
        lcb.cnt  = 1u;

    }
    else {
        
        Elem* oldHead = &(_elements[lcb.head]);

        oldHead->prev = element_index;
        elem->next = lcb.head;
        elem->prev = NULL_INDEX;

        lcb.head = element_index;
        lcb.cnt += 1u;

    }

}

inline
void SlabIndexer::removeFromList(SlabIndexer::ListControlBlock& lcb, std::size_t element_index) {
    
    if (lcb.cnt == 1u) { // Last element of list
        resetList(lcb);
        return;
    }

    Elem* elem = &(_elements[element_index]);

    if (elem->prev == NULL_INDEX) { // Head of list
        
        Elem* elemAfter = &(_elements[elem->next]);

        elemAfter->prev = NULL_INDEX;

        lcb.head = elem->next;

    }
    else if (elem->next == NULL_INDEX) {
        
        Elem* elemBefore = &(_elements[elem->prev]);

        elemBefore->next = NULL_INDEX;

    }
    else {

        Elem* elemBefore = &(_elements[elem->prev]);
        Elem* elemAfter  = &(_elements[elem->next]);

        elemBefore->next = elem->next;
        elemAfter->prev  = elem->prev;
        
    }

    lcb.cnt -= 1u;

}

inline
void SlabIndexer::resetEmptyList() {
    resetList(_empty_list);
}

inline
void SlabIndexer::resetFilledList() {
    resetList(_filled_list);
}

inline
void SlabIndexer::pushOntoEmptyList(std::size_t element_index) {
    pushOntoList(_empty_list, element_index);
}

inline
void SlabIndexer::pushOntoFilledList(std::size_t element_index) {
    pushOntoList(_filled_list, element_index);
}

inline
void SlabIndexer::removeFromEmptyList(std::size_t element_index) {
    removeFromList(_empty_list, element_index);
}

inline
void SlabIndexer::removeFromFilledList(std::size_t element_index) {
    removeFromList(_filled_list, element_index);
}

inline
void SlabIndexer::upsize(std::size_t new_size) {
    
    std::size_t oldSize = _elements.size();

    _elements.resize(new_size);

    for (unsigned i = oldSize; i < new_size; i += 1u) {
                
        _elements[i].empty = true;

        pushOntoEmptyList(i);

    }

}

inline
void SlabIndexer::downsize(std::size_t new_size) {
        
    std::size_t oldSize = _elements.size();

    for (unsigned i = oldSize - 1u; i >= new_size; i -= 1u) {
                
        if (_elements[i].empty) {
            removeFromEmptyList(i);
        }
        else {
            _elements.resize(i + 1u);
            return;
        }

    }

    _elements.resize(new_size);

}

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/pmacro_undef.hpp>

#endif // !SLAB_INDEXER_HPP