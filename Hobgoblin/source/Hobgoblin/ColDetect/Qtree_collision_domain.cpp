
#include <Hobgoblin/ColDetect/Qtree_collision_domain.hpp>
#include <SFML/Graphics.hpp> // TODO Temp.

#include <array>
#include <cassert>
#include <cmath>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace cd {

namespace detail {

class QuadTreeNode {
public:
    using Entity = detail::QuadTreeCollisionEntity;
    using EntityList = std::list<Entity>;

    QuadTreeNode(QuadTreeCollisionDomain& domain, const BoundingBox& bbox,
                 PZInteger maxDepth, PZInteger maxEntitiesPerNode, QuadTreeNode* parent)
        : _domain{domain}
        , _parent{parent}
        , _bbox{bbox}
        , _depth{parent ? (parent->_depth + 1) : 0}
        , _childrenEntitiesCount{0}
        , _maxDepth{maxDepth}
        , _maxEntities{maxEntitiesPerNode}
    {
        FRIEND_ACCESS _domain.updateNodeTable(SELF);
    }

    ~QuadTreeNode() {
    }

    EntityList::iterator insertEntityPerfectFit(const Entity& entity) {
        if (_children[0] == nullptr) {
            return insertEntity(entity);
        }

        QuadTreeNode* const newHolder = newEntityHolder(entity);
        if (newHolder == this) {
            return insertEntity(entity);
        }
        else {
            _childrenEntitiesCount += 1;
            return newHolder->insertEntityPerfectFit(entity);
        }
    }

    void eraseEntity(EntityList::iterator entityIter) {
        _entities.erase(entityIter);
        for (QuadTreeNode* curr = this; curr != nullptr; curr = curr->_parent) {
            curr->_childrenEntitiesCount -= 1;
        }
    }

    void prune() {
        if (_children[0] == nullptr) {
            return;
        }

        for (auto& child : _children) {
            child->prune();
        }

        if (_childrenEntitiesCount <= _maxEntities) {
            // Migrate all child entities to this node:
            for (auto& child : _children) {
                while (!child->_entities.empty()) {
                    _entities.splice(_entities.end(), child->_entities, std::prev(child->_entities.end()));
                    _entities.back().holder = this;
                }
            }

            for (auto& child : _children) {
                assert(child->_entities.empty());
                child.reset();
            }
        }

        FRIEND_ACCESS _domain.updateNodeTable(SELF);
    }

    void setEntityBbox(EntityList::iterator entityIter, const BoundingBox& bbox) {
        auto& entity = (*entityIter);
        entity.bbox = bbox;

        if (entityFits(entity) || _parent == nullptr) {
            // TODO - Maybe try to move downward?
            return;
        }

        _childrenEntitiesCount -= 1;

        for (QuadTreeNode* curr = _parent; true; curr = curr->_parent) {
            curr->_childrenEntitiesCount -= 1;

            if (curr->entityFits(entity) || curr->_parent == nullptr) {
                // Find new holder:
                QuadTreeNode* newHolder = curr;

            REPEAT:
                if (newHolder->_children[0] != nullptr) {
                    for (auto& child : newHolder->_children) {
                        if (child->entityFits(entity)) {
                            newHolder->_childrenEntitiesCount += 1;
                            newHolder = child.get();
                            goto REPEAT;
                        }
                    }
                }

                // New holder found:
                newHolder->insertEntitySplice(_entities, entityIter);
                break;
            }
        }
    }

    void listAllNodes(std::vector<QuadTreeNode*>& nodeVec) {
        nodeVec.push_back(this);
        if (_children[0] != nullptr) {
            for (auto& child : _children) {
                child->listAllNodes(nodeVec);
            }
        }
    }

    void visit(std::vector<CollisionPair>& collisionPairs) const {
        for (auto curr = _entities.begin(); curr != _entities.end(); curr = std::next(curr)) {
            auto& entity = *curr;

            // Collisions in this node:
            for (auto head = std::next(curr); head != _entities.end(); head = std::next(head)) {
                auto& other = *head;
                if (entity.collidesWith(other)) {
                    collisionPairs.push_back(std::make_pair(entity.tag, other.tag));
                }
            }

            // Collisions in parent nodes:
            QuadTreeNode* supernode = _parent;

            while (supernode != nullptr) {
                supernode->visitWith(collisionPairs, entity);
                supernode = supernode->_parent;
            }
        }
    }

