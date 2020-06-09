#pragma once
#include <vector>
#include "field_object.h"
#include "plant.h"
using namespace std;


class Zombie: public FieldObj
{
public:
	enum ZomType
	{
		NONE,
		NORMAL,
		CONEHEAD, //路障僵尸
		BUCKET,   //铁桶僵尸
		PAPER,    //读报僵尸
		FOOTBALL, //橄榄球僵尸
	};

	ZomType zomtype;
	Zombie(int tcnt = 10, int dcnt = 2);
	virtual ~Zombie() {}

	int get_progress(); //获取当前前进进度
	void freeze();      //让寒冰菇冰冻僵尸
	virtual bool recv_damage(int damage); //扣血:读报僵尸的扣血机制与其它不同，因此设该函数为虚函数，方便多态
	int get_score();
	virtual bool frame(const vector<FieldObj *> &next, bool &plant_killed, bool at_end);
	
	
protected:
	const int frozen_tick_cnt; //被冰冻15个帧
	const int max_prog = 14; //僵尸在格内最多前进的距离 = slot_w - 1。设每个僵尸的字符表示都是7个字母，则每个僵尸最多前进8格
	bool frozen;   //是否已被冰冻
	int progress;  //在一个格内已经前进的距离
	int tick;      //移动响应
	bool damaging; //是否正在攻击植物
	int damage_tick; //攻击植物的响应间隔
	//bool frozen;   //是否已被冰冻

	//由子类来初始化
	int health; //生命值
	int damage; //攻击力
	int score;  //杀死这个僵尸后，得到的分数

	//tick_out的值, 同样由子类来初始化
	int tick_cnt;
	const int damage_tick_cnt;
	
	void defreeze();
};


class NormalZombie: public Zombie //普通僵尸
{
public:
	NormalZombie();
	//const int tick_cnt = 10;  //僵尸移动的响应间隔，也即速度
	//const int damage_tick_cnt = 2; //间隔多久才攻击一次植物
};


class ConeheadZombie: public Zombie
{
public:
	ConeheadZombie();
	//const int tick_cnt = 9;  //僵尸移动的响应间隔，也即速度
	//const int damage_tick_cnt = 2; //
};


class BucketZombie: public Zombie
{
public:
	BucketZombie();
	//const int tick_cnt = 10;  //僵尸移动的响应间隔，也即速度
	//const int damage_tick_cnt = 2; //
};


class PaperZombie: public Zombie //读报僵尸
{
public:
	PaperZombie();
	bool recv_damage(int damage);

private:
	const int normal_tick_cnt = 10;
	const int rage_tick_cnt = 2;

	const int normal_damage = 15; 
	const int rage_damage = 25;

	const int paper_health = 150; //纸的血量是150
	const int body_health = 600;  //本体的血量


	//int tick_cnt;  //僵尸移动的响应间隔，也即速度。读报僵尸的移动速度会发生变化，因此不能设为常量
	//const int damage_tick_cnt = 2; //
};


class FootballZombie: public Zombie
{
public:
	FootballZombie();
};

