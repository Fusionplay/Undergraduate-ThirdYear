#include "plant.h"

Plant::Plant(int tcnt): tick_cnt(tcnt)
{
	type = PLANT;
}


bool Plant::recv_damage(int damage)
{
	health -= damage;
	return health <= 0;
}


bool Plant::frame()
{
	/*攻击型植物的frame：判断是否需要发射炮弹，若是，则发射炮弹。
	由于不能相互包含，因此发射炮弹的工作由Field类来完成

	返回值：false代表不需要发炮，true代表需要
	*/

	++tick;
	if (tick < tick_cnt)
		return false;

	tick = 0;
	return true; //需要发射炮弹。
}


//=============================================SunFlower=============================================


SunFlower::SunFlower()
{
	planttype = SUNFLOWER;
	tick = 0;
	health = 300;
}


bool SunFlower::frame()
{
	return false;
}


//=============================================PeaShooter=============================================


Peashooter::Peashooter(): Plant(36)
{
	planttype = PEASHOOTER;
	tick = 0;
	health = 400;
}


//=============================================DoubleShooter=============================================


DoubleShooter::DoubleShooter() : Plant(36)
{
	planttype = DOUBLESHOOTER;
	tick = 0;
	health = 450;
}


//=============================================Nut========================================================


Nut::Nut()
{
	planttype = NUT;
	tick = 0;
	health = 1500;
}


bool Nut::frame()
{
	return false;
}


//=============================================Melon========================================================


MelonPitcher::MelonPitcher() : Plant(40)
{
	planttype = MELON;
	tick = 0;
	health = 500;
}


//=============================================Pepper========================================================


Pepper::Pepper() : Plant(40)
{
	planttype = PEPPER;
	tick = 0;
	health = 100;
}


//=============================================Iceshroom========================================================


Iceshroom::Iceshroom(): Plant(20)
{
	planttype = ICESHROOM;
	tick = 0;
	health = 150;
}