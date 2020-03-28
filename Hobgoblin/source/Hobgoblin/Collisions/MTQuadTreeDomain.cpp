
#include <Hobgoblin/Collisions/MTQuadTreeDomain.hpp>

#include <Hobgoblin/Collisions/QuadTreeEntity.hpp>
#include <Hobgoblin/Collisions/MTQuadTreeNode.hpp>

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

    static void worker_body(MTQuadTreeDomain * domain, const size_t index, int * status) {
        
        std::vector<MTQuadTreeNode*> stack;
        stack.reserve(domain->MAX_DEPTH * 4u);

        while (true) {

            *status = ST_WAITING;

            domain->wsem[index].wait();

            if (*status == ST_DESTROY) return;

            // WORK: //////////////////////////////////////////////////////////

            *status = ST_WORKING;

            domain->pairs_deq[index].clear();

            stack.push_back(domain->root->chl[index].get());

            while (!stack.empty()) {

                MTQuadTreeNode * node = stack.back();
                stack.pop_back();

                node->visit(domain->pairs_deq[index]);

                if (node->chl[0] != nullptr) {

                    for (int i = 3; i >= 0; i -= 1)
                        stack.push_back(node->chl[i].get());

                    }

                } //End_while

            // SIGNAL DOMAIN: /////////////////////////////////////////////////

            domain->dsem.signal();
            
            }

        }

    ///////////////////////////////////////////////////////////////////////////

    MTQuadTreeDomain::MTQuadTreeDomain(double x, double y, double w, double h, size_t maxdep, size_t maxobj)
        : X(x), Y(y), WIDTH(w), HEIGHT(h)
        , MAX_DEPTH(maxdep)
        , MAX_OBJECTS(maxobj) {
        
        if (w <= 0.0 || h <= 0.0) 
            throw std::out_of_range("col::MTQuadTreeDomain::MTQuadTreeDomain - W and H must both be greater than 0.");

        nt_size = (1u << maxdep);

        min_width  = w / double(nt_size);
        min_height = h / double(nt_size);

        node_table.resize(nt_size, std::vector<MTQuadTreeNode*>(nt_size));

        root.reset(new MTQuadTreeNode(x, y, w, h, maxdep, maxobj, this));
        root->split(false);

        for (size_t i = 0; i < nt_size; i += 1)
            for (size_t t = 0; t < nt_size; t += 1)
                node_table[i][t] = root.get();

        for (size_t i = 0; i < 4u; i += 1) {
            
            worker[i] = std::thread(worker_body, this, i, (status + i));
            status[i] = ST_UNDEF;

            }

        domain_locked = false;

        }

    MTQuadTreeDomain::~MTQuadTreeDomain() {
        
        clear();

        for (size_t i = 0; i < 4; i += 1)
            status[i] = ST_DESTROY;

        for (size_t i = 0; i < 4; i += 1)
            wsem[i].signal();

        for (size_t i = 0; i < 4; i += 1)
            worker[i].join();

        MTQuadTreeNode * _root = root.release(); // PEP

        delete _root;

        root.release();

        }

    void MTQuadTreeDomain::clear() {
        
        assert(!domain_locked);

        entities.clear();

        cleanup_stack.clear();

        for (size_t i = 0; i < 4u; i += 1)
            pairs_deq[i].clear();

        }

    void MTQuadTreeDomain::inst_insert(GenericPtr instance, const BoundingBox & bb) {

        assert(!domain_locked);

        if (inst_exists(instance))
            throw std::logic_error("col::MTQuadTreeDomain::inst_insert - Instance already exists.");

        size_t ind = instance.index();

        if (entities.size() <= ind) entities.resize(ind + 1);

        entities[ind] = QuadTreeEntity{bb, instance};

        root->obj_add_fit( entities[ind] );

        }

    void MTQuadTreeDomain::inst_update(GenericPtr instance, const BoundingBox & bb) {
        
        assert(!domain_locked);

        if (!inst_exists(instance))
            throw std::logic_error("col::MTQuadTreeDomain::inst_update - Instance doesn't exist.");

        size_t ind = instance.index();

        auto & entity = entities[ind];

        entity.bbox = bb;

        if (entity.holder.multithr->obj_fits(entity) || entity.holder.multithr->parent == nullptr) return;

        //Find new holder:
        MTQuadTreeNode *curr = entity.holder.multithr->parent;

        // Holder cleanup:
        entity.holder.multithr->counter -= 1;

        for (; ; curr = curr->parent) {

            curr->counter -= 1;

            if (curr->obj_fits(entity) || curr->parent == nullptr) {

                // FIND NEW HOLDER: ///////////////////////////////////////////

                MTQuadTreeNode * newhld = curr;
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

                entity.holder.multithr->migrate_entity(entity, newhld);
                break;

                }

            } //End_for

        }

    bool MTQuadTreeDomain::inst_exists(GenericPtr instance) const {

        if (instance.is_null())
            throw std::logic_error("col::MTQuadTreeDomain::inst_exists - GenericPtr is null.");

        if (instance.index() >= entities.size()) return false;

        if (entities[instance.index()].ptr != instance) return false;

        return true;

        }

    void MTQuadTreeDomain::inst_remove(GenericPtr instance) {
        
        assert(!domain_locked);

        if (!inst_exists(instance))
            throw std::logic_error("col::MTQuadTreeDomain::inst_remove - Instance doesn't exist.");

        size_t ind = instance.index();

        auto & entity = entities[ind];

        MTQuadTreeNode * curr = entity.holder.multithr->parent;

        if (entity.holder.multithr) {
            // Holder cleanup:
            entity.holder.multithr->obj_list_erase(entity.my_iter);
            entity.holder.multithr->counter -= 1;
            entity.holder.multithr = nullptr;
            }

        for (; curr != nullptr; curr = curr->parent) {

            curr->counter -= 1;

            }

        entity.ptr = nullptr;

        }

    void MTQuadTreeDomain::pairs_recalc_start() {
        
        assert(!domain_locked);

        domain_locked = true;

        // Signal sem / condvar
        for (size_t i = 0; i < 4; i += 1)
            wsem[i].signal();

        }

    size_t MTQuadTreeDomain::pairs_recalc_join() {

        assert(domain_locked);

        pairs_hand   = 0u;
        deq_selector = 0u;

        for (size_t i = 0; i < 4; i += 1)
            dsem.wait();

        // IMPORTANT: Also visit the root node
        root->visit(pairs_deq[0]);

        clean_up();

        //root->draw(g_window);

        size_t rv = 0;

        for (size_t i = 0; i < 4u; i += 1)
            rv += pairs_deq[i].size();

        domain_locked = false;

        return rv;

        }

    size_t MTQuadTreeDomain::pairs_recalc() {
        
        assert(!domain_locked);

        pairs_recalc_start();

        return pairs_recalc_join();

        }

    bool   MTQuadTreeDomain::pairs_next(GenericPtr & inst1, GenericPtr & inst2) {
        
        assert(!domain_locked);

        RETRY:

        if (pairs_hand < pairs_deq[deq_selector].size()) {

            const auto & pair = pairs_deq[deq_selector][pairs_hand];

            inst1 = pair.first;
            inst2 = pair.second;

            pairs_hand += 1u;
            return true;

            }
        else if (deq_selector < 3u) {

            deq_selector += 1u;
            pairs_hand = 0u;

            goto RETRY;
                
            }
        else {

            return false;

            }

        }

    // Protected / Private: ///////////////////////////////////////////////////

    QuadTreeEntity & MTQuadTreeDomain::get_entity(size_t index) const {

        return const_cast<QuadTreeEntity &>( entities.at(index) );

        }

    void MTQuadTreeDomain::node_table_update(MTQuadTreeNode * node) {
        
        int sx = (int)round( (node->bbox.x - root->bbox.x) / min_width  );
        int sy = (int)round( (node->bbox.y - root->bbox.y) / min_height );

        int lx = (int)round( (node->bbox.w) / min_width  );
        int ly = (int)round( (node->bbox.h) / min_height );

        for (int i = sx; i < sx + lx; i += 1)
            for (int t = sy; t < sy + ly; t += 1)
                node_table[i][t] = node;

        }

    void MTQuadTreeDomain::clean_up() {

        cleanup_stack.clear();

        cleanup_stack.push_back(root.get());

        while (!cleanup_stack.empty()) {

            MTQuadTreeNode * node = cleanup_stack.back();
            cleanup_stack.pop_back();

            if (node->chl[0] != nullptr) {

                if (node->counter == node->objects.size() &&
                    node->parent  != nullptr) {

                    node->collapse();

                    }
                else {

                    for (int i = 3; i >= 0; i -= 1)
                        cleanup_stack.push_back(node->chl[i].get());

                    }

                }

            } //End_while

        }

    }