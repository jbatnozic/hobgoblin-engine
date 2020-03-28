#pragma once

namespace col {

    typedef unsigned int GroupMask;

    class BoundingBox {

    public:

        double x, y, w, h;

        GroupMask groups;

        BoundingBox();

        BoundingBox(double x, double y, double w, double h, GroupMask groups);

        bool overlaps(const BoundingBox & other) const;

        bool enveloped_by(const BoundingBox & other) const;

        void reset(double x, double y, double w, double h, GroupMask groups);

        // Copy:
        BoundingBox(const BoundingBox &other) = default;
        BoundingBox& operator=(const BoundingBox &other) = default;

        // Move:
        BoundingBox(BoundingBox &&other) = default;
        BoundingBox& operator=(BoundingBox &&other) = default;

        };

    }
