#include <Hobgoblin/Collisions/BoundingBox.hpp>

#include <assert.h>
#include <cassert>

namespace col {

    BoundingBox::BoundingBox()
        : BoundingBox(0.0, 0.0, 0.0, 0.0, 0u) {

        }

    BoundingBox::BoundingBox(double x, double y, double w, double h, GroupMask groups) {

        reset(x, y, w, h, groups);

        }

    bool BoundingBox::overlaps(const BoundingBox & other) const {

        if ((groups & other.groups) == 0) return false;

        if (( x + w <= other.x ) || ( x >= other.x + other.w ) ||
            ( y + h <= other.y ) || ( y >= other.y + other.h )) {

            return false;

            }

        return true;

        }

    bool BoundingBox::enveloped_by(const BoundingBox & other) const {

        if ((x >= other.x) && (x + w <= other.x + other.w) &&
            (y >= other.y) && (y + h <= other.y + other.h)) {

            return true;

            }

        return false;

        }

    void BoundingBox::reset(double x, double y, double w, double h, GroupMask groups) {

        assert(w >= 0.0 && h >= 0.0);

        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        this->groups = groups;

        }

    }