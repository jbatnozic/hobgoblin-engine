#ifndef STRAIN_CALC_HPP
#define STRAIN_CALC_HPP

#include <array>
#include <algorithm>
#include <vector>

#include <SFML/Graphics.hpp>

#include "PhysicsCommon.hpp"
#include "RandomGen.hpp"
#include "RowMajorMatrix.hpp"

extern sf::Font font; // TEMP;

struct Tile {
    Real force_x = 0.0;
    Real force_y = 0.0;
    Real strain = 0.0;
    bool empty = true;
};

struct StrainCalc {
    using Coordinate = sf::Vector2i;
    static constexpr Real DELTA = 0.1;

    std::minstd_rand& random_engine;
    const int side;
    RowMajorMatrix<Tile> matrix;
    std::vector<Coordinate> equilize_order;
    std::array<Tile*, 4> neighbors;

    StrainCalc(int side_)
        : random_engine{FastRandomEngine()}
        , side(side_)
        , matrix{side + 2, side + 2}
        , equilize_order{}
    {
    }

    void createEquilizationOrder() {
        equilize_order.clear();
        equilize_order.reserve(side * side);
        for (int y = 1; y <= side + 1; y += 1) {
            for (int x = 1; x <= side + 1; x += 1) {
                if (!matrix.at(x, y).empty) equilize_order.push_back({x, y});
            }
        }
        equilize_order.shrink_to_fit();
    }

    void setAllForces(Real fx, Real fy) {
        for (int y = 1; y <= side + 1; y += 1) {
            for (int x = 1; x <= side + 1; x += 1) {
                if (matrix.at(x, y).empty) continue;
                matrix.at(x, y).force_x = fx;
                matrix.at(x, y).force_y = fy;
            }
        }
    }

    int equilize() {
        Real s;
        int iteration_count = 0;
        while (true) {
            s = 0.0;
            if (iteration_count % 6 == 0) { // Extract to SHUFFLE_INTERVAL
                std::shuffle(equilize_order.begin(), equilize_order.end(), random_engine);
            }
            for (const Coordinate& coord : equilize_order) {
                s = std::max(s, equilizeTile(coord));
            }
            iteration_count += 1;
            if (s < DELTA) {
                break;
            }
        }
        return iteration_count;
    }

    Real equilizeTile(const Coordinate coord) {
        //static constexpr Real DIFF_MULTIPLIER = Real{1.0 / 1.5}; // I don't know why 1/1.5 works well but it does
        Tile& target = matrix.at(coord.x, coord.y);
        Real rv = 0.0;
        int neighborCount = 0;

        #define absdiff(a, b) abs((a) - (b))

        // Left
        {
            Tile& tmp = matrix.at(coord.x - 1, coord.y);
            if (!tmp.empty) {
                /*const Real ad_x = absdiff(target.force_x, tmp.force_x) * DIFF_MULTIPLIER;
                const Real ad_y = absdiff(target.force_y, tmp.force_y) * DIFF_MULTIPLIER;
                if (ad_x > DELTA || ad_y > DELTA) {*/
                    neighbors[neighborCount++] = &tmp;
                    /*rv = std::max(rv, std::max(ad_x, ad_y));
                }*/
            }
        }
        // Right
        {
            Tile& tmp = matrix.at(coord.x, coord.y + 1);
            if (!tmp.empty) {
                /*const Real ad_x = absdiff(target.force_x, tmp.force_x) * DIFF_MULTIPLIER;
                const Real ad_y = absdiff(target.force_y, tmp.force_y) * DIFF_MULTIPLIER;
                if (ad_x > DELTA || ad_y > DELTA) {*/
                    neighbors[neighborCount++] = &tmp;
                    /*rv = std::max(rv, std::max(ad_x, ad_y));
                }*/
            }
        }
        // Above
        {
            Tile& tmp = matrix.at(coord.x, coord.y - 1);
            if (!tmp.empty) {
                /*const Real ad_x = absdiff(target.force_x, tmp.force_x) * DIFF_MULTIPLIER;
                const Real ad_y = absdiff(target.force_y, tmp.force_y) * DIFF_MULTIPLIER;
                if (ad_x > DELTA || ad_y > DELTA) {*/
                    neighbors[neighborCount++] = &tmp;
                    /*rv = std::max(rv, std::max(ad_x, ad_y));
                }*/
            }
        }
        // Below
        {
            Tile& tmp = matrix.at(coord.x, coord.y + 1);
            if (!tmp.empty) {
                /*const Real ad_x = absdiff(target.force_x, tmp.force_x) * DIFF_MULTIPLIER;
                const Real ad_y = absdiff(target.force_y, tmp.force_y) * DIFF_MULTIPLIER;
                if (ad_x > DELTA || ad_y > DELTA) {*/
                    neighbors[neighborCount++] = &tmp;
                    /*rv = std::max(rv, std::max(ad_x, ad_y));
                }*/
            }
        }
        assert(neighborCount > 0);

        #undef absdiff

        // std::shuffle(neighbors.begin(), neighbors.begin() + neighborCount, random_engine); <- Performance hog!!!
        for (int i = 0; i < neighborCount; i += 1) {
            rv = std::max(rv, equilizeNeightbors(target, *neighbors[i]));
        }

        return rv;
    }

    static Real equilizeNeightbors(Tile& t1, Tile& t2) {
        static constexpr Real DIFF_MULTIPLIER = Real{1.0 / 1.5}; // I don't know why 1/1.5 works well but it does
        // X:
        const Real diff_x = (t1.force_x - t2.force_x) * DIFF_MULTIPLIER;
        const Real abs_diff_x = abs(diff_x);
        if (abs_diff_x >= DELTA)
        {
            t1.force_x -= diff_x;
            t2.force_x += diff_x;
            const Real strain = abs(diff_x); // TODO - Strain multiplier
            t1.strain += strain;
            t2.strain += strain;
        }
        // Y:
        const Real diff_y = (t1.force_y - t2.force_y) * DIFF_MULTIPLIER;
        const Real abs_diff_y = abs(diff_y);
        if (abs_diff_y >= DELTA)
        {
            t1.force_y -= diff_y;
            t2.force_y += diff_y;
            const Real strain = abs(diff_y); // TODO - Strain multiplier
            t1.strain += strain;
            t2.strain += strain;
        }
        return std::max(abs_diff_x, abs_diff_y);
    }
        
    ///////////////////////////////////////////////////////////////////////////

    void drawSelf(sf::RenderTarget& target) {
        const int SCALE = 20;

        sf::Color col{0, 255, 255, 255};
        sf::Text text; 
        text.setFont(font);
        text.setCharacterSize(8);

        for (int y = 0; y < side + 2; y += 1) {
            for (int x = 0; x < side + 2; x += 1) {
                const Tile& tile = matrix.at(x, y);
                if (tile.empty) {
                    text.setString("E");
                    text.setPosition(x * SCALE, y * SCALE);
                    text.setFillColor(sf::Color::Yellow);
                    target.draw(text);
                    continue;
                }

                text.setString(std::to_string(int(tile.strain)));
                text.setPosition(x * SCALE, y * SCALE);
                text.setFillColor(sf::Color::Red);
                target.draw(text);

                text.setString(std::to_string(int(tile.force_y)));
                text.setPosition(x * SCALE + (side + 1) * SCALE, y * SCALE);
                text.setFillColor(sf::Color::White);
                target.draw(text);
            }
        }
    }

};

#endif // !STRAIN_CALC_HPP