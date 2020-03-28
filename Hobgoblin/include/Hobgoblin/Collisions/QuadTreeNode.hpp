#pragma once

#pragma once

#include <memory>
#include <vector>
#include <deque>
#include <list>

#include <SFML/Graphics.hpp> //Temp
#include <Hobgoblin/Collisions/QuadTreeEntity.hpp>
#include <Hobgoblin/Collisions/BoundingBox.hpp>
#include <Hobgoblin/Collisions/GenericPtr.hpp>

namespace col {

    using qao::GenericPtr;

    class  QuadTreeDomain;

    typedef std::deque< std::pair<GenericPtr, GenericPtr> > PairsContainer;

    class QuadTreeNode {

    public:

        const size_t MAX_DEPTH;
        const size_t MAX_OBJECTS;

        ~QuadTreeNode();

        void obj_list_erase(std::list<size_t>::iterator iter);

    private:

        friend class QuadTreeDomain;

        BoundingBox bbox;

        size_t depth;
        size_t counter; // Number of objects in this node and its subnodes combined

        std::unique_ptr<QuadTreeNode> chl[4];
        QuadTreeDomain * domain;
        QuadTreeNode   * parent;

        std::list<size_t> objects;

        QuadTreeNode() = delete;
        QuadTreeNode(double x, double y, double w, double h, size_t maxdep, size_t maxobj, QuadTreeDomain * domain); //For the root
        QuadTreeNode(double x, double y, double w, double h, QuadTreeNode * parent, size_t maxdep, size_t maxobj, QuadTreeDomain * domain);

        void split(bool fit);
        void collapse();

        bool obj_fits(const QuadTreeEntity & entity) const;
        void obj_add(QuadTreeEntity & entity);
        void obj_add_fit(QuadTreeEntity & entity);
        int  obj_new_holder(const QuadTreeEntity & entity) const;

        void visit(PairsContainer & pairs_deq) const;
        void visit_with(PairsContainer & pairs_deq,
                        const QuadTreeEntity & entity) const;

        void migrate_entity(QuadTreeEntity & entity, QuadTreeNode * new_holder);

        void draw(sf::RenderTarget * rt);

        };

    }