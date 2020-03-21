#ifndef UHOBGOBLIN_QAO_REGISTRY_HPP
#define UHOBGOBLIN_QAO_REGISTRY_HPP

#include <Hobgoblin/common.hpp>
#include <Hobgoblin/QAO/config.hpp>
#include <Hobgoblin/Utility/Slab_indexer.hpp>

#include <cstdint>
#include <memory>
#include <set>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

class QAO_Base;

namespace detail {

class QAO_Registry {
public:
    QAO_Registry(PZInteger capacity = 1);

    QAO_SerialIndexPair insert(std::unique_ptr<QAO_Base> ptr);
    QAO_SerialIndexPair insertNoOwn(QAO_Base* ptr);
    std::unique_ptr<QAO_Base> release(PZInteger index);
    void erase(PZInteger index);

    PZInteger size() const;
    QAO_Base* objectAt(PZInteger index) const;
    bool isObjectAtOwned(PZInteger index) const;
    std::int64_t serialAt(PZInteger index) const;
    PZInteger instanceCount() const;
    bool isSlotEmpty(PZInteger index) const;

private:
    struct Elem {
        std::int64_t serial;
        std::unique_ptr<QAO_Base> ptr;
        bool no_own = false;

        Elem() = default;

        Elem(const Elem&) = default;
        Elem& operator=(const Elem&) = default;

        Elem(Elem&&) = default;
        Elem& operator=(Elem&&) = default;

        ~Elem() {
            if (no_own) {
                ptr.release();
            }
        }
    };

    util::SlabIndexer _indexer;
    std::vector<Elem> _elements;
    std::int64_t _serial_counter;

    void adjustSize();
    std::int64_t nextSerial();
};

} // namespace detail
} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_QAO_REGISTRY_HPP