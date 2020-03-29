
#include <Hobgoblin/Collisions/Qtree_collision_domain.hpp>
#include <SFML/Graphics.hpp> // TODO Temp.

#include <array>
#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace util {

namespace detail {

class QuadTreeNode {
public:
    using BoundingBox = QuadTreeCollisionDomain::BoundingBox;
    using Entity = detail::CollisionEntity;
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

    void setEntityBbox(EntityList::iterator entityIter, const BoundingBox& bbox) {
        auto& entity = (*entityIter);
        entity.bbox = bbox;

        if (entityFits(entity) || _parent == nullptr) {
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
                    collisionPairs.push_back(std::make_pair(entity.index, other.index));
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

    // TODO Temp.
    void draw(sf::RenderTarget& rt) const {
        sf::RectangleShape rec(sf::Vector2f(float(_bbox.w), float(_bbox.h)));
        rec.setPosition(float(_bbox.x), float(_bbox.y));
        rec.setOutlineColor(sf::Color::Green);
        rec.setFillColor(sf::Color::Transparent);
        rec.setOutlineThickness(1);

        rt.draw(rec);

        if (_children[0] != nullptr) {
            for (int i = 0; i < 4; i += 1) {
                _children[i]->draw(rt);
            }
        }
    }

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

    // TODO
    void collapse() { // Maybe tryCollapse / prune?
        // TODO - what if children still have entities? Splice them into this node!

        for (auto& child : _children) {
            child.reset();
        }

        // domain.updateNodeTable(Self);
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
                collisionPairs.push_back(std::make_pair(myEntity.index, entity.index));
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

QuadTreeCollisionDomain::~QuadTreeCollisionDomain() {
    // TODO
}

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
    , _rootNode{std::make_unique<detail::QuadTreeNode>(Self, BoundingBox{0.0, 0.0, width, height},
                                                       maxDepth, maxEntitiesPerNode, nullptr)}
    , _semaphore{0}
    , _doneSem{0}
{
    _exiting = false;
    
    // TODO Very unsafe because the contexts can get relocated...
    for (PZInteger i = 0; i < workerThreadsCount; i += 1) {
        _workerContexts.emplace_back(std::ref(_nodesToProcess), std::ref(_mutex), std::ref(_semaphore),
                                     std::ref(_doneSem), std::ref(_exiting));
    }
    for (PZInteger i = 0; i < workerThreadsCount; i += 1) {
        _workers.push_back(std::thread{workerBody, std::ref(_workerContexts[i])});
    }
}

QuadTreeCollisionDomain::EntityHandle QuadTreeCollisionDomain::insertEntity(INDEX entitySlabIndex, 
                                                                            const BoundingBox& bbox,
                                                                            std::int32_t groupMask) {
    auto iter = _rootNode->insertEntityPerfectFit(Entity{bbox, nullptr, groupMask, entitySlabIndex});
    return EntityHandle{iter};
}

PZInteger QuadTreeCollisionDomain::recalcPairs() {
    recalcPairsStart();
    return recalcPairsJoin();
}

void QuadTreeCollisionDomain::recalcPairsStart() {
    _nodesToProcess.clear();
    _rootNode->listAllNodes(_nodesToProcess);

    _waitCount = static_cast<PZInteger>(_nodesToProcess.size());
    for (PZInteger i = 0; i < _waitCount; i += 1) {
        _semaphore.signal();
    }
}

PZInteger QuadTreeCollisionDomain::recalcPairsJoin() {
    while (true) {
        if (_semaphore.tryWait()) {
            detail::QuadTreeNode* node;
            {
                std::lock_guard lock(_mutex);
                if (!_nodesToProcess.empty()) {
                    node = _nodesToProcess.back();
                    _nodesToProcess.pop_back();
                }
                else {
                    break;
                }
            }
            node->visit(_pairs);
            _doneSem.signal();
        }
        else {
            break;
        }
    }

    for (PZInteger i = 0; i < _waitCount; i += 1) {
        _doneSem.wait();
    }
    assert(_doneSem.getValue() == 0);

    PZInteger rv = _pairs.size();
    for (auto& ctx : _workerContexts) {
        rv += ctx.pairs.size();
    }

    _pairsVecSelector = -1;

    return rv;
}

bool QuadTreeCollisionDomain::pairsNext(INDEX& index1, INDEX& index2) {
    std::vector<CollisionPair>* vec;

    NEXT:
    if (_pairsVecSelector == -1) {
        vec = &_pairs;
    }
    else if (_pairsVecSelector >= _workerContexts.size()) { // TODO cast to PZInteger (stopz)
        return false;
    }
    else {
        vec = &(_workerContexts[_pairsVecSelector].pairs);
    }

    if (vec->empty()) {
        _pairsVecSelector += 1;
        goto NEXT;
    }

    if (vec->back().first < 0 || vec->back().second < 0) {
        int i = 0;
    }

    index1 = vec->back().first;
    index2 = vec->back().second;
    vec->pop_back();

    return true;
 }

void QuadTreeCollisionDomain::draw(sf::RenderTarget& rt) {
    _rootNode->draw(rt);
}

void QuadTreeCollisionDomain::print() const {
    _rootNode->print();
}

void QuadTreeCollisionDomain::prune() {

}

void QuadTreeCollisionDomain::workerBody(WorkerContext& ctx) {
    while (true) {
        ctx.semaphore.wait();
        if (ctx.exiting) {
            return;
        }
        detail::QuadTreeNode* node;
        {
            std::lock_guard lock(ctx.mutex);
            if (!ctx.nodesToProcess.empty()) {
                node = ctx.nodesToProcess.back();
                ctx.nodesToProcess.pop_back();
            }
            else {
                continue;
            }
        }
        node->visit(ctx.pairs);
        ctx.doneSem.signal();
    }
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>