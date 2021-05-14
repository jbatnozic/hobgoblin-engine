
#include <algorithm>
#include <iostream>

#include "Experimental/Isometric_tester.hpp"
#include "GameContext/Game_context.hpp"

void IsometricTester::_eventDraw1() {
    auto& renderTarget = ctx().windowMgr.getMainRenderTexture();

    for (const auto& block : _blocks) {
        float screenX = (block.y + block.x) * 94 / 2; // width/2  == 94/2
        float screenY = (block.y - block.x /*- block.z*/) * 46 / 2; // height/2 == 48/2

        /*blockSprite.setPosition({screenX, 400 + screenY});
        renderTarget.draw(blockSprite); */
        _drawBuffer.push_back(std::make_pair(&block, 
                                             [this, screenX, screenY](sf::RenderTarget& renderTarget) {
                                                 blockSprite.setPosition({screenX, 400 + screenY});
                                                 renderTarget.draw(blockSprite); 
                                             }));
    }

    {
        float screenX = (_building.y + _building.x) * 94 / 2;
        float screenY = (_building.y - _building.x /*- _building.z + 1*/) * 46 / 2;
        _drawBuffer.push_back(std::make_pair(&_building,
                                             [this, screenX, screenY](sf::RenderTarget& renderTarget) {
                                                 buildingSprite.setPosition({screenX, 400 + screenY});
                                                 renderTarget.draw(buildingSprite);
                                             }));
    }
}

void IsometricTester::_eventDraw2() {
    std::sort(_drawBuffer.begin(), _drawBuffer.end(),
              [](const bdfp_t& bdfp1, const bdfp_t& bdfp2) {
                  const Block& block1 = *bdfp1.first;
                  const Block& block2 = *bdfp2.first;

                  const bool _1_in_front_of_2 =
                      (block1.x + block1.xSize <= block2.x) ||
                      (-block1.y + block1.ySize <= -block2.y) /*||
                      (block1.z + block1.zSize <= block2.z)*/
                      ;

                  const bool _2_in_front_of_1 =
                      (block2.x + block2.xSize <= block1.x) ||
                      (-block2.y + block2.ySize <= -block1.y) /*||
                      (block2.z + block2.zSize <= block1.z)*/
                      ;

                  bool rv = _1_in_front_of_2;

                  if (_1_in_front_of_2 == _2_in_front_of_1) {
                      rv = std::addressof(block1) < std::addressof(block2);
                  }

                  /*printf("Block (%d, %d) in_front_of Block (%d, %d): %s\n", block1.x, block1.y, block2.x, block2.y,
                         (rv ? "Yes" : "No"));*/

                  return !rv;
              });

    auto& renderTarget = ctx().windowMgr.getMainRenderTexture();

    for (const auto& pair : _drawBuffer) {
        pair.second(renderTarget);
    }

    _drawBuffer.clear();
}