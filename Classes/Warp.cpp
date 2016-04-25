#include "Warp.hpp"

ParticleSystem* Warp::initWarpParticleSystem(Vec2 pRocketPosition){
  _warp = ParticleSystemQuad::create("warp.plist");
	_warp->setPosition(pRocketPosition);
  return _warp;
}

void Warp::stopWarpParticleSystem(){
  _warp->stopSystem();
}

bool Warp::collision(GameSprite* pCollider){
  return (
      pow(_warp->getPositionX() - pCollider->getPositionX(), 2) +
      pow(_warp->getPositionY() - pCollider->getPositionY(), 2) <=
      pow(pCollider->getRadius()*1.2f, 2)
  );
}

void Warp::setPosition(Vec2 pNewPosition){
  _warp->setPosition(pNewPosition);
}
