#include "zombie.h"

Zombie::Zombie(int tcnt, int dcnt): tick_cnt(tcnt), damage_tick_cnt(dcnt), frozen_tick_cnt(150)
{
	type = ZOMBIE;
	progress = 0;
	tick = 0;
	damaging = false;
	damage_tick = 0;
	frozen = false;
}


int Zombie::get_progress()
{
	return progress;
}


void Zombie::freeze()
{
	tick = 0;
	frozen = true;
}


void Zombie::defreeze()
{
	frozen = false;
}


bool Zombie::recv_damage(int damage)
{
	health -= damage;
	return health <= 0;
}


int Zombie::get_score()
{
	return score;
}


bool Zombie::frame(const vector<FieldObj*>& next, bool & plant_killed, bool at_end)
{
	/*每一帧要做的事情
	1. 实现僵尸的移动。若没有移动到底线，或前方没有植物，则向前移动一格小格（不是大格）,必要时前进一个大格
	2. 咬植物。

	返回值：true代表可以向前移动一格，false代表不能。
	*/
	plant_killed = false;

	if (frozen)
	{
		++tick;
		if (tick == frozen_tick_cnt)
		{
			tick = 0;
			defreeze();
		}
		return false;
	}


	else if (!damaging) //当前没有正在攻击植物
	{
		++tick;
		if (tick < tick_cnt)
			return false;

		tick = 0;
		if (progress < max_prog)
		{
			++progress;
			return false;
		}


		if (at_end)
			return true; //此时僵尸突破底线，游戏结束
						 //此时不需要移动，只需要停着吃植物；或者移动到下一个
						 //首先判断前方是否有植物，若有则吃植物，扣血；否则置progress为0，移动到下一格
		bool has_plant = false;
		for (auto it : next)
		{
			if (it->type == PLANT)
			{
				//发现有植物
				has_plant = true;
				break;
			}
		}

		//没有植物：直接移动到下一格
		if (!has_plant)
		{
			progress = 0;
			return true;
		}

		//有植物：切换到攻击模式
		damaging = true;
		return false;
	}


	//攻击模式
	else
	{
		++damage_tick;
		if (damage_tick < damage_tick_cnt)
			return false;

		damage_tick = 0;
		//直接在这里扣植物的血
		for (auto it : next)
		{
			if (it->type == PLANT)
			{
				//发现有植物:扣血
				plant_killed = static_cast<Plant *>(it)->recv_damage(damage);
				return false;
			}
		}

		//前方已无植物，可以前进
		damaging = false;
		return false;
	}
}


//=============================================Normal Zombie=============================================


NormalZombie::NormalZombie() : Zombie(10, 2)
{
	zomtype = NORMAL;
	health = 500;
	damage = 20;
	score = 25; //普通僵尸：25分
}


//=============================================Conehead Zombie=============================================


ConeheadZombie::ConeheadZombie() : Zombie(9, 2)
{
	zomtype = CONEHEAD;
	health = 600;
	damage = 20;
	score = 40; //路障僵尸：40分
}


//=============================================Bucket Zombie=============================================


BucketZombie::BucketZombie() : Zombie(10, 2)
{
	zomtype = BUCKET;
	health = 850;
	damage = 20;
	score = 50; //铁桶僵尸：50分
}


//=============================================Paper Zombie=============================================


PaperZombie::PaperZombie() : Zombie(normal_tick_cnt, 2) //读报僵尸正常情况下，tickcnt为10; damagecnt一直 =2
{
	zomtype = PAPER;
	health = body_health + paper_health;
	damage = normal_damage; //一开始是普通的伤害值
	score = 55; //读报僵尸：55分
	tick_cnt = normal_tick_cnt;
}


bool PaperZombie::recv_damage(int damage)
{
	health -= damage;
	if (health <= body_health)
	{
		this->damage = rage_damage;
		this->tick_cnt = rage_tick_cnt;
	}
	return health <= 0;
}


//=============================================Football Zombie=============================================


FootballZombie::FootballZombie() : Zombie(6, 1) //橄榄球僵尸攻击速度很快
{
	zomtype = FOOTBALL;
	health = 1100;
	damage = 21; 
	score = 65; //橄榄球僵尸：65分
}