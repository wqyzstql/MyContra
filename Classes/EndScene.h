#ifndef ENDSCENE
#define ENDSCENE

#include "cocos2d.h"

USING_NS_CC;

class EndScene : public Scene {
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(EndScene);

	virtual bool init();

	void ListernKeyBoardEvent();
	void OnKeyPressed(EventKeyboard::KeyCode code, Event* evt);
	void OnKeyReleased(EventKeyboard::KeyCode code, Event* evt);

};

#endif