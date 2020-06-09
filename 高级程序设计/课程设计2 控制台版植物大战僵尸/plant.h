#pragma once
#include <vector>
#include "field_object.h"
using namespace std;


class Plant: public FieldObj
{
public:
	enum PlantType
	{
		NONE,
		SUNFLOWER,
		PEASHOOTER,
		DOUBLESHOOTER,
		NUT,
		MELON,
		PEPPER, //火爆辣椒
		ICESHROOM //寒冰菇
	};
	PlantType planttype;

	Plant(int tcnt = 30);
	virtual ~Plant() {}
	virtual bool frame();
	bool recv_damage(int damage); //扣血


protected:
	int health;   //生命值
	int tick;     //帧的响应间隔计数变量

	const int tick_cnt;
};


class SunFlower: public Plant
{
public:
	SunFlower();
	bool frame(); //sunflower的frame可以什么都不用做
};


class Peashooter: public Plant
{
public:
	Peashooter();
	//const int tick_cnt = 36;  //帧发炮响应间隔
};


class DoubleShooter: public Plant
{
public:
	DoubleShooter();
	//const int tick_cnt = 36;  //帧发炮响应间隔
};


class Nut : public Plant
{
public:
	Nut();
	bool frame(); //sunflower的frame可以什么都不用做
};


class MelonPitcher : public Plant
{
public:
	MelonPitcher();
	//const int tick_cnt = 40;  //帧发炮响应间隔
};


class Pepper: public Plant
{
public:
	Pepper();
};

 
class Iceshroom : public Plant //寒冰菇
{
public:
	Iceshroom();
};
