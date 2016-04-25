/*

 Background music:
 8bit Dungeon Level by Kevin MacLeod (incompetech.com)

 */

#include "GameLayer.hpp"
#include "SimpleAudioEngine.h"
#include "Rocket.hpp"
#include "LineContainer.hpp"


USING_NS_CC;

using namespace CocosDenshion;


GameLayer::~GameLayer () {
}

Scene* GameLayer::scene()
{
    // 'scene' is an autorelease object
    Scene *scene = Scene::create();

    // 'layer' is an autorelease object
    GameLayer *layer = GameLayer::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

  	//init game values
  	_screenSize = Director::getInstance()->getWinSize();
    _drawing = false;
    _minLineLength = _screenSize.width * 0.07f;
    _state = kGameIntro;

    createGameScreen();

    createParticles();

    // Build Multitouch event Listener
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(GameLayer::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(GameLayer::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(GameLayer::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    //create main loop
    this->schedule(schedule_selector(GameLayer::update));



    return true;
}

void GameLayer::update (float dt) {

  if (!_running || _state != kGamePlay) return;

  if (_lineContainer->getLineType() != LINE_NONE){
    _lineContainer->setTip(_rocket->getPosition());
  }

  if (_rocket->collidedWithSides()){
    _lineContainer->setLineType(LINE_NONE);
  }

  _rocket->update(dt);

  //Update jet particle so that it follows the rocket
  if (!_jet->isActive()) _jet->resetSystem();
  _jet->setRotation(_rocket->getRotation());
  _jet->setPosition(_rocket->getPosition());

  _cometTimer += dt;
  float newY;
  if (_cometTimer > _cometInterval){
    _cometTimer = 0;
    if (!_comet->isVisible()){
      _comet->setPositionX(0);
      newY = (float) rand()/
        ((float) RAND_MAX/_screenSize.height*0.6f) + _screenSize.height * 0.2f;

      _comet->setPositionY(MIN(newY, _screenSize.height * 0.9f));
      _comet->setVisible(true);
      _comet->resetSystem();
    }
  }

  if (_comet->isVisible()){

    //Collision with comet
    if (
      pow(_comet->getPositionX() - _rocket->getPositionX(), 2) +
      pow(_comet->getPositionY() - _rocket->getPositionY(), 2) <=
      pow(_rocket->getRadius(), 2)
    ){

      if (_rocket->isVisible()) killPlayer();
    }

    _comet->setPositionX(_comet->getPositionX() + 50 * dt);
    if (_comet->getPositionX() > _screenSize.width * 1.5f){
      _comet->stopSystem();
      _comet->setVisible(false);
    }
  }

  _lineContainer->update(dt);
  _rocket->setOpacity(_lineContainer->getEnergy() * 255);

  // Collision with the planets
  for (auto planet: planets){
    if(
      pow(planet->getPositionX() - _rocket->getPositionX(), 2) +
      pow(planet->getPositionY() - _rocket->getPositionY(), 2) <=
      pow(_rocket->getRadius() * 0.8f + planet->getRadius()+0.65f, 2)
    ){
      if (_rocket->isVisible()) killPlayer();
      break;
    }
  }

  // Collision with the star
  if (
    pow(_star->getPositionX() - _rocket->getPositionX(), 2) +
    pow(_star->getPositionY() - _rocket->getPositionY(), 2) <=
    pow(_rocket->getRadius()*1.2f, 2)
  ){
    _pickup->setPosition(_star->getPosition());
    _pickup->resetSystem();

    if (_lineContainer->getEnergy() + 0.25f < 1){
      _lineContainer->setEnergy(_lineContainer->getEnergy() +0.25f );
    } else {
      _lineContainer->setEnergy(1.0f);
    }

    _rocket->setSpeed(_rocket->getSpeed() + 2);

    if (_rocket->getSpeed() > 70) _rocket->setSpeed(70);

    _lineContainer->setEnergyDecrement(0.0002f);
    SimpleAudioEngine::getInstance()->playEffect("pickup.wav");
    resetStar();

    int points = 100 - _timeBetweenPickups;

    if (points < 0) points = 0;

    _score += points;
    _scoreDisplay->setString(String::createWithFormat("%i", _score)->getCString());
    _timeBetweenPickups = 0;

  }

  _timeBetweenPickups += dt;
  if (_lineContainer->getEnergy() == 0){

    if (_rocket->isVisible()) killPlayer();

  }

  // Collision with a Warp
  if (warp.collision(_rocket)){

    //Rocket has collided. Teleport
    _rocket->setPosition(gridContainer.getNewStarPosition(_rocket->getPosition()));
    warp.setPosition(gridContainer.getNewCell());

  }

}

void GameLayer::onTouchesBegan(const std::vector<Touch*>& touches, Event *event){

  if (!_running) return;
  if (touches.empty()) return;

  Point tap = touches.front()->getLocation();
  float dx = _rocket->getPositionX() - tap.x;
  float dy = _rocket->getPositionY() - tap.y;

  if(dx*dx + dy*dy <= pow(_rocket->getRadius(), 2)){
    _lineContainer->setLineType(LINE_NONE);
    _rocket->setRotationOrientation(ROTATE_NONE);
    _drawing = true;
  }

}

void GameLayer::onTouchesMoved(const std::vector<Touch*>& touches, Event *event){

  if (!_running) return;
  if (touches.empty()) return;

  if (_drawing){
    Point tap = touches.front()->getLocation();
    float dx = _rocket->getPositionX() - tap.x;
    float dy = _rocket->getPositionY() - tap.y;

    if (dx*dx + dy*dy > pow(_minLineLength, 2)){
      _rocket->select(true);
      _lineContainer->setPivot(tap);
      _lineContainer->setLineType(LINE_TEMP);
    } else {
      _rocket->select(false);
      _lineContainer->setLineType(LINE_NONE);
    }

  }
}

void GameLayer::onTouchesEnded(const std::vector<Touch*>& touches, Event *event){

    if (_state == kGameIntro) {

        _intro->setVisible(false);
        _pauseBtn->setVisible(true);
        _state = kGamePlay;
        resetGame();
        return;

    } else if (_state == kGameOver) {

        _gameOver->setVisible(false);
        _pauseBtn->setVisible(true);
        _state = kGamePlay;
        resetGame();
        return;

    } else if (_state == kGamePaused) {

        _pauseBtn->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("btn_pause_off.png"));
        _paused->setVisible(false);
        _state = kGamePlay;
        _running = true;
        return;
    }

    if (!_running) return;
    if (touches.empty()) return;

    Point tap = touches.front()->getLocation();
    if (_pauseBtn->getBoundingBox().containsPoint(tap)){
      _paused->setVisible(true);
      _state = kGamePaused;
      _pauseBtn->setSpriteFrame(
        SpriteFrameCache::getInstance()->getSpriteFrameByName("btn_pause_on.png")
      );
      _running = false;
      return;
    }

    _drawing = false;
    _rocket->select(false);

    if(_lineContainer->getLineType() == LINE_TEMP){
      _lineContainer->setPivot(tap);
      _lineContainer->setLineLength(_rocket->getPosition().distance(tap));
      _rocket->setPivot(tap);

      float circle_length = _lineContainer->getLineLength() * 2 * M_PI;
      int iterations = floor(circle_length / _rocket->getSpeed() );
      _rocket->setAngularSpeed(2*M_PI/iterations);

      Vec2 diff = _rocket->getPosition();
      diff.subtract(_rocket->getPivot());
      Point clockwise = diff.getRPerp();
      float dot = clockwise.dot(_rocket->getVector());
      if (dot > 0){
        _rocket->setAngularSpeed(_rocket->getAngularSpeed()* -1);
        _rocket->setRotationOrientation(ROTATE_CLOCKWISE);
        _rocket->setTargetRotation(CC_RADIANS_TO_DEGREES(
          atan2(clockwise.y, clockwise.x))
        );
      } else {
        _rocket->setRotationOrientation(ROTATE_COUNTER);
        _rocket->setTargetRotation(CC_RADIANS_TO_DEGREES(
          atan2(-1*clockwise.y, -1*clockwise.x))
        );
        _lineContainer->setLineType(LINE_DASHED);
      }
    }
}

void GameLayer::resetGame () {

    _rocket->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.1f));
    _rocket->setOpacity(255);
    _rocket->setVisible(true);
    _rocket->reset();

    _cometInterval = 4;
    _cometTimer = 0;
    _timeBetweenPickups = 0.0;

    _score = 0;
    char szValue[100] = {0};
    sprintf(szValue, "%i", _score);
    _scoreDisplay->setString(szValue);

    _lineContainer->reset();

    //shuffle grid cells
    gridContainer.reset(_screenSize, planets);

    resetStar();

    // Reset Warp
    warp.setPosition(gridContainer.getNewCell());

    _running = true;

    auto audioEngine = SimpleAudioEngine::getInstance();
    audioEngine->playBackgroundMusic("background.mp3", true);
    audioEngine->stopAllEffects();
    audioEngine->playEffect("rocket.wav", true);
}

