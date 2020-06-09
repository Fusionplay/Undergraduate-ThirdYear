#pragma once
#include "field_object.h"


class Bullet: public FieldObj
{
public:
	enum BulletType
	{
		NONE,
		PEASHOOTERBULLET,
		MELONBULLET,
	};
	BulletType bullettype;

	Bullet(int tcnt, int prog = 1);
	virtual ~Bullet() {}
	int get_progress(); //获取当前前进进度
	int get_damage();

	virtual bool frame();


protected:
	int progress;  //在单元格内前进的进度. 注意：植物发射炮弹时，炮弹出现在植物的后一格中，进度为1！
	bool newly_set; //是否刚刚移动到新的一格中
	const int tick_cnt;

	//由子类初始化
	int damage;  //子弹的攻击力
	int tick;    //移动的响应间隔计数

	const int max_prog = 15; //子弹在格内最多前进的距离。设每个子弹的字符表示都是1个字符，则每个子弹最多前进slot_w = 15格
};


class PeashooterBullet: public Bullet
{
public:
	PeashooterBullet(int progp = 1);
};


class MelonBullet : public Bullet
{
public:
	MelonBullet(int progp = 1);
};
