#pragma once

#include "cocos2d.h"

USING_NS_CC;

class GameSprite : public Sprite {
public:

	CC_SYNTHESIZE(float, _radius, Radius);
	static GameSprite* createWithFrameName(const char * pszSpriteFrameName);

};

