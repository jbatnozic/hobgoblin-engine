#pragma once

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Collisions/GenericPtr.hpp>
#include <Hobgoblin/Collisions/BoundingBox.hpp>
#include <Hobgoblin/Collisions/QuadTreeEntity.hpp>
#include <Hobgoblin/Utility/Semaphore.hpp>

#include <vector>
#include <deque>
#include <memory>
#include <thread>

namespace col {
    
    using qao::GenericPtr;

    class  MTQuadTreeNode;
    
    typedef std::deque< std::pair<GenericPtr, GenericPtr> > PairsContainer;

    class MTQuadTreeDomain {
    
    public:
    
        const double X, Y;
        const double WIDTH, HEIGHT;

        const size_t MAX_DEPTH;
        const size_t MAX_OBJECTS;

        MTQuadTreeDomain() = delete;
        
        MTQuadTreeDomain(double x, double y, double w, double h, size_t maxdep, size_t maxobj);
        
        ~MTQuadTreeDomain();
        
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
        MTQuadTreeDomain(const MTQuadTreeDomain & other) = delete;
        MTQuadTreeDomain& operator=(const MTQuadTreeDomain & other) = delete;

        // Move:
        MTQuadTreeDomain(MTQuadTreeDomain && other) = delete;
        MTQuadTreeDomain& operator=(MTQuadTreeDomain && other) = delete;
    
    private:

        friend class MTQuadTreeNode;

        friend void worker_body(MTQuadTreeDomain*, size_t, int*);
    
        size_t nt_size;
        double min_width;
        double min_height;

        std::unique_ptr<MTQuadTreeNode> root;

        std::vector< std::vector<MTQuadTreeNode*> > node_table; //[x][y]

        std::deque<QuadTreeEntity> entities;

        std::vector<MTQuadTreeNode*> cleanup_stack;

        PairsContainer pairs_deq[4];

        size_t pairs_hand;
        size_t deq_selector;

        std::thread worker[4];
        int         status[4];

        hg::util::Semaphore wsem[4];
        hg::util::Semaphore dsem;

        bool domain_locked;

        QuadTreeEntity & get_entity(size_t index) const;
        void node_table_update(MTQuadTreeNode * node);
        void clean_up();
    
    };

}










