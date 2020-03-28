
#include <Hobgoblin/Collisions/Qtree_collision_domain.hpp>

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
        , _parent{nullptr}
        , _bbox{bbox}
        , _depth{parent ? (parent->_depth + 1) : 0}
        , _childrenEntitiesCount{0}
        , _maxDepth{maxDepth}
        , _maxEntities{maxEntitiesPerNode}
    {
    }

    ~QuadTreeNode() {
        // TODO
    }

    std::pair<EntityList&, EntityList::iterator> insertEntityPerfectFit(const Entity& entity) {
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

    void listLeafNodes(std::vector<QuadTreeNode*>& nodeVec) {
        if (_children[0] == nullptr) {
            nodeVec.push_back(this);
        }
        else {
            for (auto& child : _children) {
                child->listLeafNodes(nodeVec);
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

    std::pair<EntityList&, EntityList::iterator> insertEntity(const Entity& entity) {
        _entities.push_back(entity);
        _childrenEntitiesCount += 1;

        auto rv = std::make_pair(std::ref(_entities), std::prev(_entities.end()));

        if (_entities.size() > _maxEntities && _depth < _maxDepth) {
            split();
        }

        return rv;
    }

    void insertEntitySplice(EntityList& donorList, EntityList::iterator iterToEntity) {
        _entities.splice(_entities.end(), donorList, iterToEntity);
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

    /*
    void migrate_entity(QuadTreeEntity& entity, QuadTreeNode* new_holder);
    void draw(sf::RenderTarget* rt);
    */

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
// QuadTreeCollisionDomain methods:

QuadTreeCollisionDomain::EntityHandle QuadTreeCollisionDomain::insertEntity(PZInteger entitySlabIndex, 
                                                                            const BoundingBox& bbox) {
    if (entitySlabIndex >= static_cast<int>(_entitiyIterators.size())) {
        _entitiyIterators.resize(entitySlabIndex + 1);
    }

    assert(!_entitiyIterators[entitySlabIndex].has_value());

    auto pair = _rootNode->insertEntityPerfectFit(Entity{bbox, 0, entitySlabIndex}); // TODO (group)
    _entitiyIterators[entitySlabIndex] = pair.second;

    //return EntityHandle{Self}; // TODO
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>