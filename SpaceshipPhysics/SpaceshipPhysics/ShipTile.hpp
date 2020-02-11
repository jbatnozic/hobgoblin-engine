#pragma once

#include <array>

#include "CppChipmunk.hpp"

class Ship;

class ShipTile {
public:
    struct Constraint {
        enum Enum {
            Above,
            Left,
            AboveLeft,
            Count
        };
    };

    ShipTile(Ship* parent_ = nullptr, cpBody* body_ = nullptr, cpShape* shape_ = nullptr);

    // Noncopyable
    ShipTile(const ShipTile& rhs) = delete;
    ShipTile& operator=(const ShipTile& rhs) = delete;

    ShipTile(ShipTile&& rhs) = default;
    ShipTile& operator=(ShipTile&& rhs) = default;

    ~ShipTile() = default;

    void destroyAllContraints();

    void reset(cpBody* body_, cpShape* shape_);

    void setConstraint(Constraint::Enum index, cpConstraint* constraint);

    cpConstraint* getConstraint(Constraint::Enum index) const noexcept;

    cpBody* getBody() const noexcept;

    cpShape* getShape() const noexcept;

    void setParent(Ship* ship);

    Ship* getParent() const noexcept;

    void update();

private:
    Ship* parent;
    BodyUniquePtr body;
    ShapeUniquePtr shape;
    std::array<ConstraintUniquePtr, Constraint::Count> constraints;
};