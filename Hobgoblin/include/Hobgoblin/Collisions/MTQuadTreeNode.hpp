#pragma once

#include <memory>
#include <vector>
#include <deque>
#include <list>

#include "SFML\Graphics.hpp" //Temp
#include "QuadTreeEntity.hpp"
#include "BoundingBox.hpp"
#include "GenericPtr.hpp"

namespace col {

    using qao::GenericPtr;

    class  MTQuadTreeDomain;

    typedef std::deque< std::pair<GenericPtr, GenericPtr> > PairsContainer;

    class MTQuadTreeNode {

    public:

        const size_t MAX_DEPTH;
        const size_t MAX_OBJECTS;

        ~MTQuadTreeNode();

        void obj_list_erase(std::list<size_t>::iterator iter);

    private:

        friend class MTQuadTreeDomain;

        friend void worker_body(MTQuadTreeDomain*, size_t, int*);

        BoundingBox bbox;

        size_t depth;
        size_t counter; // Number of objects in this node and its subnodes combined

        std::unique_ptr<MTQuadTreeNode> chl[4];
        MTQuadTreeDomain * domain;
        MTQuadTreeNode   * parent;

        std::list<size_t> objects;

        MTQuadTreeNode() = delete;
        MTQuadTreeNode(double x, double y, double w, double h, size_t maxdep, size_t maxobj, MTQuadTreeDomain * domain); //For the root
        MTQuadTreeNode(double x, double y, double w, double h, MTQuadTreeNode * parent, size_t maxdep, size_t maxobj, MTQuadTreeDomain * domain);

        void split(bool fit);
        void collapse();

        bool obj_fits(const QuadTreeEntity & entity) const;
        void obj_add(QuadTreeEntity & entity);
        void obj_add_fit(QuadTreeEntity & entity);
        int  obj_new_holder(const QuadTreeEntity & entity) const;

        void visit(PairsContainer & pairs_deq) const;
        void visit_with(PairsContainer & pairs_deq,
                        const QuadTreeEntity & entity) const;

        void migrate_entity(QuadTreeEntity & entity, MTQuadTreeNode * new_holder);

        void draw(sf::RenderTarget * rt);

        };

    }