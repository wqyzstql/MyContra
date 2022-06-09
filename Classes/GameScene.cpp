#include "GameScene.h"
#include <fstream>
#include "EndScene.h"

inline bool IsExist(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}
Scene* GameScene::createScene() {
	return GameScene::create();
}
bool GameScene::init() {
	if (!Scene::init())
		return false;
	VisuableSize = Director::getInstance()->getVisibleSize();

	map = TMXTiledMap::create("res/MAP.tmx");
	_meta = map->getLayer("meta");
	_meta->setVisible(0);
	_meta2 = map->getLayer("meta2");
	_meta2->setVisible(0);
	
	Life = 9;
	NowLife = Label::createWithTTF("Life:9", "fonts/Marker Felt.ttf", 48);
	NowLife->setPosition(Vec2(100, 700));
	NowScore = Label::createWithTTF("Score:00000000", "fonts/Marker Felt.ttf", 48);
	NowScore->setPosition(Vec2(850, 700));

	Invt = IniInvincibleTime;
	_player = Sprite::create("idle.png");
	_player->setAnchorPoint(Vec2(0, 0));
	_player->setFlippedX(Left ? 0 : 1);
	idlee = SpriteFrame::create("idle.png", Rect(0, 0, 72, 81));
	
	monster = map->getObjectGroup("monster");
	for (int i = 0;; i++) {
		char txt[100] = {};
		sprintf_s(txt, "monster%d", i);
		auto msw = monster->getObject(std::string(txt));
		if (msw.empty())
			break;
		monspw.push_back(Vec4(msw["x"].asFloat() , msw["y"].asFloat(), msw["type"].asInt(),0));
	}
	obj = map->getObjectGroup("spawn");
	auto spw = obj->getObject("SpawnPoint");
	
	pos = Vec2(spw["x"].asInt(), spw["y"].asInt());
	_player->setPosition(pos);
	map->setAnchorPoint(Vec2(0, 0));
	map->setPosition(Vec2(0, 0));
	
	this->addChild(map);
	this->addChild(_player);
	lb = Label::createWithTTF("zDebugInf", "fonts/Marker Felt.ttf", 48);
	lb->setAnchorPoint(Vec2(0, 0));
	lb->setPosition(200, 40);
	this->addChild(lb);
	this->addChild(NowLife);
	this->addChild(NowScore);

	KEEPLEFT = 0;
	KEEPRIGHT = 1;

	ListernKeyboardEvent();
	this->scheduleUpdate();
	return true;
}
void GameScene::idle() {
	_player->setTexture("idle.png");
}