void GameLayer::resetStar() {

  _star->setPosition(gridContainer.getNewStarPosition(_rocket->getPosition()));
  _star->setVisible(true);
  _star->resetSystem();

}

void GameLayer::killPlayer() {

  SimpleAudioEngine::getInstance()->stopBackgroundMusic();
  SimpleAudioEngine::getInstance()->stopAllEffects();
  SimpleAudioEngine::getInstance()->playEffect("shipBoom.wav");

  _boom->setPosition(_rocket->getPosition());
  _boom->resetSystem();
  _rocket->setVisible(false);
  _jet->stopSystem();
  _lineContainer->setLineType(LINE_NONE);
  _running = false;
  _state = kGameOver;
  _gameOver->setVisible(true);
  _pauseBtn->setVisible(false);

}


void GameLayer::createGameScreen () {

    Sprite * bg = Sprite::create("bg.png");
    bg->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
    this->addChild(bg, kBackground);


    _lineContainer = LineContainer::create();
    this->addChild(_lineContainer);


    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sprite_sheet.plist");
    _gameBatchNode = SpriteBatchNode::create("sprite_sheet.png", 100);

    this->addChild(_gameBatchNode, kForeground);

    _rocket = Rocket::create();
    _rocket->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.1f));
    _gameBatchNode->addChild(_rocket, kForeground, kSpriteRocket);


    //add planets
    GameSprite * planet;
    planet = GameSprite::createWithFrameName("planet_1.png");
    planet->setPosition(Vec2(_screenSize.width * 0.25f,
                            _screenSize.height * 0.8f));
    _gameBatchNode->addChild(planet, kBackground, kSpritePlanet);
    planets.push_back(planet);

    planet = GameSprite::createWithFrameName("planet_2.png");
    planet->setPosition(Vec2(_screenSize.width * 0.8f,
                            _screenSize.height * 0.45f));
    _gameBatchNode->addChild(planet, kBackground, kSpritePlanet);
    planets.push_back(planet);

    planet = GameSprite::createWithFrameName("planet_3.png");
    planet->setPosition(Vec2(_screenSize.width * 0.75f,
                            _screenSize.height * 0.8f));
    _gameBatchNode->addChild(planet, kBackground, kSpritePlanet);
    planets.push_back(planet);

    planet = GameSprite::createWithFrameName("planet_4.png");
    planet->setPosition(Vec2(_screenSize.width * 0.5f,
                            _screenSize.height * 0.5f));
    _gameBatchNode->addChild(planet, kBackground, kSpritePlanet);
    planets.push_back(planet);

    planet = GameSprite::createWithFrameName("planet_5.png");
    planet->setPosition(Vec2(_screenSize.width * 0.18f,
                            _screenSize.height * 0.45f));
    _gameBatchNode->addChild(planet, kBackground, kSpritePlanet);
    planets.push_back(planet);

    planet = GameSprite::createWithFrameName("planet_6.png");
    planet->setPosition(Vec2(_screenSize.width * 0.8f,
                            _screenSize.height * 0.15f));
    _gameBatchNode->addChild(planet, kBackground, kSpritePlanet);
    planets.push_back(planet);

    planet = GameSprite::createWithFrameName("planet_7.png");
    planet->setPosition(Vec2(_screenSize.width * 0.18f,
                            _screenSize.height * 0.1f));
    _gameBatchNode->addChild(planet, kBackground, kSpritePlanet);
    planets.push_back(planet);

    Sprite * scoreLabel = Sprite::createWithSpriteFrameName("label_score.png");
    scoreLabel->setPosition(Vec2(_screenSize.width * 0.4f, _screenSize.height * 0.95));
    _gameBatchNode->addChild(scoreLabel, kBackground);

    _scoreDisplay = Label::createWithBMFont("font.fnt", "0", TextHAlignment::LEFT,  _screenSize.width * 0.5f);
    _scoreDisplay->setAnchorPoint(Vec2(0,0.5));
    _scoreDisplay->setPosition(Vec2(_screenSize.width * 0.48f, _screenSize.height * 0.95f));
    this->addChild(_scoreDisplay, kBackground);

    _pauseBtn = GameSprite::createWithFrameName("btn_pause_off.png");
    _pauseBtn->setVisible(false);
    _pauseBtn->setPosition(Vec2(_screenSize.width * 0.06f, _screenSize.height * 0.95f));
    _gameBatchNode->addChild(_pauseBtn, kBackground);


    _intro = Sprite::createWithSpriteFrameName("logo.png");
    _intro->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.55f));
    Sprite *play = Sprite::createWithSpriteFrameName("label_play.png");
    play->setPosition(
      Vec2(
        _intro->getBoundingBox().size.width * 0.5f,
        -_intro->getBoundingBox().size.height * 0.5f)
      );
    _intro->addChild(play);
    _gameBatchNode->addChild(_intro, kForeground);

    _gameOver = Sprite::createWithSpriteFrameName("gameOver.png");
    _gameOver->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.55f));
    _gameOver->setVisible(false);
    _gameBatchNode->addChild(_gameOver, kForeground);

    _paused = Sprite::createWithSpriteFrameName("label_paused.png");
    _paused->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.55f));
    _paused->setVisible(false);
    _gameBatchNode->addChild(_paused, kForeground);
}


void GameLayer::createParticles() {

	_jet = ParticleSystemQuad::create("jet.plist");
	_jet->setSourcePosition(Vec2(-_rocket->getRadius()*0.08f, 0));
	_jet->setAngle(180);
	_jet->stopSystem();
	this->addChild(_jet, kBackground);

	_boom = ParticleSystemQuad::create("boom.plist");
	_boom->stopSystem();
	this->addChild(_boom, kForeground);

	_comet = ParticleSystemQuad::create("comet.plist");
	_comet->stopSystem();
	_comet->setPosition(Vec2(0, _screenSize.height*0.6f));
	_comet->setVisible(false);
	this->addChild(_comet, kForeground);

	_pickup = ParticleSystemQuad::create("plink.plist");
	_pickup->stopSystem();
	this->addChild(_pickup, kMiddleground);

  auto warpPS = warp.initWarpParticleSystem(_rocket->getPosition());
	this->addChild(warpPS, kBackground);

	_star = ParticleSystemQuad::create("star.plist");
	_star->stopSystem();
	_star->setVisible(false);
	this->addChild(_star, kBackground, kSpriteStar);
}
