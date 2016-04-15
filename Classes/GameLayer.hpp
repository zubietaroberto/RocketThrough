#ifndef __GAMELAYER_HPP__
#define __GAMELAYER_HPP__


#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "GridContainer.hpp"


USING_NS_CC;
using namespace CocosDenshion;

enum {
    kBackground,
    kMiddleground,
    kForeground
};

enum {
    kSpriteRocket,
    kSpritePlanet,
    kSpriteBoost,
    kSpriteStar
};

typedef enum gamestates {
    kGameIntro,
    kGamePaused,
    kGamePlay,
    kGameOver

}GameState;

class Rocket;
class LineContainer;
class GameSprite;


class GameLayer : public Layer {


    Rocket * _rocket;
    LineContainer * _lineContainer;
    GridContainer gridContainer;

    SpriteBatchNode *_gameBatchNode;
    Label* _scoreDisplay;

    GameSprite * _pauseBtn;
    Sprite * _intro;
    Sprite *_gameOver;
    Sprite *_paused;

    ParticleSystem * _star;
    ParticleSystem * _jet;
    ParticleSystem * _boom;
    ParticleSystem * _comet;
    ParticleSystem * _pickup;
    ParticleSystem * _warp;

    std::vector<GameSprite*> planets;
    Size _screenSize;

    GameState _state;

    bool _drawing;
    bool _running;

    int _minLineLength;
    float _cometInterval;
    float _cometTimer;

    void resetGame(void);
    void resetStar(void);
    void killPlayer(void);

    void createGameScreen(void);
    void createParticles(void);

	int _score;
	float _timeBetweenPickups;

public:
    ~GameLayer(void);

    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();

    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::Scene* scene();

    void update (float dt);

    virtual void onTouchesBegan(const std::vector<Touch*>& touches, Event *event) override;
    virtual void onTouchesMoved(const std::vector<Touch*>& touches, Event *event) override;
    virtual void onTouchesEnded(const std::vector<Touch*>& touches, Event *event) override;

    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(GameLayer);


};

#endif // __GAMELAYER_HPP__
