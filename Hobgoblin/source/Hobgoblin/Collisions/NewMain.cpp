
#include <deque>
#include <iostream>
#include <random>
#include <cmath>
#include <assert.h>

#include <Hobgoblin/Collisions/MTQuadTreeDomain.hpp>

#include <SFML\System.hpp>
#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>

// !!! ODKOMENTIRAJ KAD IMAS IMPLEMENTACIJU !!!
#define HAS_IMPL

#define DomainType MTQuadTreeDomain
const size_t WINDOW_W = 1400u;
const size_t WINDOW_H = 900u;
const size_t INST_CNT = 30000;
const size_t MON_INDEX = INST_CNT + 5;
#define THREAD_CNT  6
#define SQUARE_SIZE 0.8
#define SQUARE_DRAW_SIZE 1.f

sf::RenderWindow * g_window;

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

    RedSquare(col::DomainType * dom, size_t index, double x = 0.0, double y = 0.0)
        : m_bb(x, y, SQUARE_SIZE, SQUARE_SIZE, 1)
        , m_dom(dom)
        , m_index(index) {

        #ifdef HAS_IMPL
        if (m_dom) m_dom->inst_insert(qao::GenericPtr(m_index, m_index + 1), m_bb);
        #endif

        dest_x = rand(WINDOW_W);
        dest_y = rand(WINDOW_H);

        m_bb.x = rand(WINDOW_W);
        m_bb.y = rand(WINDOW_H);

        blue_cnt = 0;
        white_cnt = 0;

        done = false;

        }

    RedSquare(const RedSquare & other) {

        blue_cnt = other.blue_cnt;
        white_cnt = other.white_cnt;
        m_bb = other.m_bb;
        m_dom = other.m_dom;
        m_index = other.m_index;
        dest_x = other.dest_x;
        dest_y = other.dest_y;
        done   = other.done;

        }

    ~RedSquare() {

        #ifdef HAS_IMPL
        if (m_dom && !done) m_dom->inst_remove(qao::GenericPtr(m_index, m_index + 1));
        #endif

        }

    void move() {

        if (done) return;

        if (distance(m_bb.x, m_bb.y, dest_x, dest_y) < 8) {

            dest_x = rand(WINDOW_W);
            dest_y = rand(WINDOW_H);

            }

        double dir = atan2(dest_y - m_bb.y, dest_x - m_bb.x);
        double spd = 5.0;

        m_bb.x += spd * cos(dir);
        m_bb.y += spd * sin(dir);

        #ifdef HAS_IMPL
        m_dom->inst_update(qao::GenericPtr(m_index, m_index + 1), m_bb);
        #endif

        }

    void draw() {

        static sf::RectangleShape rect;

        if (done) return;

        rect.setSize({ SQUARE_DRAW_SIZE, SQUARE_DRAW_SIZE });
        rect.setPosition({ float(m_bb.x), float(m_bb.y) });

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
    col::BoundingBox m_bb;
    col::DomainType * m_dom;
    size_t m_index;
    double dest_x, dest_y;
    bool done;

    double rand(size_t Max) {

        static const size_t pad = 32u;

        return double(irandom_range(pad, Max - pad));

        }

    };

class Monitor {

public:

    Monitor(col::DomainType * dom)
        : m_dom(dom)
        , m_bb(0.0, 0.0, 64.0, 64.0, 1) {

        #ifdef HAS_IMPL
        if (m_dom) m_dom->inst_insert(qao::GenericPtr(MON_INDEX, MON_INDEX + 1), m_bb);
        #endif

        }

    ~Monitor() {

        #ifdef HAS_IMPL
        if (m_dom) m_dom->inst_remove(qao::GenericPtr(MON_INDEX, MON_INDEX + 1));
        #endif

        }

    void move() {

        auto pos = sf::Mouse::getPosition(*g_window);

        m_bb.x = double(pos.x) - 32.0;
        m_bb.y = double(pos.y) - 32.0;

        if (m_bb.x < 0.0) m_bb.x = 0.0;
        if (m_bb.y < 0.0) m_bb.y = 0.0;

        if (m_bb.x > double(WINDOW_W) - 64.0) m_bb.x = double(WINDOW_W) - 64.0;
        if (m_bb.y > double(WINDOW_H) - 64.0) m_bb.y = double(WINDOW_H) - 64.0;

        #ifdef HAS_IMPL
        if (m_dom) m_dom->inst_update(qao::GenericPtr(MON_INDEX, MON_INDEX + 1), m_bb);
        #endif

        }

    void draw() {

        static sf::RectangleShape rect;

        rect.setSize({ 64.0, 64.0 });
        rect.setPosition({ float(m_bb.x), float(m_bb.y) });
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineColor(sf::Color::Yellow);
        rect.setOutlineThickness(1.0);

        g_window->draw(rect);

        }

protected:

    col::DomainType * m_dom;
    col::BoundingBox m_bb;

    };

int main() {

    col::DomainType * domain = nullptr;
    #ifdef HAS_IMPL
    col::DomainType dom_impl{ 0.0, 0.0, double(WINDOW_W), double(WINDOW_H), 8, 8 };
    domain = &dom_impl;
    #endif
    std::deque<RedSquare> rsdeq;
    size_t index_cnt = 0u;
    Monitor mon{ domain };

    for (int i = 0; i < INST_CNT; i += 1) {

        #ifndef HAS_IMPL
        rsdeq.push_back(RedSquare{ nullptr, size_t(i), WINDOW_W / 2, WINDOW_H / 2 });
        #else
        rsdeq.emplace_back(domain, size_t(i), WINDOW_W / 2, WINDOW_H / 2);
        #endif

        }

    ///////////////////////////////////////////////////////////////////////////

    sf::RenderWindow window{ sf::VideoMode(WINDOW_W, WINDOW_H), "Collision detection" };

    g_window = &window;
    window.setFramerateLimit(60u);
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

        for (auto & rs : rsdeq) {
            rs.move();
            }

        mon.move();

        // Collisions:
        #ifdef HAS_IMPL
        sf::Clock clock;
        clock.restart();
        //domain->pairs_recalc_start();
        //size_t pn = domain->pairs_recalc_join();
        size_t pn = domain->pairs_recalc();

        int tm = clock.getElapsedTime().asMilliseconds();

        std::cout << "\r";
        std::cout << tm << "ms ";
        std::cout << "(" << pn << " pairs)       ";

        qao::GenericPtr gp1, gp2;
        while (domain->pairs_next(gp1, gp2)) {

            if (gp1.index() != MON_INDEX && gp2.index() != MON_INDEX) { // Both are RedSquare

                rsdeq[gp1.index()].coll_redsquare();
                rsdeq[gp2.index()].coll_redsquare();

                }
            else if (gp1.index() == MON_INDEX && gp2.index() != MON_INDEX) { // gp1 is monitor

                rsdeq[gp2.index()].coll_monitor();

                }
            else if (gp2.index() == MON_INDEX && gp1.index() != MON_INDEX) { // gp2 is monitor

                rsdeq[gp1.index()].coll_monitor();

                }
            else {
                assert(0);
                }

            };
        #endif

        for (auto & rs : rsdeq) {
            rs.draw();
            }

        mon.draw();

        window.display();

        }

    std::cout << "\nHappy end!\n";

    }