    bool scanBbox(const BoundingBox& bbox, std::int32_t groupMask, bool mustEnvelop, EntityTag& tag) const {
        if (!mustEnvelop) {
            for (auto& entity : _entities) {
                if ((entity.groupMask & groupMask != 0) && entity.bbox.overlaps(bbox)) {
                    tag = entity.tag;
                    return true;
                }
            }
        }
        else {
            for (auto& entity : _entities) {
                if ((entity.groupMask & groupMask != 0) && entity.bbox.envelopedBy(bbox)) {
                    tag = entity.tag;
                    return true;
                }
            }
        }
        return false;
    }

    PZInteger scanBbox(const BoundingBox& bbox, std::int32_t groupMask, bool mustEnvelop, 
                       std::vector<EntityTag>& tags) const {
        PZInteger rv = 0;
        if (!mustEnvelop) {
            for (auto& entity : _entities) {
                if ((entity.groupMask & groupMask != 0) && entity.bbox.overlaps(bbox)) {
                    tags.push_back(entity.tag);
                    rv += 1;
                }
            }
        }
        else {
            for (auto& entity : _entities) {
                if ((entity.groupMask & groupMask != 0) && entity.bbox.envelopedBy(bbox)) {
                    tags.push_back(entity.tag);
                    rv += 1;
                }
            }
        }
        return rv;
    }

    PZInteger getChildEntityCount() const {
        return _childrenEntitiesCount;
    }

    const BoundingBox& getBoundingBox() const {
        return _bbox;
    }

    const QuadTreeNode* getParent() const {
        return _parent;
    }

    // TODO Temp.
    void draw(sf::RenderTarget& rt) const {
        sf::RectangleShape rec(sf::Vector2f(float(_bbox.w), float(_bbox.h)));
        rec.setPosition(float(_bbox.x), float(_bbox.y));
        rec.setOutlineColor(sf::Color(41, 77, 51));
        rec.setFillColor(sf::Color::Transparent);
        rec.setOutlineThickness(1);

        rt.draw(rec);

        if (_children[0] != nullptr) {
            for (int i = 0; i < 4; i += 1) {
                _children[i]->draw(rt);
            }
        }
    }

    // TODO Temp.
    void print() const {
        /*for (int i = 0; i < _depth; i += 1) {
            std::cout << "  ";
        }
        std::cout << "Node at depth " << _depth << "\n";
        for (auto& entity : _entities) {
            for (int i = 0; i < _depth + 1; i += 1) {
                std::cout << "  ";
            }
            std::cout << "Entity with index " << entity.index << '\n';
        }
        for (auto& child : _children) {
            if (child != nullptr) {
                child->print();
            }
        }*/
    }

private:
    std::array<std::unique_ptr<QuadTreeNode>, 4> _children;
    std::list<Entity> _entities;

    QuadTreeCollisionDomain& _domain;
    QuadTreeNode* _parent;

    BoundingBox _bbox;

    PZInteger _depth;
    PZInteger _childrenEntitiesCount; // Number of objects in this node and its children nodes combined

    PZInteger _maxDepth;
    PZInteger _maxEntities;