void GameScene::ListernKeyboardEvent() {
	EventListenerKeyboard* listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(GameScene::OnKeyPressed, this);
	listener->onKeyReleased = CC_CALLBACK_2(GameScene::OnKeyReleased, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
void GameScene::OnKeyPressed(EventKeyboard::KeyCode code, Event* evt) {
	if (IsDeath)
		return;
	int num = (int)code;
	char txt[100] = {};
	sprintf_s(txt, "Key %d is pressed!", num);
	lb->setString(txt);
	if (num == 164 || num == 35) {
		if (DestoryBoss) {
			EndScene* scene = EndScene::create();
			Director::getInstance()->replaceScene(scene);
		}
	}
	if (num == 126) {//jump
		if (DownDown == 1) {
			float gd = 1.0 / 60 * g;
			Vec2 p = _player->getPosition();
			p.y -= gd;
			if (p.y < 0 || p.y>VisuableSize.y) {
				int NowX = _player->getPositionX();
				_player->setPosition(Vec2(NowX, pos.y));
				return;
			}
			CCPoint tmp = p;
			tmp.x -= map->getPositionX();
			CCPoint titleCoop = ExchangeToMap(tmp);
			int titlegid = _meta->tileGIDAt(titleCoop);
			if (titlegid) {
				float nowy = _player->getPositionY();
				if (tmp.y < nowy) {
					Down = 1;
					while (1) {
						_player->setPosition(p);
						float gd = 1.0 / 60 * g;
						p = _player->getPosition();
						p.y -= gd;
						CCPoint tmp = p;
						tmp.x -= map->getPositionX();
						CCPoint titleCoop = ExchangeToMap(tmp);
						int titlegid = _meta->tileGIDAt(titleCoop);
						if (!titlegid)	break;
					}

				}
			}
			DownDown = 0;
		}
		if (!IsJump && !Down && !pa) {
			JumpTargetY = _player->getPositionY() + JumpHigh;
			_player->stopAllActions();
			jump();
			IsJump = 1;
		}
	}
	if (num == 147) {//fire
		//fire1 = 1;
		OpenFire();
	}
	if (num == 26) {//Left
		KeyLeft = 1;
		Left = 1;
		Right = 0;
		KEEPLEFT = 1;
		KEEPRIGHT = 0;
		if (!IsJump&&!Down&&!pa) {
			RunAnimation();
		}
	}
	else if (num == 27) {//Right
		KeyRight = 1;
		Right = 1;
		Left = 0;
		KEEPRIGHT = 1;
		KEEPLEFT = 0;
		if (!IsJump && !Down&&!pa) {
			RunAnimation();
		}
	}
	else if (num == 28) {//Up
		if (!LookUp && !IsJump && !Down&&!Left&&!Right) {
			LookUp = 1;
			_player->setTexture("res/player/lookup.png");
		}
		else if (!IsJump && !Down && (Left || Right)) {
			RunUp = 1;
			RunUpAnimation();
		}
	}
	else if (num == 29) {//Down
		if (!IsJump && !Down && (Left || Right)) {
			DownDown = 1;
			RunDown = 1;
			RunDownAnimation();
		}
		else if (!IsJump && !Down) {
			DownDown = 1;
			Left = Right = 0;
			if (_player->numberOfRunningActions() > 0)
				_player->stopAllActions();
			pa = 1;
			_player->setTexture("res/player/down.png");
		}
	}
}
void GameScene::OnKeyReleased(EventKeyboard::KeyCode code, Event* evt) {
	if (IsDeath)
		return;
	int num = (int)code;
	if (num == 147) {
		fire1 = 0;
	}
	if (num == 26) {
		KeyLeft = 0;
		Left = 0;
		if (!IsJump&&!pa) {
			_player->stopAction(run);
			idle();
		}
	}
	if (num == 27) {
		KeyRight = 0;
		Right = 0;
		if (!IsJump&&!pa) {
			_player->stopAction(run);
			idle();
		}
	}
	if (num == 28) {
		if (LookUp) {
			LookUp = 0;
			idle();
		}
		if (RunUp == 1) {
			RunUp = 0;
			RunAnimation();
		}
	}
	if (num == 29) {
		DownDown = 0;
		if (pa) {
			pa = 0;
			idle();
		}
		if (RunDown == 1) {
			RunDown = 0;
			RunAnimation();
		}
	}
}

void GameScene :: inital() {
	Left = 0, Right = 0, IsJump = 0, Down = 0, pa = 0;
	DownDown = 0, KeyLeft = 0, KeyRight = 0;

	KEEPLEFT = 0, KEEPRIGHT = 1;
	LookUp = 0, RunUp = 0, RunDown = 0;

	fire1 = 0;
}

// ANIMATION
void GameScene::DeathAnimation() {
	_player->stopAllActions();
	Vector<SpriteFrame*> allFrames;
	char txt[100] = {};
	for (int i = 1; i <= 5; i++) {
		sprintf_s(txt, "res/player/death_000%d.png", i);
		Sprite* tmp = Sprite::create(txt);
		SpriteFrame* sf = SpriteFrame::create(txt, Rect(0, 0, tmp->getContentSize().width, tmp->getContentSize().height));
		allFrames.pushBack(sf);
	}
	Animation* ani = Animation::createWithSpriteFrames(allFrames, 0.1f);
	_player->runAction(Animate::create(ani));
}
void GameScene::RunUpAnimation() {
	_player->stopAllActions();
	Vector<SpriteFrame*> allFrames;
	char txt[100] = {};
	for (int i = 1; i <= 3; i++) {
		sprintf_s(txt, "res/player/run_up000%d.png", i);
		Sprite* tmp = Sprite::create(txt);
		SpriteFrame* sf = SpriteFrame::create(txt, Rect(0, 0, tmp->getContentSize().width, tmp->getContentSize().height));
		allFrames.pushBack(sf);
	}
	Animation* ani = Animation::createWithSpriteFrames(allFrames, 0.1f);
	_player->runAction(RepeatForever::create(Animate::create(ani)));
}
void GameScene::RunDownAnimation() {
	_player->stopAllActions();
	Vector<SpriteFrame*> allFrames;
	char txt[100] = {};
	for (int i = 1; i <= 3; i++) {
		sprintf_s(txt, "res/player/run_down000%d.png", i);
		Sprite* tmp = Sprite::create(txt);
		SpriteFrame* sf = SpriteFrame::create(txt, Rect(0, 0, tmp->getContentSize().width, tmp->getContentSize().height));
		allFrames.pushBack(sf);
	}
	Animation* ani = Animation::createWithSpriteFrames(allFrames, 0.1f);
	_player->runAction(RepeatForever::create(Animate::create(ani)));
}
void GameScene::RunAnimation() {
	_player->stopAllActions();
	Vector<SpriteFrame*> allFrames;
	char txt[100] = {};
	for (int i = 2; i <= 4; i++) {
		sprintf_s(txt, "res/player/run_left000%d.png", i);
		Sprite* tmp = Sprite::create(txt);
		SpriteFrame* sf = SpriteFrame::create(txt, Rect(0, 0, tmp->getContentSize().width, tmp->getContentSize().height));
		allFrames.pushBack(sf);
	}
	Animation* ani = Animation::createWithSpriteFrames(allFrames, 0.1f);
	run = _player->runAction(RepeatForever::create(Animate::create(ani)));
}
void GameScene::jump() {
	_player->stopAllActions();
	Vector<SpriteFrame*> allFrames;
	char txt[100] = {};
	for (int i = 1; i < 4; i++) {
		sprintf_s(txt, "res/player/jump000%d.png", i);
		Sprite* tmp = Sprite::create(txt);
		SpriteFrame* sf = SpriteFrame::create(txt, Rect(0, 0, tmp->getContentSize().width, tmp->getContentSize().height));
		allFrames.pushBack(sf);
	}
	Animation* ani = Animation::createWithSpriteFrames(allFrames, 0.1f);
	jumpaction = _player->runAction(RepeatForever::create(Animate::create(ani)));
}



void GameScene::Gravity(float dt) {
	if (IsDeath)
		return;
	float gd = dt * g;
	Vec2 gg = _player->getPosition();
	gg.y -= gd;
	if (gg.y < 0 || gg.y>VisuableSize.y) {
		Invt = IniInvincibleTime;
		_player->stopAllActions();
		inital();
		DeathAnimation();
		deat = IniDeathTime;
		IsDeath = 1;
		return;
	}
	SetGPlayerPosition(gg);
}
CCPoint GameScene::ExchangeToMap(CCPoint p) {
	mapSize = map->getMapSize();
	tileSize = map->getTileSize();
	int x = p.x / tileSize.width;
	int y = (mapSize.height * tileSize.height - p.y) / tileSize.height;
	return Vec2(x, y);
}
void GameScene::SetPlayerPosition(CCPoint p) {
	CCPoint tmp = p;
	tmp.x -= map->getPositionX();
	CCPoint titleCoop = ExchangeToMap(tmp);
	int titlegid = _meta->tileGIDAt(titleCoop);
	int til2 = _meta2->tileGIDAt(titleCoop);
	if (til2)
		return;
	if (titlegid) {
		float nowy = _player->getPositionY();
		if (tmp.y < nowy) {
			return;
		}
	}
	_player->setPosition(p);
}
void GameScene::SetGPlayerPosition(CCPoint p) {
	CCPoint tmp = p;
	tmp.x -= map->getPositionX();
	CCPoint titleCoop = ExchangeToMap(tmp);
	int titlegid = _meta->tileGIDAt(titleCoop);
	int til2 = _meta2->tileGIDAt(titleCoop);
	if (til2)
		return;
	if (titlegid) {
		float nowy = _player->getPositionY();
		if (tmp.y < nowy) {
			if (Down == 1)
				idle();
			Down = 0;
			return;
		}
	}
	Down = 1;
	_player->setPosition(p);
}

void GameScene::OpenFire() {
	Sprite* AnewBullet = Sprite::create("res/bullet.png");
	Vec2 pos = _player->getPosition();

	int dx = 1, dy = 0;

	if (LookUp) {
		dx = 0;
		dy = 1;
		AnewBullet->setPosition(Vec2(pos.x + 33, pos.y + 86));
	}
	else if (KEEPRIGHT) {
		dx = 1; dy = 0;
		if (!pa) {
			if (!IsJump && !Down) {
				if (RunUp) {
					dy = 1;
					AnewBullet->setPosition(Vec2(pos.x + 61, pos.y + 72));
				}
				else if (RunDown) {
					dy = -1;
					AnewBullet->setPosition(Vec2(pos.x + 61, pos.y + 38));
				}
				else 
					AnewBullet->setPosition(Vec2(pos.x + 71, pos.y + 53));
					
			}
			else if(IsJump)
				AnewBullet->setPosition(Vec2(pos.x + 50, pos.y + 27));
			else if(Down)
				AnewBullet->setPosition(Vec2(pos.x + 61, pos.y + 72));

		}
		else {
			AnewBullet->setPosition(Vec2(pos.x + 90, pos.y + 27));
		}
	}
	else {
		dx = -1; dy = 0;
		if (!pa) {
			if (!IsJump && !Down) {
				if (RunUp) {
					dy = 1;
					AnewBullet->setPosition(Vec2(pos.x + 0, pos.y + 72));
				}
				else if (RunDown) {
					dy = -1;
					AnewBullet->setPosition(Vec2(pos.x + 0, pos.y + 38));
				}
				else 
					AnewBullet->setPosition(Vec2(pos.x + 0, pos.y + 53));
			}
			else if (IsJump)
				AnewBullet->setPosition(Vec2(pos.x + 0 , pos.y + 27));
			else if (Down)
				AnewBullet->setPosition(Vec2(pos.x + 0, pos.y + 72));
		}
		else {
			AnewBullet->setPosition(Vec2(pos.x + 0 , pos.y + 27));
		}
	}
	this->addChild(AnewBullet);

	bullet.push_back(std::make_pair(AnewBullet, std::make_pair(dx,dy)));
}
void GameScene::BulletRun(float dt) {
	for (int i = 0; i < bullet.size();i++) {
		int dx = bullet[i].second.first;
		int dy = bullet[i].second.second;

		float off = 1.0 * 550 * dt;
		float offx = off * dx, offy = off * dy;

		float newx = bullet[i].first->getPositionX() + offx;
		float newy = bullet[i].first->getPositionY() + offy;
		if (newx > 1024||newx<0) {
			this->removeChild(bullet[i].first);
			bullet.erase(bullet.begin()+i);
			i--;
			continue;
		}
		if (newy > 768 || newy < 0) {
			this->removeChild(bullet[i].first);
			bullet.erase(bullet.begin() + i);
			i--;
			continue;
		}
		bullet[i].first->setPositionX(newx);
		bullet[i].first->setPositionY(newy);
	}
}

void GameScene::CreateMonster() {
	for (int i = 0; i < monspw.size();i++) {
		Vec2 pos = _player->getPosition();
		pos.x -= map->getPositionX();
		float dis = abs(pos.x - monspw[i].x);
		if (dis <= 500) {
			Vector <SpriteFrame*> allFrames;
			char name[100] = {};
			sprintf_s(name, "res/monster/monster_%d_0.png", (int)monspw[i].z);
			Sprite* AnewMonster = Sprite::create(std::string(name));
			AnewMonster->setAnchorPoint(Vec2(0, 0));
			AnewMonster->setPosition(Vec2(monspw[i].x, monspw[i].y));
			map->addChild(AnewMonster);
			for (int j = 0;; j++) {
				char Anime[100] = {};
				sprintf_s(Anime, "res/monster/monster_%d_%d.png", (int)monspw[i].z, j);
				if (!IsExist(std::string(Anime)))
					break;
				Sprite* tmp = Sprite::create(Anime);
				SpriteFrame* sf = SpriteFrame::create(Anime, Rect(0, 0, tmp->getContentSize().width, tmp->getContentSize().height));
				allFrames.pushBack(sf);
			}
			Animation* ani = Animation::createWithSpriteFrames(allFrames,0.1f);
			AnewMonster->runAction(RepeatForever::create(Animate::create(ani)));
			mon.push_back(std::make_pair(AnewMonster,std::make_pair(MonsterLife[(int)monspw[i].z],(int)monspw[i].z)));
			monspw[i].w = 1;
			monspw.erase(monspw.begin() + i);
		}
	}
}
void GameScene::Hit() {
	for (int j = 0; j < mon.size(); j++) {
		for (int i = 0; i < bullet.size(); i++) {
			if (mon.size() == 0)
				break;
			if (j < 0)
				j++;
			if (j > mon.size())
				break;
			float bx = bullet[i].first->getPositionX() - map->getPositionX();
			float by = bullet[i].first->getPositionY();
			if (bx > mon[j].first->getPositionX()&&bx<mon[j].first->getPositionX()+mon[j].first->getContentSize().width
				&& by>mon[j].first->getPositionY()&&by<mon[j].first->getPositionY()+mon[j].first->getContentSize().height) {

				this->removeChild(bullet[i].first);
				bullet.erase(bullet.begin() + i);
				mon[j].second.first--;
				i--;
				if (mon[j].second.first == 0) {
					if (mon[j].second.second != 99)
						score += MonsterLife[(mon[j].second.second)] * 1000;
					else {
						score += 10000000;
						DestoryBoss = 1;
					}
					char txt[100] = {};
					sprintf_s(txt, "Score:%08d", score);
					NowScore->setString(txt);
					map->removeChild(mon[j].first);
					mon.erase(mon.begin() + j);
					j--;
				}
			}
		}
	}
}
void GameScene::HitHero() {
	for (int i = 0; i < mon.size(); i++) {
		float bx = _player->getPositionX() - map->getPositionX();
		float by = _player->getPositionY();
		if (bx > mon[i].first->getPositionX() && bx<mon[i].first->getPositionX() + mon[i].first->getContentSize().width
			&& by>mon[i].first->getPositionY() && by < mon[i].first->getPositionY() + mon[i].first->getContentSize().height) {
			IsDeath = 1;
			char txt[100] = {};
			sprintf_s(txt, "WasHit!");
			lb->setString(txt);
			Invt = IniInvincibleTime;
			_player->stopAllActions();
			inital();
			DeathAnimation();
			deat = IniDeathTime;
		}
		else {
			char txt[100] = {};
			sprintf_s(txt, "NotWasHit!");
			lb->setString(txt);
		}
	}
}

void GameScene::UpdateLife() {
	char txt[100] = {};
	sprintf(txt, "Life:%d", Life+1);
	NowLife->setString(txt);
	if (Life < 0) {
		GameFinished = 1;
	}
}
void GameScene::SetMonsterGPosition(Sprite* x,CCPoint p) {
	CCPoint titleCoop = ExchangeToMap(p);
	int titlegid = _meta->tileGIDAt(titleCoop);
	if (titlegid) {
		float nowy = x->getPositionY();
		if (p.y < nowy)
			return;
	}
	x->setPosition(p);
}
void GameScene::MonsterMove(float dt) {
	for (int i = 0; i < mon.size(); i++) {
		float gd = dt * g;
		Vec2 gg = mon[i].first->getPosition();
		gg.y -= gd;
		if (gg.y < 0) {
			map->removeChild(mon[i].first);
			mon.erase(mon.begin() + i);
			i--;
			continue;
		}
		SetMonsterGPosition(mon[i].first,gg);
		if (mon[i].second.second == 1)
			continue;
		int base = 200;
		if (mon[i].second.second == 99)
			base = 50;
		float d = dt * base;
		float newx = mon[i].first->getPositionX() + d;
		if (newx < 0) {
			map->removeChild(mon[i].first);
			mon.erase(mon.begin() + i);
			i--;
			continue;
		}
		SetMonsterGPosition(mon[i].first, Vec2(mon[i].first->getPositionX() - d, mon[i].first->getPositionY()));
	}
}

void GameScene::CreateBoss() {
	float nowx = _player->getPositionX() - map->getPositionX();
	float nowy = _player->getPositionY();
	if (fabs(nowx - 14616.00) + fabs(nowy - 269.00) <= 600) {
			BossExist = 1;
			Vector <SpriteFrame*> allFrames;
			char name[100] = {};
		sprintf_s(name, "res/monster/BOSS_0.png");
		Sprite* ABOSS = Sprite::create(std::string(name));
		ABOSS->setAnchorPoint(Vec2(0, 0));
		ABOSS->setPosition(Vec2(14616.00, 269.00));
		map->addChild(ABOSS);
		for (int j = 0;; j++) {
				char Anime[100] = {};
				sprintf_s(Anime, "res/monster/BOSS_%d.png", j);
				if (!IsExist(std::string(Anime)))
					break;
				Sprite* tmp = Sprite::create(Anime);
				SpriteFrame* sf = SpriteFrame::create(Anime, Rect(0, 0, tmp->getContentSize().width, tmp->getContentSize().height));
				allFrames.pushBack(sf);
			}
			Animation* ani = Animation::createWithSpriteFrames(allFrames, 0.1f);
			ABOSS->runAction(RepeatForever::create(Animate::create(ani)));
			mon.push_back(std::make_pair(ABOSS, std::make_pair(25, 99)));
	} 
}

void GameScene::update(float dt) {
	if(!BossExist)
		CreateBoss();
	UpdateLife();
	if(!IsJump)
		Gravity(dt);
	MonsterMove(dt);
	Hit();
	CreateMonster();
	BulletRun(dt);
	if (DestoryBoss) {
		Label* qwq = Label::createWithTTF("GaveOver!\nPress Enter to EndScene!", "fonts/Marker Felt.ttf", 48);
		qwq->setPosition(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height / 2);
		this->addChild(qwq);
	}
	if (Invt > 0)
		Invt -= dt;
	else {
		_player->setVisible(1);
		HitHero();
	}

	if (deat > 0) {
		deat -= dt;
		return;
	}
	else if (IsDeath) {
		_player->setPosition(Vec2(_player->getPositionX(), 600));
		IsDeath = 0;
		if (GameFinished) {
			EndScene* scene = EndScene::create();
			Director::getInstance()->replaceScene(scene);
		}
		Life--;
		Invt = IniInvincibleTime;
	}
	else {
		char txt[100] = {};
		sprintf_s(txt, "%lf", _player->getPositionX() - map->getPositionX());
		lb->setString(txt);
		if (fire1)
			OpenFire();
		_player->setFlippedX(KEEPLEFT ? 0 : 1);
		if (IsJump || Down) {
			_player->stopAction(run);
			if (Down)
				_player->setTexture("res/player/fall.png");
		}
		if (!IsJump && !Down && (Left || Right) && _player->numberOfRunningActions() == 0 && !pa) {
			if (RunUp)
				RunUpAnimation();
			else if (RunDown)
				RunDownAnimation();
			else
				RunAnimation();
		}
		if (!IsJump && !Down && !Left && !Right && _player->numberOfRunningActions() == 0 && !pa) {
			if (LookUp)
				_player->setTexture("res/player/lookup.png");
		}
		float d = dt * 400;
		Vec2 Pos = _player->getPosition();
		CCPoint Target;
		if (Left == 1 && !pa) {
			if (Pos.x >= 50) {
				Target = CCPoint(Pos.x - d, Pos.y);
				SetPlayerPosition(Target);
			}
		}
		else if (Right == 1 && !pa) {
			if (map->getPositionX() < -14000) {
				if (_player->getPositionX() < 500) {
					Target = CCPoint(Pos.x + d, Pos.y);
					SetPlayerPosition(Target);
				}
			}
			else if (_player->getPositionX() > 500) {
				map->setPositionX(map->getPositionX() - d);
			}
			else {
				Target = CCPoint(Pos.x + d, Pos.y);
				SetPlayerPosition(Target);
			}
		}
		if (IsJump == 1) {
			if (_player->numberOfRunningActions() == 0)
				jump();
			Vec2 Tar = Vec2(_player->getPositionX(), _player->getPositionY() + dt * 420);
			SetPlayerPosition(Tar);
			if (_player->getPositionY() >= JumpTargetY) {
				IsJump = 0;
				Down = 1;
				_player->stopAction(jumpaction);
			}
		}
	}

}
