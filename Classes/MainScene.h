#ifndef __MAINSCENE_H__
#define __MAINSCENE_H__

#include "cocos2d.h"
#include "GameScene.h"
#include <vector>

USING_NS_CC;

class MainScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    CREATE_FUNC(MainScene);
    
    void ListernKeyBoardEvent();
    void OnKeyPressed(EventKeyboard::KeyCode code, Event* evt);
    void OnKeyReleased(EventKeyboard::KeyCode code, Event* evt);

    Sprite* bg;
};

#endif