    void split() {
        if (_children[0] == nullptr) {
            const double x = _bbox.x;
            const double y = _bbox.y;
            const double w = _bbox.w;
            const double h = _bbox.h;

            const auto bboxTopLeft = BoundingBox{x, y, w / 2, h / 2};
            const auto bboxTopRight = BoundingBox{x + w / 2, y, w / 2, h / 2};
            const auto bboxBtmLeft = BoundingBox{x, y + h / 2, w / 2, h / 2};
            const auto bboxBtmRight = BoundingBox{x + w / 2, y + h / 2, w / 2, h / 2};

            _children[0] = std::make_unique<QuadTreeNode>(_domain, bboxTopLeft,  _maxDepth, _maxEntities, this);
            _children[1] = std::make_unique<QuadTreeNode>(_domain, bboxTopRight, _maxDepth, _maxEntities, this);
            _children[2] = std::make_unique<QuadTreeNode>(_domain, bboxBtmLeft,  _maxDepth, _maxEntities, this);
            _children[3] = std::make_unique<QuadTreeNode>(_domain, bboxBtmRight, _maxDepth, _maxEntities, this);
        }

        for (auto curr = _entities.begin(); curr != _entities.end(); NO_OP()) {
            auto& entity = *curr;

            QuadTreeNode* const newHolder = newEntityHolder(entity);

            if (newHolder == this) {
                curr = std::next(curr);
            }
            else {
                auto temp = curr;
                curr = std::next(curr);
                newHolder->insertEntitySplice(_entities, temp);
            }
        }
    }

    bool entityFits(const Entity& entity) const {
        return entity.bbox.envelopedBy(_bbox);
    }

    EntityList::iterator insertEntity(const Entity& entity) {
        _entities.push_back(entity);
        _entities.back().holder = this;
        _childrenEntitiesCount += 1;

        auto rv = std::prev(_entities.end());

        if (_entities.size() > _maxEntities && _depth < _maxDepth) {
            split();
        }

        return rv;
    }

    void insertEntitySplice(EntityList& donorList, EntityList::iterator iterToEntity) {
        _entities.splice(_entities.end(), donorList, iterToEntity);
        _entities.back().holder = this;
        _childrenEntitiesCount += 1;

        if (_entities.size() > _maxEntities && _depth < _maxDepth) {
            split();
        }
    }

    void visitWith(std::vector<CollisionPair>& collisionPairs, const Entity& entity) const {
        for (auto& myEntity : _entities) {
            if (myEntity.collidesWith(entity)) {
                collisionPairs.push_back(std::make_pair(myEntity.tag, entity.tag));
            }
        }
    }

    QuadTreeNode* newEntityHolder(const Entity& entity) {
        for (auto& child : _children) {
            if (child->entityFits(entity)) {
                return child.get();
            }
        }
        return this;
    }
};

} // namespace detail

///////////////////////////////////////////////////////////////////////////////
// EntityHandle methods:

QuadTreeCollisionDomain::EntityHandle::EntityHandle(std::list<Entity>::iterator entityListIter)
    : _myIter{entityListIter}
{
}

QuadTreeCollisionDomain::EntityHandle::~EntityHandle() {
    invalidate();
}

void QuadTreeCollisionDomain::EntityHandle::invalidate() {
    if (_myIter) {
        detail::QuadTreeNode* const holder = (**_myIter).holder;
        holder->eraseEntity(*_myIter);
    }
}

void QuadTreeCollisionDomain::EntityHandle::update(const BoundingBox& bbox) {
    (**_myIter).holder->setEntityBbox(*_myIter, bbox);
}

void QuadTreeCollisionDomain::EntityHandle::update(const BoundingBox& bbox, std::int32_t groupMask) {
    update(bbox);
    (**_myIter).groupMask = groupMask;
}

void QuadTreeCollisionDomain::EntityHandle::update(std::int32_t groupMask) {
    (**_myIter).groupMask = groupMask;
}

///////////////////////////////////////////////////////////////////////////////
// QuadTreeCollisionDomain methods:

QuadTreeCollisionDomain::QuadTreeCollisionDomain(double width, double height, PZInteger maxDepth,
                                                 PZInteger maxEntitiesPerNode, PZInteger workerThreadsCount)
    : _maxDepth{maxDepth}
    , _maxEntitiesPerNode{maxEntitiesPerNode}
    , _maxNodesPerRow{1 << maxDepth}
    , _width{width}
    , _height{height}
    , _minWidth{width / _maxNodesPerRow}
    , _minHeight{height / _maxNodesPerRow}
    , _nodeTable{_maxNodesPerRow, std::vector<detail::QuadTreeNode*>{static_cast<std::size_t>(_maxNodesPerRow)}}
    , _rootNode{std::make_unique<detail::QuadTreeNode>(SELF, BoundingBox{0.0, 0.0, width, height},
                                                       maxDepth, maxEntitiesPerNode, nullptr)}
{
    if (workerThreadsCount > 0) {
        _mtData = std::make_unique<MultithreadingData>(workerThreadsCount, _nodesToProcess);
    }
}

