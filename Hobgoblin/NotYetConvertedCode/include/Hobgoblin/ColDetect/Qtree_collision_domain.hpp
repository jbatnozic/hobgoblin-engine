#ifndef UHOBGOBLIN_CD_QTREE_COLLISION_DOMAIN_HPP
#define UHOBGOBLIN_CD_QTREE_COLLISION_DOMAIN_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/ColDetect/Common.hpp>
#include <Hobgoblin/Utility/No_copy_no_move.hpp>
#include <Hobgoblin/Utility/Rectangle.hpp>
#include <Hobgoblin/Utility/Semaphore.hpp>

#include <SFML/Graphics.hpp> // TODO Temp.

#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace cd {

namespace detail {

class QuadTreeNode;

class QuadTreeCollisionEntity {
public:
    BoundingBox bbox;
    QuadTreeNode* holder;
    std::int32_t groupMask;
    EntityTag tag;

    bool collidesWith(const QuadTreeCollisionEntity& other) const {
        return (((groupMask & other.groupMask) != 0) && bbox.overlaps(other.bbox));
    }
};

} // namespace detail

class QuadTreeCollisionDomain : NO_COPY, NO_MOVE {
public:
    using Entity = detail::QuadTreeCollisionEntity;

    class EntityHandle : NO_COPY {
    public:
        EntityHandle() = default;
        EntityHandle(std::list<Entity>::iterator entityListIter);
        ~EntityHandle();

        void invalidate();
        void update(const BoundingBox& bbox);
        void update(const BoundingBox& bbox, std::int32_t groupMask);
        void update(std::int32_t groupMask);

        // Move: (TODO - Move to .cpp file)
        EntityHandle(EntityHandle&& other) noexcept
            : _myIter{other._myIter}
        {
            other._myIter.reset();
        }

        EntityHandle& operator=(EntityHandle&& other) noexcept {
            _myIter = other._myIter;
            other._myIter.reset();
            return SELF;
        }

    private:
        std::optional<std::list<Entity>::iterator> _myIter;
    };

    QuadTreeCollisionDomain(double width, double height, PZInteger maxDepth, 
                            PZInteger maxEntitiesPerNode, PZInteger workerThreadsCount = 0);
    ~QuadTreeCollisionDomain();

    // Main functionality:
    EntityHandle insertEntity(EntityTag tag, const BoundingBox& bbox, std::int32_t groupMask);
    void prune();

    PZInteger recalcPairs();
    void recalcPairsStart();
    PZInteger recalcPairsJoin();
    bool pairsNext(CollisionPair& collisionPair);

    // Scanning - Point:
    bool scanPoint(double x, double y, std::int32_t groupMask, EntityTag& entityTag) const;

    PZInteger scanPoint(double x, double y, std::int32_t groupMask, std::vector<EntityTag>& entityTags) const;

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

    void draw(sf::RenderTarget& rt); // TODO Temp.
    void print() const;

private:
    using EntityList = std::list<Entity>;

    PZInteger _maxDepth;
    PZInteger _maxEntitiesPerNode;
    PZInteger _maxNodesPerRow;
    double _width, _height;
    double _minWidth, _minHeight;
    
    std::vector<std::vector<detail::QuadTreeNode*>> _nodeTable; // [x][y]
    std::unique_ptr<detail::QuadTreeNode> _rootNode;
    
    std::vector<detail::QuadTreeNode*> _nodesToProcess;
    std::vector<CollisionPair> _generatedPairs;

    void updateNodeTable(detail::QuadTreeNode& node);

    friend class detail::QuadTreeNode;

///////////////////////////////////////////////////////////////////////////////
// Multithreading stuff:

    struct MultithreadingData;

    struct WorkerContext {
        MultithreadingData& data;
        std::vector<detail::QuadTreeNode*>& nodesToProcess;
        std::vector<CollisionPair> generatedPairs;

        WorkerContext(MultithreadingData& mtData, std::vector<detail::QuadTreeNode*>& nodesToProcess);
        WorkerContext(const WorkerContext& other) = default;
    };

    struct MultithreadingData {
        std::vector<WorkerContext> workerContexts;
        std::vector<std::thread> workers;
        std::mutex jobMutex;
        util::Semaphore jobAvailableSem;
        util::Semaphore jobCompletedSem;
        PZInteger jobCount = 0;
        bool workersShouldReturn = false;
        int resultSelector = -1;

        MultithreadingData(PZInteger threadCount, std::vector<detail::QuadTreeNode*>& nodesToProcessVec);
        ~MultithreadingData();
    };

    std::unique_ptr<MultithreadingData> _mtData;

    static void workerBody(WorkerContext& ctx);

    bool isMultithreading() const;
};

} // namespace cd
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_CD_QTREE_COLLISION_DOMAIN_HPP