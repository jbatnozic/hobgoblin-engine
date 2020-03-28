#pragma once

#include "GenericPtr.hpp"
#include "BoundingBox.hpp"
#include "QuadTreeEntity.hpp"

#include <vector>
#include <deque>
#include <memory>
#include <thread>

namespace col {

    using qao::GenericPtr;

    struct QuadTreeEntity;
    class  QuadTreeNode;

    typedef std::deque< std::pair<GenericPtr, GenericPtr> > PairsContainer;

    class QuadTreeDomain {

    public:

        const double X, Y;
        const double WIDTH, HEIGHT;

        const size_t MAX_DEPTH;
        const size_t MAX_OBJECTS;

        QuadTreeDomain() = delete;

        QuadTreeDomain(double x, double y, double w, double h, size_t maxdep, size_t maxobj);

        ~QuadTreeDomain();

        // Main functionality:
        void clear();

        void inst_insert(GenericPtr instance, const BoundingBox & bb);
        void inst_update(GenericPtr instance, const BoundingBox & bb);
        bool inst_exists(GenericPtr instance) const;
        void inst_remove(GenericPtr instance);

        size_t pairs_recalc();
        void   pairs_recalc_start();
        size_t pairs_recalc_join();
        bool   pairs_next(GenericPtr & inst1, GenericPtr & inst2);

        // Scanning - Point:
        GenericPtr scan_point_one( GroupMask groups,
                                  double x, double y) const;

        void scan_point_vector( GroupMask groups,
                               double x, double y, std::vector<GenericPtr> & vec) const;

        // Scanning - Rectangle:
        GenericPtr scan_rect_one( GroupMask groups, bool must_envelop,
                                 double x, double y, double w, double h) const;

        void scan_point_vector( GroupMask groups, bool must_envelop,
                               double x, double y, double w, double h, std::vector<GenericPtr> & vec) const;

        // Scanning - Circle:
        GenericPtr scan_circle_one( GroupMask groups, bool must_envelop,
                                   double x, double y, double r) const;

        void scan_circle_vector( GroupMask groups, bool must_envelop,
                                double x, double y, double r, std::vector<GenericPtr> & vec) const;


    protected:

        // Copy:
        QuadTreeDomain(const QuadTreeDomain & other) = delete;
        QuadTreeDomain& operator=(const QuadTreeDomain & other) = delete;

        // Move:
        QuadTreeDomain(QuadTreeDomain && other) = delete;
        QuadTreeDomain& operator=(QuadTreeDomain && other) = delete;

    private:

        friend class QuadTreeNode;

        size_t nt_size;
        double min_width;
        double min_height;

        std::unique_ptr<QuadTreeNode> root;

        std::vector< std::vector<QuadTreeNode*> > node_table; //[x][y]

        std::deque<QuadTreeEntity> entities;

        std::vector<QuadTreeNode*> work_stack;

        PairsContainer pairs_deq;

        size_t pairs_hand;

        QuadTreeEntity & get_entity(size_t index) const;
        void node_table_update(QuadTreeNode * node);
        void clean_up();

        };

    }










