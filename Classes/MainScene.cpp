#include "MainScene.h"

Scene* MainScene::createScene() {
	return MainScene::create();
}

bool MainScene::init() {
	if (!Scene::init())
		return false;
	auto VisibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	bg = Sprite::create("res/MainBG1.png");
	bg->setAnchorPoint(Vec2(0, 0));
	bg->setPosition (Vec2(origin.x, origin.y));
	this->addChild(bg, -1);
	ListernKeyBoardEvent();
	return true;
}
void MainScene::ListernKeyBoardEvent() {
	this->_eventDispatcher->removeAllEventListeners();
	auto Listener = EventListenerKeyboard::create();
	Listener->onKeyPressed = CC_CALLBACK_2(MainScene::OnKeyPressed, this);
	Listener->onKeyReleased = CC_CALLBACK_2(MainScene::OnKeyReleased, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(Listener, this);
}
void MainScene::OnKeyPressed(EventKeyboard::KeyCode code, Event* evt) {
	int ThisCode = (int)code;
	if (ThisCode == 164 || ThisCode == 35) {
		Director::getInstance()->replaceScene(GameScene::create());
	}
}
void MainScene::OnKeyReleased(EventKeyboard::KeyCode code, Event* evt) {
}
