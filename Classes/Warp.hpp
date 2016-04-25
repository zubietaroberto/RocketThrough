#pragma once

#include "cocos2d.h"

USING_NS_CC;

class Warp{

private:

  ParticleSystem * _warp;

public:

  ParticleSystem* initWarpParticleSystem(Vec2 pRocketPosition);
  void stopWarpParticleSystem();
  
};
