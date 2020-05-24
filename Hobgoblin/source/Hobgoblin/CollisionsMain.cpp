
#include <deque>
#include <iostream>
#include <random>
#include <cmath>
#include <assert.h>

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/ColDetect.hpp>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

//#define DomainType hg::util:QuadTreeCollisionDomain
using hg::cd::QuadTreeCollisionDomain;
using BoundingBox = hg::cd::BoundingBox;

const size_t WINDOW_W = 1400u;
const size_t WINDOW_H = 900u;
//const size_t INST_CNT = 40'000;
const hg::PZInteger INST_CNT = 20'0;
const hg::PZInteger MON_INDEX = INST_CNT + 5;

#define SQUARE_SIZE (2.0 * 1)
#define SQUARE_DRAW_SIZE (2.f * 1)

sf::RenderWindow* g_window;

int irandom_range(int Min, int Max) {
    static std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(Min, Max);
    return distribution(generator);
}

double distance(double x1, double y1, double x2, double y2) {
#define sqr(x) ((x)*(x))
    return std::sqrt(sqr(x2 - x1) + sqr(y2 - y1));
#undef sqr
}

class RedSquare {
public:
    RedSquare() = delete;

    RedSquare(QuadTreeCollisionDomain* dom, size_t index, double x = 0.0, double y = 0.0)
        : m_bb(x, y, SQUARE_SIZE, SQUARE_SIZE)
        , m_dom(dom)
        , m_index(index)
    {
        if (m_dom) {
            cHandle = m_dom->insertEntity(hg::cd::MakeTag(m_index), m_bb, 1);
        }

        dest_x = rand(WINDOW_W);
        dest_y = rand(WINDOW_H);

        m_bb.x = rand(WINDOW_W);
        m_bb.y = rand(WINDOW_H);

        blue_cnt = 0;
        white_cnt = 0;

        done = false;
    }

    RedSquare(const RedSquare& other) {
        blue_cnt = other.blue_cnt;
        white_cnt = other.white_cnt;
        m_bb = other.m_bb;
        m_dom = other.m_dom;
        m_index = other.m_index;
        dest_x = other.dest_x;
        dest_y = other.dest_y;
        done = other.done;
    }

    void move() {
        if (done) return;

        if (distance(m_bb.x, m_bb.y, dest_x, dest_y) < 8) {
            /*auto mpos = sf::Mouse::getPosition(*g_window);

            dest_x = mpos.x + rand(WINDOW_W / 2) - WINDOW_W / 4;
            dest_y = mpos.y + rand(WINDOW_H / 2) - WINDOW_H / 4;*/

            dest_x = rand(WINDOW_W);
            dest_y = rand(WINDOW_H);
        }

        double dir = atan2(dest_y - m_bb.y, dest_x - m_bb.x);
        double spd = 5.0;

        m_bb.x += spd * cos(dir);
        m_bb.y += spd * sin(dir);

        if (m_dom) {
            cHandle.update(m_bb);
        }
    }

    void draw() {
        static sf::RectangleShape rect;

        if (done) return;

        rect.setSize({SQUARE_DRAW_SIZE, SQUARE_DRAW_SIZE});
        rect.setPosition({float(m_bb.x), float(m_bb.y)});

        if (white_cnt > 0)
            rect.setFillColor(sf::Color::White);
        else if (blue_cnt > 0)
            rect.setFillColor(sf::Color{0, 0, 255, 255});
        else
            rect.setFillColor(sf::Color::Red);

        if (blue_cnt > 0)  blue_cnt -= 1;
        if (white_cnt > 0) white_cnt -= 1;

        g_window->draw(rect);
    }

    void coll_redsquare() {
        blue_cnt = 2;

        //if (m_dom && !done) m_dom->inst_remove(qao::GenericPtr(m_index, m_index + 1));
        //done = true;

        /*if (blue_cnt >= 256)
            blue_cnt = 255;*/
    }

    void coll_monitor() {
        white_cnt = 2;

        //if (m_dom && !done) m_dom->inst_remove(qao::GenericPtr(m_index, m_index + 1));
        //done = true;
    }

protected:
    int blue_cnt;
    int white_cnt;
    BoundingBox m_bb;
    QuadTreeCollisionDomain* m_dom;
    hg::PZInteger m_index;
    double dest_x, dest_y;
    bool done;
    QuadTreeCollisionDomain::EntityHandle cHandle;

