#include "Warp.hpp"

ParticleSystem* Warp::initWarpParticleSystem(Vec2 pRocketPosition){
  _warp = ParticleSystemQuad::create("warp.plist");
	_warp->setPosition(pRocketPosition);
  return _warp;
}

void Warp::stopWarpParticleSystem(){
  _warp->stopSystem();
}
