#pragma once

#include "cocos2d.h"
#include "GameSprite.hpp"
#include <algorithm>
#include <random>


USING_NS_CC;

class GridContainer{

private:

  std::vector<Point> _grid;
  std::vector<Point>::size_type _gridIndex;
  std::default_random_engine randomEngine{};

public:
  GridContainer();

  void reset(Size pScreenSize, std::vector<GameSprite*> planets);
  Vec2 getNewPosition();

};
