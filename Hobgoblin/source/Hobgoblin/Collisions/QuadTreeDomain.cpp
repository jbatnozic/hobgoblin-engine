
#include <Hobgoblin/Collisions/QuadTreeDomain.hpp>

#include <Hobgoblin/Collisions/QuadTreeEntity.hpp>
#include <Hobgoblin/Collisions/QuadTreeNode.hpp>

#include <vector>
#include <memory>
#include <deque>
#include <cmath>
#include <stdexcept>

#include <SFML\Graphics.hpp> // Temp!
#include <iostream>
#include <assert.h>

extern sf::RenderWindow * g_window;

#define ST_UNDEF   0
#define ST_WAITING 1
#define ST_WORKING 2
#define ST_DESTROY 3

namespace col {

    QuadTreeDomain::QuadTreeDomain(double x, double y, double w, double h, size_t maxdep, size_t maxobj)
        : X(x), Y(y), WIDTH(w), HEIGHT(h)
        , MAX_DEPTH(maxdep)
        , MAX_OBJECTS(maxobj) {

        if (w <= 0.0 || h <= 0.0) 
            throw std::out_of_range("col::QuadTreeDomain::QuadTreeDomain - W and H must both be greater than 0.");

        nt_size = (1u << maxdep);

        min_width  = w / double(nt_size);
        min_height = h / double(nt_size);

        node_table.resize(nt_size, std::vector<QuadTreeNode*>(nt_size));

        root.reset(new QuadTreeNode(x, y, w, h, maxdep, maxobj, this));
        root->split(false);

        for (size_t i = 0; i < nt_size; i += 1)
            for (size_t t = 0; t < nt_size; t += 1)
                node_table[i][t] = root.get();

        }

    QuadTreeDomain::~QuadTreeDomain() {

        clear();

        QuadTreeNode * _root = root.release(); // PEP

        delete _root;

        root.release();

        }

    void QuadTreeDomain::clear() {

        entities.clear();

        work_stack.clear();

        pairs_deq.clear();

        }

    void QuadTreeDomain::inst_insert(GenericPtr instance, const BoundingBox & bb) {

        if (inst_exists(instance))
            throw std::logic_error("col::QuadTreeDomain::inst_insert - Instance already exists.");

        size_t ind = instance.index();

        if (entities.size() <= ind) entities.resize(ind + 1);

        entities[ind] = QuadTreeEntity{bb, instance};

        root->obj_add_fit( entities[ind] );

        }

    void QuadTreeDomain::inst_update(GenericPtr instance, const BoundingBox & bb) {

        if (!inst_exists(instance))
            throw std::logic_error("col::QuadTreeDomain::inst_update - Instance doesn't exist.");

        size_t ind = instance.index();

        auto & entity = entities[ind];

        entity.bbox = bb;

        if (entity.holder.regular->obj_fits(entity) || entity.holder.regular->parent == nullptr) return;

        //Find new holder:
        QuadTreeNode *curr = entity.holder.regular->parent;

        // Holder cleanup:
        entity.holder.regular->counter -= 1;

        for (; ; curr = curr->parent) {

            curr->counter -= 1;

            if (curr->obj_fits(entity) || curr->parent == nullptr) {

                // FIND NEW HOLDER: ///////////////////////////////////////////

                QuadTreeNode * newhld = curr;
                newhld->counter += 1u;

                NEXT: 

                if (newhld->chl[0] != nullptr) {

                    for (size_t i = 0; i < 4u; i += 1) {

                        if (newhld->chl[i]->obj_fits(entity)) {

                            newhld = newhld->chl[i].get();
                            newhld->counter += 1u;
                            goto NEXT;

                            }

                        }

                    }

                ///////////////////////////////////////////////////////////////

                entity.holder.regular->migrate_entity(entity, newhld);
                break;

                }

            } //End_for

        }

    bool QuadTreeDomain::inst_exists(GenericPtr instance) const {

        if (instance.is_null())
            throw std::logic_error("col::QuadTreeDomain::inst_exists - GenericPtr is null.");

        if (instance.index() >= entities.size()) return false;

        if (entities[instance.index()].ptr != instance) return false;

        return true;

        }

    void QuadTreeDomain::inst_remove(GenericPtr instance) {

        if (!inst_exists(instance))
            throw std::logic_error("col::QuadTreeDomain::inst_remove - Instance doesn't exist.");

        size_t ind = instance.index();

        auto & entity = entities[ind];

        QuadTreeNode * curr = entity.holder.regular->parent;

        if (entity.holder.multithr) {
            // Holder cleanup:
            entity.holder.regular->obj_list_erase(entity.my_iter);
            entity.holder.regular->counter -= 1;
            entity.holder.regular = nullptr;
            }

        for (; curr != nullptr; curr = curr->parent) {

            curr->counter -= 1;

            }

        entity.ptr = nullptr;

        }

    void QuadTreeDomain::pairs_recalc_start() {

        // Do nothing

        }

    size_t QuadTreeDomain::pairs_recalc_join() {

        pairs_deq.clear();
        pairs_hand = 0;

        work_stack.push_back(root.get());

        while (!work_stack.empty()) {

            QuadTreeNode *node = work_stack.back();
            work_stack.pop_back();

            node->visit(pairs_deq);

            if (node->chl[0] != nullptr) {

                for (int i = 3; i >= 0; i -= 1)
                    work_stack.push_back(node->chl[i].get());

                }

            } //End_while

        clean_up();

        // TEMP:
        //root->draw(g_window);

        return pairs_deq.size();

        }

    size_t QuadTreeDomain::pairs_recalc() {

        pairs_recalc_start();

        return pairs_recalc_join();

        }

    bool   QuadTreeDomain::pairs_next(GenericPtr & inst1, GenericPtr & inst2) {

        if (pairs_hand < pairs_deq.size()) {

            const auto & pair = pairs_deq[pairs_hand];

            inst1 = pair.first;
            inst2 = pair.second;

            pairs_hand += 1u;
            return true;

            }
        else {

            return false;

            }

        }

    // Protected / Private: ///////////////////////////////////////////////////

    QuadTreeEntity & QuadTreeDomain::get_entity(size_t index) const {

        return const_cast<QuadTreeEntity &>( entities.at(index) );

        }

    void QuadTreeDomain::node_table_update(QuadTreeNode * node) {

        int sx = (int)round( (node->bbox.x - root->bbox.x) / min_width  );
        int sy = (int)round( (node->bbox.y - root->bbox.y) / min_height );

        int lx = (int)round( (node->bbox.w) / min_width  );
        int ly = (int)round( (node->bbox.h) / min_height );

        for (int i = sx; i < sx + lx; i += 1)
            for (int t = sy; t < sy + ly; t += 1)
                node_table[i][t] = node;

        }

    void QuadTreeDomain::clean_up() {

        work_stack.clear();

        work_stack.push_back(root.get());

        while (!work_stack.empty()) {

            QuadTreeNode * node = work_stack.back();
            work_stack.pop_back();

            if (node->chl[0] != nullptr) {

                if (node->counter == node->objects.size()) {

                    node->collapse();

                    }
                else {

                    for (int i = 3; i >= 0; i -= 1)
                        work_stack.push_back(node->chl[i].get());

                    }

                }

            } //End_while

        }

    }