QuadTreeCollisionDomain::~QuadTreeCollisionDomain() {
    assert(_rootNode->getChildEntityCount() == 0 && "Must remove all entities from the domain before destructing it");
    _mtData.reset();
}

QuadTreeCollisionDomain::EntityHandle QuadTreeCollisionDomain::insertEntity(EntityTag tag, 
                                                                            const BoundingBox& bbox,
                                                                            std::int32_t groupMask) {
    auto iter = _rootNode->insertEntityPerfectFit(Entity{bbox, nullptr, groupMask, tag});
    return EntityHandle{iter};
}

void QuadTreeCollisionDomain::prune() {
    _rootNode->prune();
}

PZInteger QuadTreeCollisionDomain::recalcPairs() {
    recalcPairsStart();
    return recalcPairsJoin();
}

void QuadTreeCollisionDomain::recalcPairsStart() {
    // TODO clearGeneratedPairs

    _nodesToProcess.clear();
    _rootNode->listAllNodes(_nodesToProcess);

    if (isMultithreading()) {
        _mtData->jobCount = static_cast<PZInteger>(_nodesToProcess.size());
        for (PZInteger i = 0; i < _mtData->jobCount; i += 1) {
            _mtData->jobAvailableSem.signal();
        }
    }
}

PZInteger QuadTreeCollisionDomain::recalcPairsJoin() {
    if (!isMultithreading()) {
        for (auto& node : _nodesToProcess) {
            node->visit(_generatedPairs);
        }
        return stopz(_generatedPairs.size());
    }
    else {
        // Help the workers while waiting:
        while (true) {
            if (_mtData->jobAvailableSem.tryWait()) {
                detail::QuadTreeNode* node;
                {
                    std::lock_guard lock(_mtData->jobMutex);
                    if (!_nodesToProcess.empty()) {
                        node = _nodesToProcess.back();
                        _nodesToProcess.pop_back();
                    }
                    else {
                        break;
                    }
                }
                node->visit(_generatedPairs);
                _mtData->jobCompletedSem.signal();
            }
            else {
                break;
            }
        }

        // Wait for all to complete jobs:
        for (PZInteger i = 0; i < _mtData->jobCount; i += 1) {
            _mtData->jobCompletedSem.wait();
        }
        assert(_mtData->jobCompletedSem.getValue() == 0);

        // Count results:
        std::size_t rv = _generatedPairs.size();
        for (auto& ctx : _mtData->workerContexts) {
            rv += ctx.generatedPairs.size();
        }

        _mtData->resultSelector = -1;

        return stopz(rv);
    }
}

bool QuadTreeCollisionDomain::pairsNext(CollisionPair& collisionPair) {
    if (!isMultithreading()) {
        if (_generatedPairs.empty()) {
            return false;
        }

        collisionPair = _generatedPairs.back();
        _generatedPairs.pop_back();

        return true;
    }
    else {
        std::vector<CollisionPair>* resultVec;

    NEXT_RESULT_VEC:
        if (_mtData->resultSelector == -1) {
            resultVec = &_generatedPairs;
        }
        else if (_mtData->resultSelector >= stopz(_mtData->workerContexts.size())) {
            return false;
        }
        else {
            resultVec = &(_mtData->workerContexts[_mtData->resultSelector].generatedPairs);
        }

        if (resultVec->empty()) {
            _mtData->resultSelector += 1;
            goto NEXT_RESULT_VEC;
        }

        collisionPair = resultVec->back();
        resultVec->pop_back();

        return true;
    }
 }

