#include "EndScene.h"

Scene* create() {
	return EndScene::create();
}

bool EndScene::init() {
	if (!Scene::init())
		return false;
	Label* lb = Label::createWithTTF("GameOver\nPressAnyKey to exit This Game", "fonts/Marker Felt.ttf", 40);
	lb->setPosition(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height / 2);
	this->addChild(lb);
	ListernKeyBoardEvent();

}


void EndScene::ListernKeyBoardEvent() {
	this->_eventDispatcher->removeAllEventListeners();
	auto Listener = EventListenerKeyboard::create();
	Listener->onKeyPressed = CC_CALLBACK_2(EndScene::OnKeyPressed, this);
	Listener->onKeyReleased = CC_CALLBACK_2(EndScene::OnKeyReleased, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(Listener, this);
}
void EndScene::OnKeyPressed(EventKeyboard::KeyCode code, Event* evt) {
	exit(0);
	int ThisCode = (int)code;
	if (ThisCode == 164 || ThisCode == 35) {
	}
}
void EndScene::OnKeyReleased(EventKeyboard::KeyCode code, Event* evt) {
}
