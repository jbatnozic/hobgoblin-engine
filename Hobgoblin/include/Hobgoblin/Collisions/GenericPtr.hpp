#pragma once

#include <cstdlib> // za size_t

namespace qao {

    class Base;   
    typedef size_t Index;
    typedef size_t Uid;

    class GenericPtr {

    private:

        Index m_index;
        Uid   m_uid;

    public:

        // Construction:
        GenericPtr()
            : m_index(0)
            , m_uid(Uid(0)) {

        }
        
        GenericPtr(Index index_, Uid uid_)
            : m_index(index_)
            , m_uid(uid_) {

        }

        GenericPtr(std::nullptr_t p)
            : GenericPtr() {

        }

        // Comparison:
        bool operator==(const GenericPtr &other) const {
    
            return (m_index == other.m_index && m_uid == other.m_uid);

        }
        
        bool operator!=(const GenericPtr &other) const {

            return (m_index != other.m_index || m_uid != other.m_uid);

        }

        // Copy:
        GenericPtr(const GenericPtr & other) = default;
        GenericPtr& operator=(const GenericPtr & other) = default;

        // Move:
        GenericPtr(GenericPtr && other) = default;
        GenericPtr& operator=(GenericPtr && other) = default;

        // Utility:
        Index index() const { return m_index; }
        
        Uid uid() const { return m_uid; }

        bool is_null() const {
            
            return (m_uid == 0);

            }

    };
        
}