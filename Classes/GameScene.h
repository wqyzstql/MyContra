#ifndef GAMESCENE
#define GAMESCENE

#include "cocos2d.h"

USING_NS_CC; //using namespace cocos2d

class GameScene : public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(GameScene);

	virtual bool init();

	TMXTiledMap* map;
	TMXLayer* _meta;
	TMXLayer* _meta2;
	TMXObjectGroup* obj;
	const float g = 500;
	Label* lb;
	Sprite * _player;
	Size mapSize;
	Size tileSize;
	CCPoint ExchangeToMap(CCPoint p);
	SpriteFrame* idlee;
	Vec2 pos;
	Vec2 VisuableSize;

	bool GameFinished = 0;

	void UpdateLife();
	int Life = 0;
	Label* NowLife;

	Label* NowScore;
	int score = 0;


	bool KillBoss = 0;

	const float JumpHigh = 200;
	float JumpTargetY;

	void SetPlayerPosition(CCPoint p);
	bool Left = 0, Right = 0, IsJump = 0, Down = 0 , pa=0;
	bool DownDown = 0, KeyLeft = 0, KeyRight = 0;

	bool KEEPLEFT = 0, KEEPRIGHT = 0;
	bool LookUp = 0, RunUp = 0, RunDown = 0;

	void inital();

	const float IniInvincibleTime = 2.0f;
	float Invt;

	const float IniDeathTime = 2.0f;
	float deat = 0;
	bool IsDeath = 0;

	void RunUpAnimation();
	void RunDownAnimation();
	void DeathAnimation();

	float mapWidth;
	float mapX;
	void idle();
	void Gravity(float dt);
	void SetGPlayerPosition(CCPoint p);
	void jump();
	void RunAnimation();
	Action* jumpaction , *run;
	int count = 0;
	TMXObjectGroup* monster;
	void CreateMonster();
	std::vector<std::pair<Sprite*, std::pair<int, int > > > mon;
	std::vector<Vec4> monspw;
	std::vector<std::pair<Sprite*, std::pair<int, int> > > bullet;
	bool outscreen();
	bool fire1;

	bool BossExist = 0;
	void CreateBoss();
	bool DestoryBoss = 0;

	int MonsterLife[5] = { 1, 2, 10, 3, 6 };

	void Hit();

	void MonsterMove(float dt);
	void SetMonsterGPosition(Sprite* x, CCPoint p);

	void BulletRun(float dt);
	void OpenFire();

	void HitHero();

	void ListernKeyboardEvent();
	void OnKeyPressed(EventKeyboard::KeyCode code, Event* evt);
	void OnKeyReleased(EventKeyboard::KeyCode code, Event* evt);
	void update(float dt);
};


#endif // !GAMESCENE
