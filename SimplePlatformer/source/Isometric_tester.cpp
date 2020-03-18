
#include <algorithm>
#include <iostream>

#include "Isometric_tester.hpp"
#include "Global_program_state.hpp"

void IsometricTester::eventDraw1() {
    //auto& renderTarget = global().windowMgr.appSurface;

    for (const auto& block : _blocks) {
        float screenY = (block.x + block.y) * 48 / 2;
        float screenX = (block.x - block.y) * 94 / 2;

        //blockSprite.setPosition({screenX, 400 + screenY});
        //renderTarget.draw(blockSprite); 
        _drawBuffer.push_back(std::make_pair(&block, 
                                             [this, screenX, screenY](sf::RenderTarget& renderTarget) {
                                                 blockSprite.setPosition({screenX, 400 + screenY});
                                                 renderTarget.draw(blockSprite); 
                                             }));
    }
}

void IsometricTester::eventDraw2() {
    std::sort(_drawBuffer.begin(), _drawBuffer.end(),
              [](const bdfp_t& bdfp1, const bdfp_t& bdfp2) {
                  const Block& block1 = *bdfp1.first;
                  const Block& block2 = *bdfp2.first;

                  const bool _1_in_front_of_2 =
                      (block1.x + block1.xSize <= block2.x) ||
                      (block1.y + block1.ySize <= block2.y);

                  const bool _2_in_front_of_1 =
                      (block2.x + block2.xSize <= block1.x) ||
                      (block2.y + block2.ySize <= block1.y);

                  bool rv = _1_in_front_of_2;

                  if (_1_in_front_of_2 && _2_in_front_of_1) {
                      rv = std::addressof(block1) < std::addressof(block2);
                  }

                  /*printf("Block (%d, %d) in_front_of Block (%d, %d): %s\n", block1.x, block1.y, block2.x, block2.y,
                         (rv ? "Yes" : "No"));*/

                  return rv;
              });

    auto& renderTarget = global().windowMgr.appSurface;

    for (const auto& pair : _drawBuffer) {
        pair.second(renderTarget);
    }

    _drawBuffer.clear();
}