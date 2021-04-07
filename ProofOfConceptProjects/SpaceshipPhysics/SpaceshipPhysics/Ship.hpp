#ifndef SHIP_HPP
#define SHIP_HPP

#include "CppChipmunk.hpp"
#include "PhysicsCommon.hpp"
#include "RowMajorMatrix.hpp"
#include "ShipTile.hpp"

#include <SFML\Graphics.hpp>

struct ShipSpec {
    cpFloat tile_size;
    int width;
    int height;
    cpFloat tile_mass;
    cpFloat init_x;
    cpFloat init_y;
    Radians init_angle;
    cpFloat max_joint_force;
};

class Ship {
public:

    explicit Ship(cpSpace* space_, const ShipSpec& spec)
        : space{space_}
        , width{spec.width}
        , height{spec.height}
        , tile_size{spec.tile_size}
        , tiles{spec.width, spec.height}
    {
        init(spec);
    }

    cpSpace* getSpace() const noexcept {
        return space;
    }

    void update() {
        for (int y = 0; y < height; y += 1) {
            for (int x = 0; x < width; x += 1) {
                ShipTile& tile = tiles.at(x, y);
                tile.update();
            }
        }
    }

    void drawSelf(sf::RenderTarget& target, sf::Color color) const {
        sf::CircleShape shape{float(tile_size / 2.0), 8u};
        shape.setFillColor(color);
        shape.setOrigin({float(tile_size / 2.0), float(tile_size / 2.0)});

        for (int y = 0; y < height; y += 1) {
            for (int x = 0; x < width; x += 1) {
                const ShipTile& tile = tiles.at(x, y);
                const cpBody* body = tile.getBody();
                const cpVect pos = cpBodyGetPosition(body);
                shape.setPosition({float(pos.x), float(pos.y)});
                shape.setRotation(cpBodyGetAngle(body) * 180.0 / PHYS_PI);
                target.draw(shape);
            }
        }

        for (int y = 0; y < height; y += 1) {
            for (int x = 0; x < width; x += 1) {
                const ShipTile& tile = tiles.at(x, y);
                drawConstraint(target, tile.getConstraint(ShipTile::Constraint::Above));
                drawConstraint(target, tile.getConstraint(ShipTile::Constraint::Left));
                drawConstraint(target, tile.getConstraint(ShipTile::Constraint::AboveLeft));
            }
        }
    }

    void drawConstraint(sf::RenderTarget& target, const cpConstraint* con) const {
        if (!con) return;

        sf::RectangleShape rect{};

        const cpBody* a = cpConstraintGetBodyA(con);
        const cpBody* b = cpConstraintGetBodyB(con);
        const cpVect posa = cpBodyGetPosition(a);
        const cpVect posb = cpBodyGetPosition(b);

        rect.setFillColor(sf::Color::Yellow);
        rect.setPosition({float(posa.x), float(posa.y)});
        rect.setSize({float(sqrt(Sqr(posa.x - posb.x) + Sqr(posa.y - posb.y))), 0.5f});
        rect.setRotation(float(atan2( posb.y - posa.y, posb.x - posa.x ) * 180.0 / PHYS_PI));
        target.draw(rect);
    }

    void power(cpVect mouseCoord, cpFloat intensity) {
        for (int x = 0; x < width; x += 1) {
            ShipTile& tile = tiles.at(x, 0);
            cpBody* body = tile.getBody();
            cpVect pos = cpBodyGetPosition(body);
            cpVect force = cpvclamp(cpv(mouseCoord.x - pos.x, mouseCoord.y - pos.y), intensity);
            cpBodyApplyForceAtWorldPoint(body, force, pos);
        }
    }

private:
    void init(const ShipSpec& spec) {
        #define CP_ZERO_VECTOR cpv(0.0, 0.0)

        const cpFloat tile_moment = cpMomentForCircle(spec.tile_mass, 0.0, spec.tile_size, cpv(0.0, 0.0));
        const cpFloat error_bias = 0.05;

        for (int y = 0; y < height; y += 1) {
            for (int x = 0; x < width; x += 1) {
                ShipTile& tile = tiles.at(x, y);
                tile.setParent(this);

                cpBody* body = cpSpaceAddBody(space, cpBodyNew(spec.tile_mass, tile_moment));
                cpShape* shape = cpSpaceAddShape(space, cpCircleShapeNew(body, spec.tile_size / 2.0, cpv(0.0, 0.0))); 
            
                cpBodySetPosition(body, tilePos(x, y, spec));
                tile.reset(body, shape);

                if (x != 0) {
                    cpConstraint* con = cpPinJointNew(body,
                                                      tiles.at(x - 1, y).getBody(),
                                                      CP_ZERO_VECTOR,
                                                      CP_ZERO_VECTOR);
                    cpConstraintSetErrorBias(con, error_bias);
                    cpConstraintSetMaxForce(con, spec.max_joint_force);
                    cpConstraintSetCollideBodies(con, cpFalse);
                    cpSpaceAddConstraint(space, con);
                    tiles.at(x, y).setConstraint(ShipTile::Constraint::Left, con);
                }

                if (y != 0) {
                    cpConstraint* con = cpPinJointNew(body,
                                                      tiles.at(x, y - 1).getBody(),
                                                      CP_ZERO_VECTOR,
                                                      CP_ZERO_VECTOR);
                    cpConstraintSetErrorBias(con, error_bias);
                    cpConstraintSetMaxForce(con, spec.max_joint_force);
                    cpConstraintSetCollideBodies(con, cpFalse);
                    cpSpaceAddConstraint(space, con);
                    tiles.at(x, y).setConstraint(ShipTile::Constraint::Above, con);
                }

                if (x != 0 && y != 0) {
                    cpConstraint* con = cpPinJointNew(body,
                                                      tiles.at(x - 1, y - 1).getBody(),
                                                      CP_ZERO_VECTOR,
                                                      CP_ZERO_VECTOR);
                    cpConstraintSetErrorBias(con, error_bias);
                    cpConstraintSetMaxForce(con, spec.max_joint_force);
                    cpConstraintSetCollideBodies(con, cpFalse);
                    cpSpaceAddConstraint(space, con);
                    tiles.at(x, y).setConstraint(ShipTile::Constraint::AboveLeft, con);
                }
            }
        }

        #undef CP_ZERO_VECTOR
    }

    cpVect tilePos(int x, int y, const ShipSpec& spec) {
        cpFloat xx = spec.tile_size * (x - spec.width / 2);
        cpFloat yy = spec.tile_size * (y - spec.height / 2);
        cpFloat theta = atan2(-yy, xx);
        cpFloat d = sqrt(Sqr(xx) + Sqr(yy));
        
        return cpv(spec.init_x + d * +cos(spec.init_angle.rad + theta), 
                   spec.init_y + d * -sin(spec.init_angle.rad + theta));
    }

    cpSpace* space;
    int width;
    int height;
    cpFloat tile_size;
    RowMajorMatrix<ShipTile> tiles;
};

#endif // !SHIP_HPP

