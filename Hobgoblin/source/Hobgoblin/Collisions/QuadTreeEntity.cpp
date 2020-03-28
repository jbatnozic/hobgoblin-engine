
#include <Hobgoblin/Collisions/QuadTreeEntity.hpp>

#include <Hobgoblin/Collisions/MTQuadTreeNode.hpp>

namespace col {
    
    QuadTreeEntity::QuadTreeEntity()
        : bbox()
        , ptr(nullptr) {

        holder.regular = nullptr;

        }
    
    QuadTreeEntity::QuadTreeEntity(const BoundingBox & bbox, const qao::GenericPtr & ptr)
        : bbox(bbox) 
        , ptr(ptr) {

        holder.regular = nullptr;

        }

    QuadTreeEntity::~QuadTreeEntity() {

        if (!holder.regular) return;

        if (type == REGULAR) {

            //STUB
            //if (holder.regular) 
            //    holder.regular->obj_list_erase(my_iter);

            }
        else /*if (type == MULTITHR)*/ {
            
            if (holder.multithr) 
                holder.multithr->obj_list_erase(my_iter);

            }

        }

    // Move:
    QuadTreeEntity::QuadTreeEntity(QuadTreeEntity && other) {
        
        bbox    = other.bbox;
        ptr     = other.ptr;
        holder  = other.holder;
        my_iter = other.my_iter;
        type    = other.type;

        other.holder.regular = nullptr;

        }

    QuadTreeEntity & QuadTreeEntity::operator=(QuadTreeEntity && other) {
        
        bbox    = other.bbox;
        ptr     = other.ptr;
        holder  = other.holder;
        my_iter = other.my_iter;
        type    = other.type;

        other.holder.regular = nullptr;

        return (*this);

        }

    }