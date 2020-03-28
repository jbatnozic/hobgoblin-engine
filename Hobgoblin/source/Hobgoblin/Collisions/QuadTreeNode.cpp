
#include <Hobgoblin/Collisions/QuadTreeNode.hpp>

#include <Hobgoblin/Collisions/QuadTreeEntity.hpp>
#include <Hobgoblin/Collisions/QuadTreeDomain.hpp>

#include <iostream>
#include <assert.h>

#define CHKGRP(entity1, entity2) ( ((entity1.bbox.groups) & (entity2.bbox.groups)) != 0 )
#define Self QuadTreeNode

namespace col {

    QuadTreeNode::QuadTreeNode(double x, double y, double w, double h, size_t maxdep, size_t maxobj, QuadTreeDomain * domain)
        : bbox(x, y, w, h, 0)
        , parent(nullptr)
        , MAX_DEPTH(maxdep)
        , MAX_OBJECTS(maxobj)
        , objects()
        , domain(domain) {

        for (int i = 0; i < 4; i += 1) {
            chl[i].reset();
            }

        depth   = 0;
        counter = 0;

        }

    QuadTreeNode::QuadTreeNode(double x, double y, double w, double h, QuadTreeNode * parent, size_t maxdep, size_t maxobj, QuadTreeDomain * domain)
        : bbox(x, y, w, h, 0)
        , parent(parent)
        , MAX_DEPTH(maxdep)
        , MAX_OBJECTS(maxobj)
        , objects()
        , domain(domain) {

        collapse();

        if (parent != nullptr)
            depth = parent->depth + 1;
        else 
            depth = 0;

        counter = 0;

        }

    QuadTreeNode::~QuadTreeNode() {

        for (int i = 0; i < 4; i += 1) {
            chl[i].reset();
            }

        assert(objects.empty());

        }

    void QuadTreeNode::split(bool fit) {

        //std::cout<<"QuadNode at depth ("<<depth<<") split.\n";

        if (chl[0] == nullptr) {

            double x = bbox.x,
                y = bbox.y,
                w = bbox.w,
                h = bbox.h;

            chl[0].reset(new QuadTreeNode(x, y, w / 2, h / 2, this, MAX_DEPTH, MAX_OBJECTS, domain));                 //Top-left
            chl[1].reset(new QuadTreeNode(x + w / 2, y, w / 2, h / 2, this, MAX_DEPTH, MAX_OBJECTS, domain));         //Top-right
            chl[2].reset(new QuadTreeNode(x, y + h / 2, w / 2, h / 2, this, MAX_DEPTH, MAX_OBJECTS, domain));         //Bottom-left
            chl[3].reset(new QuadTreeNode(x + w / 2, y + h / 2, w / 2, h / 2, this, MAX_DEPTH, MAX_OBJECTS, domain)); //Bottom-right

            }

        for (auto curr = objects.begin(); curr != objects.end(); void()) {

            auto & entity = domain->get_entity(*curr);

            int nh = obj_new_holder(entity);

            if (nh == -1) {

                curr = std::next(curr);

                }
            else {

                if (!fit)
                    chl[nh]->obj_add(entity);
                else
                    chl[nh]->obj_add_fit(entity);

                curr = objects.erase(curr);

                }

            } //End_for

        }

    void QuadTreeNode::collapse() {

        for (int i = 0; i < 4; i += 1) {
            chl[i].reset();
            }

        domain->node_table_update(this); 

        }

    bool QuadTreeNode::obj_fits(const QuadTreeEntity & entity) const {

        return (entity.bbox).enveloped_by(bbox);

        }

    void QuadTreeNode::obj_add(QuadTreeEntity & entity) {

        objects.push_back(entity.ptr.index());

        counter += 1;

        entity.holder.regular = this;
        entity.type = QuadTreeEntity::REGULAR;
        entity.my_iter = std::prev(objects.end());

        if (objects.size() > MAX_OBJECTS && depth < MAX_DEPTH) split(false);

        }

    void QuadTreeNode::obj_add_fit(QuadTreeEntity & entity) {

        QuadTreeNode * curr = this;

        NEXT: 

        if (curr->chl[0] != nullptr) {

            for (size_t i = 0; i < 4u; i += 1) {

                if (curr->chl[i]->obj_fits(entity)) {

                    curr->counter += 1u;
                    curr = curr->chl[i].get();
                    goto NEXT;

                    }

                }

            }

        curr->obj_add(entity);

        }

    void QuadTreeNode::obj_list_erase(std::list<size_t>::iterator iter) {

        objects.erase(iter);

        }

    int  QuadTreeNode::obj_new_holder(const QuadTreeEntity & entity) const {

        for (int i = 0; i < 4; i += 1) {

            if (chl[i]->obj_fits(entity)) return i;

            }

        return -1;

        }

    void QuadTreeNode::visit(PairsContainer & pairs_deq) const {

        if (objects.empty()) return;

        for (auto curr = objects.begin(); curr != objects.end(); curr = std::next(curr)) { //Fixate one object (curr)

            const QuadTreeEntity & obj = domain->get_entity(*curr);

            // This node:
            for (auto head = std::next(curr); head != objects.end(); head = std::next(head)) {

                //check_counter += 1;
                const auto & other = domain->get_entity(*head);

                if (CHKGRP(obj, other) && obj.bbox.overlaps( other.bbox )) {

                    pairs_deq.push_back(std::make_pair(obj.ptr, other.ptr));

                    }

                }

            // Supernodes:
            QuadTreeNode * inode = parent;

            while (inode != nullptr) {

                if (inode->objects.size() > 0)
                    inode->visit_with(pairs_deq, obj);

                inode = inode->parent;

                }

            } // End_for

        }

    void QuadTreeNode::visit_with(PairsContainer & pairs_deq,
                                  const QuadTreeEntity & entity) const {

        for (auto curr = objects.begin(); curr != objects.end(); curr = std::next(curr)) {

            const auto & other = domain->get_entity(*curr);

            if (CHKGRP(entity, other) && entity.bbox.overlaps( other.bbox )) {

                pairs_deq.push_back(std::make_pair(entity.ptr, other.ptr));

                }

            } // End_for

        }

    void QuadTreeNode::migrate_entity(QuadTreeEntity & entity, QuadTreeNode * new_holder) {

        new_holder->objects.splice(new_holder->objects.end(), objects, entity.my_iter);

        entity.holder.regular = new_holder;
        entity.type = QuadTreeEntity::REGULAR;
        entity.my_iter = std::prev(new_holder->objects.end());

        if (new_holder->objects.size() > new_holder->MAX_OBJECTS &&
            new_holder->depth < new_holder->MAX_DEPTH) new_holder->split(false);

        }

    void QuadTreeNode::draw(sf::RenderTarget * rt) {

        sf::RectangleShape rec(sf::Vector2f(float(bbox.w), float(bbox.h)));
        rec.setPosition(float(bbox.x), float(bbox.y));
        rec.setOutlineColor(sf::Color::Green);
        rec.setFillColor(sf::Color::Transparent);
        rec.setOutlineThickness(1);

        rt->draw(rec);

        if (chl[0].get() != nullptr)
            for (int i = 0; i < 4; i += 1)
                chl[i]->draw(rt);

        }

    }