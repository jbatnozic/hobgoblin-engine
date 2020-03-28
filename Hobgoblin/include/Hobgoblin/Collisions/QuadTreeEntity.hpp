#pragma once

#include <Hobgoblin/Collisions/BoundingBox.hpp>
#include <Hobgoblin/Collisions/GenericPtr.hpp>

#include <list>

namespace col {
    
    class QuadTreeNode;
    class MTQuadTreeNode;

    struct QuadTreeEntity {
        
        static const int REGULAR = 0;
        static const int MULTITHR = 1;

        BoundingBox     bbox;
        qao::GenericPtr ptr;

        union {
            QuadTreeNode   * regular;
            MTQuadTreeNode * multithr;
            } holder;

        int type;

        std::list<size_t>::iterator my_iter;

        QuadTreeEntity();
        QuadTreeEntity(const BoundingBox & bbox, const qao::GenericPtr & ptr);
        ~QuadTreeEntity();

        // Copy:
        QuadTreeEntity(const QuadTreeEntity & other) = delete;
        QuadTreeEntity& operator=(const QuadTreeEntity & other) = delete;

        // Move:
        QuadTreeEntity(QuadTreeEntity && other);
        QuadTreeEntity& operator=(QuadTreeEntity && other);

        };
    
    }