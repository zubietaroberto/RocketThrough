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

    CC_SAFE_RELEASE(_planets);

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
  	_screenSize = Director::sharedDirector()->getWinSize();
    _drawing = false;
    _minLineLength = _screenSize.width * 0.07f;
    _state = kGameIntro;

    createGameScreen();

    createParticles();

    createStarGrid();

    //listen for touches
    this->setTouchEnabled(true);

    //create main loop
    this->schedule(schedule_selector(GameLayer::update));



    return true;
}

void GameLayer::update (float dt) {


}

void GameLayer::onTouchesBegan(const std::vector<Touch*>& touches, Event *event){



}

void GameLayer::onTouchesMoved(const std::vector<Touch*>& touches, Event *event){



}

void GameLayer::onTouchesEnded(const std::vector<Touch*>& touches, Event *event){

    if (_state == kGameIntro) {

        _intro->setVisible(false);
        _pauseBtn->setVisible(true);
        _state = kGamePlay;
        //resetGame();
        return;

    } else if (_state == kGameOver) {

        _gameOver->setVisible(false);
        _pauseBtn->setVisible(true);
        _state = kGamePlay;
        //resetGame();
        return;

    } else if (_state == kGamePaused) {

        _pauseBtn->setDisplayFrame(SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName ("btn_pause_off.png"));
        _paused->setVisible(false);
        _state = kGamePlay;
        _running = true;
        return;
    }

    if (!_running) return;



}

void GameLayer::resetGame () {

    _rocket->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.1f));
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
    std::random_shuffle(_grid.begin(), _grid.end());
    _gridIndex = 0;

    resetStar();

    _warp->stopSystem();

    _running = true;

    auto audioEngine = SimpleAudioEngine::getInstance();
    audioEngine->playBackgroundMusic("background.mp3", true);
    audioEngine->stopAllEffects();
    audioEngine->playEffect("rocket.wav", true);
}

void GameLayer::resetStar() {

}

void GameLayer::killPlayer() {



}


void GameLayer::createGameScreen () {

    Sprite * bg = Sprite::create("bg.png");
    bg->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
    this->addChild(bg, kBackground);


    _lineContainer = LineContainer::create();
    this->addChild(_lineContainer);


    SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("sprite_sheet.plist");
    _gameBatchNode = SpriteBatchNode::create("sprite_sheet.png", 100);

    this->addChild(_gameBatchNode, kForeground);

    _rocket = Rocket::create();
    _rocket->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.1f));
    _gameBatchNode->addChild(_rocket, kForeground, kSpriteRocket);


    //add planets
    GameSprite * planet;
    _planets = Array::create();
    _planets->retain();

    planet = GameSprite::createWithFrameName("planet_1.png");
    planet->setPosition(ccp(_screenSize.width * 0.25f,
                            _screenSize.height * 0.8f));
    _gameBatchNode->addChild(planet, kBackground, kSpritePlanet);
    _planets->addObject(planet);

    planet = GameSprite::createWithFrameName("planet_2.png");
    planet->setPosition(ccp(_screenSize.width * 0.8f,
                            _screenSize.height * 0.45f));
    _gameBatchNode->addChild(planet, kBackground, kSpritePlanet);
    _planets->addObject(planet);

    planet = GameSprite::createWithFrameName("planet_3.png");
    planet->setPosition(ccp(_screenSize.width * 0.75f,
                            _screenSize.height * 0.8f));
    _gameBatchNode->addChild(planet, kBackground, kSpritePlanet);
    _planets->addObject(planet);

    planet = GameSprite::createWithFrameName("planet_4.png");
    planet->setPosition(ccp(_screenSize.width * 0.5f,
                            _screenSize.height * 0.5f));
    _gameBatchNode->addChild(planet, kBackground, kSpritePlanet);
    _planets->addObject(planet);

    planet = GameSprite::createWithFrameName("planet_5.png");
    planet->setPosition(ccp(_screenSize.width * 0.18f,
                            _screenSize.height * 0.45f));
    _gameBatchNode->addChild(planet, kBackground, kSpritePlanet);
    _planets->addObject(planet);

    planet = GameSprite::createWithFrameName("planet_6.png");
    planet->setPosition(ccp(_screenSize.width * 0.8f,
                            _screenSize.height * 0.15f));
    _gameBatchNode->addChild(planet, kBackground, kSpritePlanet);
    _planets->addObject(planet);

    planet = GameSprite::createWithFrameName("planet_7.png");
    planet->setPosition(ccp(_screenSize.width * 0.18f,
                            _screenSize.height * 0.1f));
    _gameBatchNode->addChild(planet, kBackground, kSpritePlanet);
    _planets->addObject(planet);

    Sprite * scoreLabel = Sprite::createWithSpriteFrameName("label_score.png");
    scoreLabel->setPosition(ccp(_screenSize.width * 0.4f, _screenSize.height * 0.95));
    _gameBatchNode->addChild(scoreLabel, kBackground);

    _scoreDisplay = LabelBMFont::create("0", "font.fnt", _screenSize.width * 0.5f, kCCTextAlignmentLeft);
    _scoreDisplay->setAnchorPoint(ccp(0,0.5));
    _scoreDisplay->setPosition(ccp(_screenSize.width * 0.48f, _screenSize.height * 0.95f));
    this->addChild(_scoreDisplay, kBackground);

    _pauseBtn = GameSprite::createWithFrameName("btn_pause_off.png");
    _pauseBtn->setVisible(false);
    _pauseBtn->setPosition(ccp(_screenSize.width * 0.06f, _screenSize.height * 0.95f));
    _gameBatchNode->addChild(_pauseBtn, kBackground);


    _intro = Sprite::createWithSpriteFrameName("logo.png");
    _intro->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.55f));
    CCSprite *play = CCSprite::createWithSpriteFrameName("label_play.png");
    play->setPosition(ccp(_intro->boundingBox().size.width * 0.5f, -_intro->boundingBox().size.height * 0.5f));
    _intro->addChild(play);
    _gameBatchNode->addChild(_intro, kForeground);

    _gameOver = Sprite::createWithSpriteFrameName("gameOver.png");
    _gameOver->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.55f));
    _gameOver->setVisible(false);
    _gameBatchNode->addChild(_gameOver, kForeground);

    _paused = Sprite::createWithSpriteFrameName("label_paused.png");
    _paused->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.55f));
    _paused->setVisible(false);
    _gameBatchNode->addChild(_paused, kForeground);
}


void GameLayer::createParticles() {



}

void GameLayer::createStarGrid() {
    //create grid
    float gridFrame = _screenSize.width * 0.1f;
    int tile = 32;
    int rows = (_screenSize.height - 4 * gridFrame)/tile;
    int cols = (_screenSize.width  - 2 * gridFrame)/tile;

    int count = _planets->count();
    GameSprite * planet;
    CCPoint cell;
    bool overlaps;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            cell = ccp(gridFrame + c * tile, 2 * gridFrame + r * tile);
            overlaps = false;
            for (int j = 0; j < count; j++) {
                planet = (GameSprite *) _planets->objectAtIndex(j);
                if (pow(planet->getPositionX() - cell.x, 2) + pow(planet->getPositionY() - cell.y, 2) <= pow(planet->getRadius() * 1.2f, 2)) {
                    overlaps = true;
                }
            }
            if (!overlaps) _grid.push_back(cell);
        }
    }
    CCLOG("POSSIBLE STARS: %i", _grid.size());

}