    double rand(size_t Max) {
        static const size_t pad = 32u;
        return double(irandom_range(pad, Max - pad));
    }
};

class Monitor {
public:
    Monitor(QuadTreeCollisionDomain* dom)
        : m_dom(dom)
        , m_bb(0.0, 0.0, 64.0, 64.0)
    {
        if (m_dom) {
            cHandle = m_dom->insertEntity(hg::cd::MakeTag(MON_INDEX), m_bb, 1);
        }
    }

    void move() {
        auto pos = sf::Mouse::getPosition(*g_window);

        m_bb.x = double(pos.x) - 32.0;
        m_bb.y = double(pos.y) - 32.0;

        if (m_bb.x < 0.0) m_bb.x = 0.0;
        if (m_bb.y < 0.0) m_bb.y = 0.0;

        if (m_bb.x > double(WINDOW_W) - 64.0) m_bb.x = double(WINDOW_W) - 64.0;
        if (m_bb.y > double(WINDOW_H) - 64.0) m_bb.y = double(WINDOW_H) - 64.0;

        if (m_dom) {
            cHandle.update(m_bb);
        }
    }

    void draw() {

        static sf::RectangleShape rect;

        rect.setSize({64.0, 64.0});
        rect.setPosition({float(m_bb.x), float(m_bb.y)});
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineColor(sf::Color::Yellow);
        rect.setOutlineThickness(1.0);

        g_window->draw(rect);

    }

protected:
    QuadTreeCollisionDomain* m_dom;
    BoundingBox m_bb;
    QuadTreeCollisionDomain::EntityHandle cHandle;
};

int main() {
    {
        QuadTreeCollisionDomain* domain = nullptr;
        const hg::PZInteger maxDepth = 6;
        const hg::PZInteger maxEntitiesPerNode = 10;
        QuadTreeCollisionDomain dom_impl{double(WINDOW_W), double(WINDOW_H), maxDepth, maxEntitiesPerNode, 3};
        domain = &dom_impl;

        std::deque<RedSquare> rsdeq;
        size_t index_cnt = 0u;
        Monitor mon{domain};

        for (int i = 0; i < INST_CNT; i += 1) {
            rsdeq.emplace_back(domain, size_t(i), WINDOW_W / 2, WINDOW_H / 2);
        }

        ///////////////////////////////////////////////////////////////////////////

        sf::RenderWindow window{sf::VideoMode(WINDOW_W, WINDOW_H), "Collision detection"};

        g_window = &window;
        window.setFramerateLimit(30u);
        window.setVerticalSyncEnabled(true);

        while (window.isOpen()) {

            sf::Event ev;
            while (window.pollEvent(ev)) {
                if (ev.type == sf::Event::Closed) {
                    window.close();
                    break;
                }
                // ...
            }

            // clear -> draw -> display

            window.clear(sf::Color::Black);

            for (auto& rs : rsdeq) {
                rs.move();
            }

            mon.move();

            // Collisions:
            sf::Clock clock;
            clock.restart();
            //domain->pairs_recalc_start();
            //size_t pn = domain->pairs_recalc_join();
            size_t pn = domain->recalcPairs();

            int tm = clock.getElapsedTime().asMilliseconds();

            std::cout << "\r";
            std::cout << tm << "ms ";
            std::cout << "(" << pn << " pairs)       ";

            hg::cd::CollisionPair pair;
            while (domain->pairsNext(pair)) {
                auto index1 = std::get<hg::PZInteger>(pair.first);
                auto index2 = std::get<hg::PZInteger>(pair.second);

                if (index1 != MON_INDEX && index2 != MON_INDEX) { // Both are RedSquare
                    rsdeq[index1].coll_redsquare();
                    rsdeq[index2].coll_redsquare();
                }
                else if (index1 == MON_INDEX && index2 != MON_INDEX) { // gp1 is monitor
                    rsdeq[index2].coll_monitor();
                }
                else if (index2 == MON_INDEX && index1 != MON_INDEX) { // gp2 is monitor
                    rsdeq[index1].coll_monitor();
                }
                else {
                    assert(0);
                }
            };

            domain->prune();

            domain->draw(window);

            for (auto& rs : rsdeq) {
                rs.draw();
            }

            mon.draw();

            window.display();
        }
    }
    std::cout << "\nHappy end!\n";
    std::cin.get();
}