bool QuadTreeCollisionDomain::scanPoint(double x, double y, std::int32_t groupMask,
                                        EntityTag& entityTag) const {
    const BoundingBox dummyBbox{x, y, 0.0, 0.0};
    
    const auto startX = static_cast<std::size_t>(floor(x / _minWidth));
    const auto startY = static_cast<std::size_t>(floor(y / _minHeight));

    const detail::QuadTreeNode* currentNode = _nodeTable[startX][startY];
    for (NO_OP(); currentNode != nullptr; currentNode = currentNode->getParent()) {
        if (currentNode->scanBbox(dummyBbox, groupMask, false, entityTag)) {
            return true;
        }
    }
    return false;
}

PZInteger QuadTreeCollisionDomain::scanPoint(double x, double y, std::int32_t groupMask,
                                             std::vector<EntityTag>& entityTags) const {
    const BoundingBox dummyBbox{x, y, 0.0, 0.0};

    const auto startX = static_cast<std::size_t>(floor(x / _minWidth));
    const auto startY = static_cast<std::size_t>(floor(y / _minHeight));

    PZInteger rv = 0;
    const detail::QuadTreeNode* currentNode = _nodeTable[startX][startY];
    for (NO_OP(); currentNode != nullptr; currentNode = currentNode->getParent()) {
        rv += currentNode->scanBbox(dummyBbox, groupMask, false, entityTags);
    }
    return rv;
}

void QuadTreeCollisionDomain::draw(sf::RenderTarget& rt) {
    _rootNode->draw(rt);
}

void QuadTreeCollisionDomain::print() const {
    _rootNode->print();
}

void QuadTreeCollisionDomain::updateNodeTable(detail::QuadTreeNode& node) {
    using std::round;

    const auto& nodeBbox = node.getBoundingBox();

    const auto startX = static_cast<std::size_t>(round(nodeBbox.x / _minWidth));
    const auto startY = static_cast<std::size_t>(round(nodeBbox.y / _minHeight));

    const auto lenX = static_cast<std::size_t>(round(nodeBbox.w / _minWidth));
    const auto lenY = static_cast<std::size_t>(round(nodeBbox.h / _minHeight));

    for (auto t = startY; t < startY + lenY; t += 1) {
        for (auto i = startX; i < startX + lenX; i += 1) {
            _nodeTable[i][t] = &node;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// QuadTreeCollisionDomain multithreading stuff:

QuadTreeCollisionDomain::WorkerContext::WorkerContext(MultithreadingData& mtData,
                                                      std::vector<detail::QuadTreeNode*>& nodesToProcess)
    : data{mtData}
    , nodesToProcess{nodesToProcess}
{
}

QuadTreeCollisionDomain::MultithreadingData::MultithreadingData(PZInteger threadCount,
                                                                std::vector<detail::QuadTreeNode*>& nodesToProcessVec)
    : jobAvailableSem{0}
    , jobCompletedSem{0}
{
    // IMPORTANT: Reserve memory beforehand so the contexts don't get relocated.
    workerContexts.reserve(pztos(threadCount));
    workers.reserve(pztos(threadCount));

    for (PZInteger i = 0; i < threadCount; i += 1) {
        workerContexts.emplace_back(std::ref(SELF), std::ref(nodesToProcessVec));
        workers.push_back(std::thread{QuadTreeCollisionDomain::workerBody, std::ref(workerContexts.back())});
    }
}

QuadTreeCollisionDomain::MultithreadingData::~MultithreadingData() {
    workersShouldReturn = true;
    for (auto& ctx : workerContexts) {
        jobAvailableSem.signal();
    }
    for (auto& worker : workers) {
        worker.join();
    }
}

void QuadTreeCollisionDomain::workerBody(WorkerContext& ctx) {
    while (true) {
        ctx.data.jobAvailableSem.wait();
        if (ctx.data.workersShouldReturn) {
            return;
        }
        detail::QuadTreeNode* node;
        {
            std::lock_guard lock(ctx.data.jobMutex);
            assert(!ctx.nodesToProcess.empty());
            node = ctx.nodesToProcess.back();
            ctx.nodesToProcess.pop_back();
        }
        node->visit(ctx.generatedPairs);
        ctx.data.jobCompletedSem.signal();
    }
}

bool QuadTreeCollisionDomain::isMultithreading() const {
    return _mtData != nullptr;
}

} // namespace cd
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>