#ifndef UHOBGOBLIN_UTIL_QTREE_COLLISION_DOMAIN_HPP
#define UHOBGOBLIN_UTIL_QTREE_COLLISION_DOMAIN_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/NoCopyNoMove.hpp>
#include <Hobgoblin/Utility/Rectangle.hpp>

#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace util {

namespace detail {

class CollisionEntity {
public:
    using BoundingBox = Rectangle<double>;

    BoundingBox bbox;
    std::int32_t groupMask;
    PZInteger index;

    bool collidesWith(const CollisionEntity& other) const {
        return (((groupMask & other.groupMask) != 0) && bbox.overlaps(other.bbox));
    }
};

class QuadTreeNode;

} // namespace detail

using CollisionPair = std::pair<PZInteger, PZInteger>;

class QuadTreeCollisionDomain : NO_COPY, NO_MOVE {
public:
    using Entity = detail::CollisionEntity;
    using BoundingBox = Entity::BoundingBox;

    class EntityHandle {
    public:
        ~EntityHandle();
        void reset();
        void move();

    private:
        QuadTreeCollisionDomain& _domain;
    };

    const size_t MAX_DEPTH;
    const size_t MAX_OBJECTS;

    QuadTreeCollisionDomain() = delete;
    QuadTreeCollisionDomain(double w, double h, PZInteger maxDepth, PZInteger maxObjectsPerNode,
                            PZInteger workerThreadsCount = 0);
    ~QuadTreeCollisionDomain();

    // Main functionality:
    void clear();

    EntityHandle insertEntity(PZInteger entitySlabIndex, const BoundingBox& bbox);
    //void updateEntity(PZInteger entitySlabIndex, const BoundingBox& bb);
    //bool entityExists(PZInteger entitySlabIndex) const;
    //void removeEntity(PZInteger entitySlabIndex);

    PZInteger recalcPairs();
    void recalcPairsStart();
    PZInteger recalcPairsJoin();
    //bool pairs_next(GenericPtr& inst1, GenericPtr& inst2);

    //// Scanning - Point:
    //GenericPtr scan_point_one(GroupMask groups,
    //                          double x, double y) const;

    //void scan_point_vector(GroupMask groups,
    //                       double x, double y, std::vector<GenericPtr>& vec) const;

    //// Scanning - Rectangle:
    //GenericPtr scan_rect_one(GroupMask groups, bool must_envelop,
    //                         double x, double y, double w, double h) const;

    //void scan_point_vector(GroupMask groups, bool must_envelop,
    //                       double x, double y, double w, double h, std::vector<GenericPtr>& vec) const;

    //// Scanning - Circle:
    //GenericPtr scan_circle_one(GroupMask groups, bool must_envelop,
    //                           double x, double y, double r) const;

    //void scan_circle_vector(GroupMask groups, bool must_envelop,
    //                        double x, double y, double r, std::vector<GenericPtr>& vec) const;

private:
    using EntityList = std::list<Entity>;

    //friend void worker_body(MTQuadTreeDomain*, size_t, int*);

    double _width, _height;
    double _minWidth, _minHeight;
    //size_t nt_size;

    std::unique_ptr<detail::QuadTreeNode> _rootNode;
    std::vector<std::vector<detail::QuadTreeNode*>> _nodeTable; // [x][y]

    std::vector<std::optional<EntityList::iterator>> _entitiyIterators;

    //std::vector<MTQuadTreeNode*> cleanup_stack;

    //PairsContainer pairs_deq[4];

    //size_t pairs_hand;
    //size_t deq_selector;

    //std::thread worker[4];
    //int         status[4];

    ///*util::Semaphore wsem[4];
    //util::Semaphore dsem;*/

    //bool domain_locked;

    //QuadTreeEntity& get_entity(size_t index) const;
    //void node_table_update(MTQuadTreeNode* node);
    //void clean_up();

};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_QTREE_COLLISION_DOMAIN_HPP