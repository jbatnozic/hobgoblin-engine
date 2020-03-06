
#include <Hobgoblin/Utility/Stopwatch.hpp>

#include <deque>
#include <iostream>
#include <SFML/Graphics.hpp>

struct PlayerInput {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
};

struct Player {
    float x, y;
    float xspeed, yspeed;
    float width, height;
    
    static constexpr float MAX_SPEED = 5.f;
    static constexpr float GRAVITY = 1.f;
    static constexpr float JUMP_POWER = 16.f;
};

constexpr int INPUT_DELAY = 6;

#define WINDOW_W int{800}
#define WINDOW_H int{400}

int main() {
    Player player = {400.f, 400.f, 0.f, 0.f, 48.f, 64.f};
    bool oldUp = false;

    std::deque<PlayerInput> inputQueue;
    inputQueue.resize(INPUT_DELAY);

    sf::RenderWindow window{sf::VideoMode(WINDOW_W, WINDOW_H), "PlatformerInputTest"};
    //window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    sf::RectangleShape rect{{player.width, player.height}};
    rect.setFillColor(sf::Color{204, 0, 204, 255});

    while (window.isOpen()) {
        // Update controls:
        inputQueue.push_back({
            sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Mouse::isButtonPressed(sf::Mouse::Left),
            sf::Keyboard::isKeyPressed(sf::Keyboard::Down),
            sf::Keyboard::isKeyPressed(sf::Keyboard::Left),
            sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
        });

        // Update player:
        const PlayerInput input = inputQueue.front();
        inputQueue.pop_front();
        

        if (player.y < static_cast<float>(WINDOW_H) - player.height) {
            player.yspeed += Player::GRAVITY;
        }
        else {
            player.y = static_cast<float>(WINDOW_H) - player.height;
            player.yspeed = 0.f;
        }

        if (input.up && !oldUp) {
            player.yspeed -= Player::JUMP_POWER;
        }

        player.xspeed = (static_cast<float>(input.right) - static_cast<float>(input.left)) * Player::MAX_SPEED;   

        player.x += player.xspeed;
        player.y += player.yspeed;

        oldUp = input.up;

        // Draw:
        window.clear();

        rect.setPosition(player.x, player.y);
        window.draw(rect);

        // Update window:
        window.display();

        // Poll window events:
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }

    return 0;
}
