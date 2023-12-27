#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Utility/Randomization.hpp>
#include <Hobgoblin/Utility/Time_utils.hpp>

#include <SFML/Graphics.hpp>

#include <chrono>
#include <optional>
#include <thread>
#include <iostream>
#include <vector>

namespace gr = hg::gr;
namespace math = hg::math;

namespace {

const auto SAMPLE_SPRITE_FILE_PATH = std::string{HG_TEST_ASSET_DIR} + "/Screenshot_1.png";

class Tester {
public:
    Tester(math::Vector2pz aAreaSize, hg::PZInteger aObjectCount)
        : _objectCount{aObjectCount}
    {
        _spritePositions.reserve(hg::pztos(aObjectCount));
        _varrPositions.reserve(hg::pztos(aObjectCount));
        for (hg::PZInteger i = 0; i < aObjectCount; i += 1) {
            {
                const auto x = hg::util::GetRandomNumber<float>(0.f, (float)aAreaSize.x);
                const auto y = hg::util::GetRandomNumber<float>(0.f, (float)aAreaSize.y);
                _spritePositions.push_back({x, y});
            }
            {
                const auto x = hg::util::GetRandomNumber<float>(0.f, (float)aAreaSize.x);
                const auto y = hg::util::GetRandomNumber<float>(0.f, (float)aAreaSize.y);
                _varrPositions.push_back({x, y});
            }
        }

        _sfTexture.loadFromFile(SAMPLE_SPRITE_FILE_PATH); // TODO: error handling
        _sfSprite.setTexture(_sfTexture);

        _hgLoader.startTexture(1024, 1024)
            ->addSprite("spr1", SAMPLE_SPRITE_FILE_PATH)
            ->finalize(gr::TexturePackingHeuristic::BestAreaFit);
        _hgSprite.emplace(_hgLoader.getMultiBlueprint("spr1").multispr());
    }

    void sfmlDraw(sf::RenderTarget& aTarget) {
        // Draw sprites
        for (hg::PZInteger i = 0; i < _objectCount; i += 1) {
            const auto pos = _spritePositions[hg::pztos(i)];
            _sfSprite.setPosition(pos.x, pos.y);
            aTarget.draw(_sfSprite);
        }

        // Draw shape
        sf::CircleShape circle{32.f, 20};
        aTarget.draw(circle);
    }

    void hgDraw(gr::Canvas& aCanvas) {
        // Draw sprites
        for (hg::PZInteger i = 0; i < _objectCount; i += 1) {
            const auto pos = _spritePositions[hg::pztos(i)];
            _hgSprite->setPosition(pos);
            aCanvas.draw(*_hgSprite);
        }

        // Draw shape
        gr::CircleShape circle{32.f, 20};
        aCanvas.draw(circle);
    }

private:
    hg::PZInteger _objectCount;

    std::vector<math::Vector2f> _spritePositions;
    std::vector<math::Vector2f> _varrPositions;

    // SFML
    sf::Texture _sfTexture;
    sf::Sprite _sfSprite;

    // HG
    gr::SpriteLoader _hgLoader;
    std::optional<gr::Multisprite> _hgSprite;
};

} // namespace

#define SECONDS 20

int main(int argc, char* argv[]) {
    Tester tester{{800, 800}, 3000};

    // SFML
    {
        sf::RenderWindow window{sf::VideoMode{800, 800}, "PerformanceTest: SFML"};
        //window.setFramerateLimit(300);
        //window.setVerticalSyncEnabled(true);

        std::chrono::microseconds totalTime{0};
        hg::PZInteger iterationCount = 0;
        hg::util::Stopwatch timer;
        while (window.isOpen() && timer.getElapsedTime() < std::chrono::seconds{SECONDS}) {
            iterationCount += 1;

            sf::Event ev;
            while (window.pollEvent(ev)) {
                if (ev.type == sf::Event::Closed) {
                    window.close();
                }
            }

            hg::util::Stopwatch stopwatch;

            window.clear();
            tester.sfmlDraw(window);
            window.display();
            
            const auto elapsedTime = stopwatch.getElapsedTime<std::chrono::microseconds>();
            totalTime += elapsedTime;
            std::cout << "Time for frame: " 
                      << elapsedTime.count() / 1000.0
                      << "ms; average time: "
                      << totalTime.count() / iterationCount / 1000.0
                      << "ms.\n";

            //std::this_thread::sleep_for(std::chrono::milliseconds{20});
        }
        std::cout << "SFML: Finished iterations in " << SECONDS << " seconds = " << iterationCount << '\n';
    }

    // HG
    {
        gr::RenderWindow window{hg::win::VideoMode{800, 800}, "PerformanceTest: Hobgoblin"};
        //window.setFramerateLimit(300);
        //window.setVerticalSyncEnabled(true);

        gr::DrawBatcher batcher{window};

        gr::Canvas& canvas = batcher;

        std::chrono::microseconds totalTime{0};
        hg::PZInteger iterationCount = 0;
        hg::util::Stopwatch timer;
        while (window.isOpen() && timer.getElapsedTime() < std::chrono::seconds{10}) {
            iterationCount += 1;

            hg::win::Event ev;
            while (window.pollEvent(ev)) {
                ev.visit(
                    [&window](hg::win::Event::Closed) {
                        window.close();
                    });
            }

            hg::util::Stopwatch stopwatch;

            window.clear();
            tester.hgDraw(canvas);
            batcher.flush();
            window.display();
            
            const auto elapsedTime = stopwatch.getElapsedTime<std::chrono::microseconds>();
            totalTime += elapsedTime;
            std::cout << "Time for frame: "
                      << elapsedTime.count() / 1000.0
                      << "ms; average time: "
                      << totalTime.count() / iterationCount / 1000.0
                      << "ms.\n";

            //std::this_thread::sleep_for(std::chrono::milliseconds{20});
        }
        std::cout << "Hobgoblin: Finished iterations in " << SECONDS << " seconds = " << iterationCount << '\n';
    }

    return EXIT_SUCCESS;
}
