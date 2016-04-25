#pragma once

#include "cocos2d.h"
#include "GameSprite.hpp"

USING_NS_CC;

class Warp{

private:

  ParticleSystem * _warp;

public:

  ParticleSystem* initWarpParticleSystem(Vec2 pRocketPosition);
  void stopWarpParticleSystem();

  bool collision(GameSprite* pCollider);
  void setPosition(Vec2 pNewPosition);

};
