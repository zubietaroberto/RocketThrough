#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

static Size designResolutionSize = Size(768, 1024);

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate()
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages,
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("RocketThrough", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("RocketThrough");
#endif
        director->setOpenGLView(glview);
    }

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // Set the design resolution
    glview->setDesignResolutionSize(
      designResolutionSize.width, designResolutionSize.height,
      ResolutionPolicy::EXACT_FIT
    );

    Size frameSize = glview->getFrameSize();
    std::vector<std::string> searchPaths;
    float screenRatio = frameSize.height / frameSize.width;

    if (frameSize.width > 768){
      searchPaths.push_back("ipadhd");
      director->setContentScaleFactor(frameSize.height/designResolutionSize.height);
    } else if (frameSize.width > 320) {
      if (screenRatio >= 1.5f){
        searchPaths.push_back("iphonehd");
      } else {
        searchPaths.push_back("ipad");
      }
      director->setContentScaleFactor(frameSize.height/designResolutionSize.height);
    } else {
      searchPaths.push_back("iphone");
      director->setContentScaleFactor(frameSize.height/designResolutionSize.height);
    }
    FileUtils::getInstance()->setSearchPaths(searchPaths);

    // Preload sounds
    auto audioEngine = SimpleAudioEngine::getInstance();
    audioEngine->preloadBackgroundMusic("background.mp3");
    audioEngine->preloadEffect("pickup.wav");
    audioEngine->preloadEffect("bombRelease.wav");
    audioEngine->preloadEffect("rocket.wav");
    audioEngine->preloadEffect("shipBoom.wav");
    audioEngine->setBackgroundMusicVolume(0.4f);
    audioEngine->setEffectsVolume(0.5f);

    // turn on display FPS
    director->setDisplayStats(false);

    register_all_packages();

    // create a scene. it's an autorelease object
    auto scene = HelloWorld::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
