
#include <cassert>

#include "ShipTile.hpp"
#include "Ship.hpp"

ShipTile::ShipTile(Ship* parent_, cpBody* body_, cpShape* shape_)
    : parent{parent_}
    , body{body_}
    , shape{shape_}
    , constraints{}
{
}

void ShipTile::destroyAllContraints() {
    for (int i = 0; i < Constraint::Count; i += 1) {
        constraints[i].reset();
    }
}

void ShipTile::reset(cpBody* body_, cpShape* shape_) {
    assert((body_ == nullptr && shape_ == nullptr) || (body_ != nullptr && shape_ != nullptr));
    assert(!shape_ || cpShapeGetBody(shape_) == body_);
    destroyAllContraints();
    shape.reset(shape_);
    body.reset(body_);
}

void ShipTile::setConstraint(Constraint::Enum index, cpConstraint* constraint) {
    constraints[index].reset(constraint);
}

cpConstraint* ShipTile::getConstraint(Constraint::Enum index) const noexcept {
    return constraints[index].get();
}

cpBody* ShipTile::getBody() const noexcept {
    return body.get();
}

cpShape* ShipTile::getShape() const noexcept {
    return shape.get();
}

void ShipTile::setParent(Ship* ship) {
    parent = ship;
}

Ship* ShipTile::getParent() const noexcept {
    return parent;
}

void ShipTile::update() {
    for (int i = 0; i < Constraint::Count; i += 1) {
        cpConstraint* con = constraints[i].get();
        if (con) {
            const cpFloat force = cpConstraintGetImpulse(con) * 30.0; // TODO Extract framerate
            const cpFloat maxForce = cpConstraintGetMaxForce(con);
            if (force > 0.9 * maxForce) { // TODO Extract multiplier
                constraints[i].reset();
                //destroyAllContraints(); // TODO Temp.
            }
        }
    }
}