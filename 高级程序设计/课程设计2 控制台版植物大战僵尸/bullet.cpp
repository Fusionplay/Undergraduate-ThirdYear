#include "bullet.h"

Bullet::Bullet(int tcnt, int prog): tick_cnt(tcnt)
{
	type = BULLET;
	progress = prog;
	newly_set = true;
}


int Bullet::get_progress()
{
	return progress;
}


int Bullet::get_damage()
{
	return damage;
}


bool Bullet::frame()
{
	if (newly_set)
	{
		newly_set = false;
		return false;
	}

	++tick;
	if (tick < tick_cnt)
		return false;

	tick = 0;
	if (progress < max_prog)
	{
		++progress;
		return false;
	}

	progress = 1;
	newly_set = true;
	return true; //需要移动到下一格
}


//=============================================PeashooterBullet=====================================================


PeashooterBullet::PeashooterBullet(int progp): Bullet(1, progp)
{
	bullettype = PEASHOOTERBULLET;
	tick = 0;
	damage = 50;
}


//=============================================MelonBullet=====================================================


MelonBullet::MelonBullet(int progp) : Bullet(2, progp)
{
	bullettype = MELONBULLET;
	tick = 0;
	damage = 150;
}