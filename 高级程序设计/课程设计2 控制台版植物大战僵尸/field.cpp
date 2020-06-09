#include "field.h"

bool cmp(FieldObj* a, FieldObj *b) //cmp为true时，a b不交换，为false时，交换
{
	FieldObj::ObjType t = a->type;
	if (t == b->type)
	{
		if (a->type == FieldObj::ZOMBIE) //都是僵尸
			return static_cast<Zombie *>(a)->get_progress() > static_cast<Zombie *>(b)->get_progress();
		else //都是子弹
			return static_cast<Bullet *>(a)->get_progress() > static_cast<Bullet *>(b)->get_progress();
	}

	else if (t == FieldObj::ZOMBIE) //a是僵尸，b是子弹
		return false;  //应交换，子弹应来到僵尸前面
	else
		return true;
}


Field::Field() //zom_tick_cnt(1)
{
	sun = 200; //一开始有200块
	score = 0;
	price[Plant::SUNFLOWER] = 50;  //向日葵价格
	price[Plant::PEASHOOTER] = 100;  //豌豆射手价格
	price[Plant::DOUBLESHOOTER] = 190;  //双枪射手价格
	price[Plant::NUT] = 250;  //土豆价格
	price[Plant::MELON] = 400;  //西瓜投手价格
	price[Plant::PEPPER] = 600;  //火爆辣椒价格
	price[Plant::ICESHROOM] = 550;  //寒冰菇价格

	sun_add = 25;
	sun_plus = 25;             //每多一个向日葵，增长的阳光数就多25
	sun_tick = 0;
	zom_tick = 0;
	init_store_vars();
	init_shovel();
	srand((unsigned int)time(0));

	//显示用变量的初始化
	coord = { 0, 0 };
	bytes = 0;
	h1 = CreateConsoleScreenBuffer(
		GENERIC_WRITE | GENERIC_READ,//定义进程可以往缓冲区写数据
		FILE_SHARE_WRITE,//定义缓冲区可共享写权限
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	current = h1;
	cur_point = {100, 19};
	SetConsoleCursorPosition(current, cur_point);//设置光标的初始位置

	//显示两个缓冲区的光标
	cci.bVisible = 0; //一开始设置光标不可见
	cci.dwSize = 1;
	SetConsoleCursorInfo(h1, &cci);
	SetConsoleActiveScreenBuffer(current);  //设置当前控制台

	//初始化战场
	setup_field();
	for (int i = 0; i < dis_h + gap + store_h; i++)
		dis[i][dis_w] = '\0';
}


void Field::setup_field() //初始化战场
{
	//打印战场
	//第一行的=
	for (int i = 0; i <= (col_num + 1) * (slot_w + 1); i++)
		dis[0][i] = '=';

	int row = 1;
	for (int i = 0; i < row_num; i++)  //3个行
	{
		for (int j = 0; j < slot_h; j++)  //每行：slot_h行个空位
		{
			for (int k = 0; k < (col_num + 1) * (slot_w + 1); k++) //先铺满空格，再填充分隔符
				dis[row][k] = ' ';
			for (int k = 0; k < (col_num + 1) * (slot_w + 1); k += (slot_w + 1))
				dis[row][k] = '#';
			++row;
		}

		for (int j = 0; j <= col_num * (slot_w + 1); j++)
			dis[row][j] = '=';
		for (int j = col_num * (slot_w + 1) + 1; j <= (col_num + 1) * (slot_w + 1); j++)
			dis[row][j] = ' ';
		++row;
	}

	--row;
	for (int i = col_num * (slot_w + 1); i <= (col_num + 1) * (slot_w + 1); i++)
		dis[row][i] = '=';
	

	//打印战场和商店的间隔，以及初始化商店界面中的空格
	++row;
	for (int i = row; i < row + store_h + gap; i++)
	{
		for (int j = 0; j <= (col_num + 1) * (slot_w + 1); j++)
			dis[i][j] = ' ';
	}


	//打印商店
	row += gap;
	//第1行的=
	for (int i = 0; i <= (col_num + 1) * (slot_w + 1); i++)
		dis[row][i] = '=';

	
	//商店第2行：STORE字样，SUN，SCORE字样
	++row;
	strncpy(dis[row] + 1, "[STORE]", 7);
	strncpy(dis[row] + 65, "SUN:", 4);

	char t[10];
	_itoa(sun, t, 10);
	strncpy(dis[row] + 69, t, strlen(t));

	strncpy(dis[row] + 80, "[SCORE]:", 8);
	_itoa(score, t, 10);
	strncpy(dis[row] + 88, t, strlen(t));


	//商店第3行：分隔符
	++row;
	for (int i = 0; i <= (col_num + 1) * (slot_w + 1); i++)
		dis[row][i] = '-';


	//商店第4行：植物价格
	++row;
	strncpy(dis[row] + 1, "SUNFLOWER:50", 12);
	strncpy(dis[row] + 1 + gap_of_plant_name, "PEASHOOTER:100", 14);
	strncpy(dis[row] + 1 + gap_of_plant_name * 2, "DOUBLESHOOTER:190", 17);

	//商店第5行：第二排植物
	++row;
	strncpy(dis[row] + 1, "NUT:250", 7);
	strncpy(dis[row] + 1 + gap_of_plant_name, "MELONPITCHER:400", 16);
	strncpy(dis[row] + 1 + gap_of_plant_name * 2, "PEPPER:600", 10);

	//商店第6行：第三排植物
	++row;
	strncpy(dis[row] + 1, "ICESHROOM:550", 13);

	//商店第7行：=号
	++row;
	for (int i = 0; i <= (col_num + 1) * (slot_w + 1); i++)
		dis[row][i] = '=';
}


void Field::print_all() //根据battle_field数组的内容，将元素打印到战场上并显示出来
{
	setup_field();

	/* 下面开始将battle_field数组的内容打印到“显存”中。
	每个单元格中的内容有以下种情况：
	1. nothing: 0: 忽略该情况
	2. only bullet: 1
	3. only one plant: 1
	4. multiple bullets + one plant: 1 + x
	5. multiple zombies: x
	6. multiple bullets + multiple zombies: x + x
	*/

	int size;
	int row = 1;
	int plen;
	COORD loc;
	Plant *plant;
	Zombie *zom;
	Bullet *bul;
	for (int i = 0; i < row_num; ++i)
	{
		for (int j = 0; j <= col_num; ++j)
		{
			size = battle_field[i][j].size();
			if (size == 0)
				continue;

			row = 1;
			vector<FieldObj *>::iterator it = battle_field[i][j].begin();
			while (it != battle_field[i][j].end())
			{
				if ((*it)->type == FieldObj::BULLET)
				{
					loc.Y = i * (slot_h + 1) + 3; //格子中的第3行
					loc.X = j * (slot_w + 1) + static_cast<Bullet*>(*it)->get_progress(); //格子中的相应列

					bul = static_cast<Bullet *>(*it);
					if (bul->bullettype == Bullet::PEASHOOTERBULLET) //豌豆射手的子弹
						strncpy(dis[loc.Y] + loc.X, "O", 1);
					else  //豌豆射手的子弹
						strncpy(dis[loc.Y] + loc.X, "%", 1);
				}

				else if ((*it)->type == FieldObj::ZOMBIE)
				{
					zom = static_cast<Zombie *>(*it);
					loc.Y = i * (slot_h + 1) + row; //格子中的第row行
					loc.X = j * (slot_w + 1) + slot_w - zom->get_progress(); //格子中的相应列
					plen = min(zom_name_len, zom->get_progress() + 1);
					switch (zom->zomtype)
					{
					case Zombie::NORMAL: strncpy(dis[loc.Y] + loc.X, "Nzombie", plen); break;
					case Zombie::CONEHEAD: strncpy(dis[loc.Y] + loc.X, "Conezom", plen); break;
					case Zombie::BUCKET: strncpy(dis[loc.Y] + loc.X, "Buckzom", plen); break;
					case Zombie::PAPER: strncpy(dis[loc.Y] + loc.X, "Papezom", plen); break;
					case Zombie::FOOTBALL: strncpy(dis[loc.Y] + loc.X, "Footzom", plen); break;
					default: break;
					}

					++row;
					if (row > slot_h)
						row = 1;
				}

				else //plant
				{
					loc.Y = i * (slot_h + 1) + 4; //格子中的第4行
					loc.X = j * (slot_w + 1) + 2; //格子中的第3列

					plant = static_cast<Plant *>(*it);
					switch (plant->planttype)
					{
					case Plant::PEASHOOTER: strncpy(dis[loc.Y] + loc.X, "PeaShooter", 10); break;
					case Plant::SUNFLOWER: strncpy(dis[loc.Y] + loc.X, "SunFlower", 9); break;
					case Plant::DOUBLESHOOTER: strncpy(dis[loc.Y] + loc.X, "DoubleShooter", 13); break;
					case Plant::NUT: strncpy(dis[loc.Y] + loc.X, "Nut", 3); break;
					case Plant::MELON: strncpy(dis[loc.Y] + loc.X, "MelonPitcher", 12); break;
					case Plant::PEPPER: strncpy(dis[loc.Y] + loc.X, "PEPPER!", 7); break;
					case Plant::ICESHROOM: strncpy(dis[loc.Y] + loc.X, "ICESHROOM!", 10); break;
					default:break;
					}
				}

				++it;
			}
		}
	}


	//显示出来
	for (int i = 0; i < dis_h + gap + store_h; i++)
	{
		coord.Y = i;
		WriteConsoleOutputCharacterA(current, dis[i], dis_w, coord, &bytes);
	}
	SetConsoleCursorPosition(current, cur_point);//设置光标位置
}


inline void Field::init_store_vars()
{
	plant_selected = false;
	entered_store = false;
	choosed_plant = Plant::NONE;
	plant_loc = { -1, -1 };
}


inline void Field::init_shovel()
{
	entered_shovel = false;
	shovel_loc = { -1, -1 };
}


bool Field::OK_to_plant(vector<FieldObj*>& place)
{
	for (int i = 0; i < place.size(); i++)
	{
		if (place[i]->type != FieldObj::BULLET)
			return false;
	}
	return true;
}


bool Field::move_to_1st_available_slot()
{
	/* 
	从上到下，从左到右地遍历全部21个格子，找到首个没有任何植物和僵尸存在的位置。
	将光标位置设为那个格子的中心点处。
	*/

	for (int j = 0; j < col_num; ++j)
	{
		for (int i = 0; i < row_num; ++i)
		{
			if (OK_to_plant(battle_field[i][j]))
			{
				cur_point.Y = i * (slot_h + 1) + 3; //格子中的第3行
				cur_point.X = j * (slot_w + 1) + 1; //格子中的第1列
				plant_loc.Y = i;
				plant_loc.X = j;
				return true;
			}
		}
	}
	return false;
}


bool Field::move_to_next_plant(bool first)
{
	if (first)
	{
		for (int j = 0; j < col_num; ++j)
		{
			for (int i = 0; i < row_num; ++i)
			{
				for (int k = 0; k < battle_field[i][j].size(); ++k)
				{
					if (battle_field[i][j][k]->type == FieldObj::PLANT)
					{
						cur_point.Y = i * (slot_h + 1) + 3; //格子中的第3行
						cur_point.X = j * (slot_w + 1) + 1; //格子中的第1列
						shovel_loc.Y = i;
						shovel_loc.X = j;
						return true;
					}
				}
			}
		}
	}


	else
	{
		bool never = true;
		for (int j = 0; j < col_num; ++j) //col_num次
		{
			for (int i = shovel_loc.Y; i < row_num; ++i)
			{
				if (!never)
				{
					for (int k = 0; k < battle_field[i][shovel_loc.X].size(); ++k)
					{
						if (battle_field[i][shovel_loc.X][k]->type == FieldObj::PLANT)
						{
							cur_point.Y = i * (slot_h + 1) + 3; //格子中的第3行
							cur_point.X = shovel_loc.X * (slot_w + 1) + 1; //格子中的第1列
																		   //shovel_loc.Y = i;
																		   //shovel_loc.X = j;
							return true;
						}
					}
				}

				shovel_loc.Y = (shovel_loc.Y + 1) % row_num;
				never = false;
			}

			shovel_loc.X = (shovel_loc.X + 1) % col_num;
		}
	}
	
	return false;
}


inline void Field::exit_store()
{
	init_store_vars();
	cci.bVisible = 0; //退出商店，光标回到隐藏状态
	SetConsoleCursorInfo(current, &cci);
}


inline void Field::exit_shovel()
{
	init_shovel();
	cci.bVisible = 0; 
	SetConsoleCursorInfo(current, &cci);
}


void Field::handle_key_press(char ch)
{
	if (ch == 0)
		return;

	if (!entered_store && !entered_shovel) //未进入商店且未进入铲子模式
	{
		switch (ch)
		{
		case ' ': //进入商店
			entered_store = true;
			cur_point.Y = dis_h + gap + 3; //第一行商品所在行
			cur_point.X = 0;

			//设置光标可见
			cci.bVisible = 1;
			SetConsoleCursorInfo(current, &cci);
			break;

		case 'f': //铲子
			entered_shovel = true;
			if (move_to_next_plant())
			{
				cci.bVisible = 1;
				SetConsoleCursorInfo(current, &cci);
			}
			//设置光标可见
			else
				init_shovel();
			break;

		default: break;
		}
	}

	else if (entered_store)  //已进入商店
	{
		if (ch == 'x') //退出商店
			exit_store();

		else if (!plant_selected) //未选中植物
		{
			switch (ch)
			{
			case 'd': //光标向右移动 
				if (cur_point.X < gap_of_plant_name * (num_of_plants_per_row - 1))
					cur_point.X += gap_of_plant_name;
				else
					cur_point.X = 0;
				break;

			case 'a': //光标向左移动 
				if (cur_point.X > 0)
					cur_point.X -= gap_of_plant_name;
				else
					cur_point.X = gap_of_plant_name * (num_of_plants_per_row - 1);
				break;

			case 's': //光标向下移动 
				if (cur_point.Y < dis_h + gap + 3 + (store_plantline_h - 1))
					cur_point.Y += 1;
				else
					cur_point.Y = dis_h + gap + 3;
				break;

			case 'w': //光标向上移动 
				if (cur_point.Y > dis_h + gap + 3)
					cur_point.Y -= 1;
				else
					cur_point.Y = dis_h + gap + 3 + (store_plantline_h - 1);
				break;

			case 13: //Enter键
				//要考虑钱不够，无法选中的问题。这个问题应在这里被解决
				if (cur_point.Y == dis_h + gap + 3) //第一排
				{
					if (cur_point.X == 0) //第一个植物
					{
						if (sun < price[Plant::SUNFLOWER]) //钱不够花
							break;
						else
							choosed_plant = Plant::SUNFLOWER;
					}
						
					else if (cur_point.X == gap_of_plant_name)
					{
						if (sun < price[Plant::PEASHOOTER]) //钱不够花
							break;
						else
							choosed_plant = Plant::PEASHOOTER;
					}

					else if (cur_point.X == gap_of_plant_name * 2)
					{
						if (sun < price[Plant::DOUBLESHOOTER]) //钱不够花
							break;
						else
							choosed_plant = Plant::DOUBLESHOOTER;
					}
				}

				else if (cur_point.Y == dis_h + gap + 4) //第二排：更多植物
				{
					if (cur_point.X == 0) //第一个植物:NUT
					{
						if (sun < price[Plant::NUT]) //钱不够花
							break;
						else
							choosed_plant = Plant::NUT;
					}

					else if (cur_point.X == gap_of_plant_name) //第2排第2个植物：西瓜投手
					{
						if (sun < price[Plant::MELON]) //钱不够花
							break;
						else
							choosed_plant = Plant::MELON;
					}

					else if (cur_point.X == gap_of_plant_name * 2) //第2排第3个植物：火爆辣椒
					{
						if (sun < price[Plant::PEPPER]) //钱不够花
							break;
						else
							choosed_plant = Plant::PEPPER;
					}
				}

				else //第3排植物：寒冰菇
				{
					if (cur_point.X == 0) //第一个植物:寒冰菇
					{
						if (sun < price[Plant::ICESHROOM]) //钱不够花
							break;
						else
							choosed_plant = Plant::ICESHROOM;
					}
				}

				plant_selected = true;
				if (!move_to_1st_available_slot()) //将光标移到第一个可用的位置处，准备放置植物
					exit_store();
				break;

			default:break;
			}
		}


		else //已选中植物，等待放置
		{
			bool tempok = true;
			if (!OK_to_plant(battle_field[plant_loc.Y][plant_loc.X])) //若当前位置不可种植
				tempok = move_to_1st_available_slot();

			//首先要移动到首个可用位置, 因为在等待放置的过程中，战场会随时变化
			if (!tempok) //将光标移到第一个可用的位置处，准备放置植物. 若没有可用的位置: 退出商店
				exit_store();

			else
			{
				switch (ch)
				{
				case 'd': //向右移动格子
					for (int i = 0; i < col_num; ++i)
					{
						if (cur_point.X == (col_num - 1) * (slot_w + 1) + 1)
							cur_point.X = 1;
						else
							cur_point.X += slot_w + 1;

						plant_loc.X = (plant_loc.X + 1) % col_num;
						if (OK_to_plant(battle_field[plant_loc.Y][plant_loc.X]))
							break;
					}
					break;


				case 'a': //向左移动
					for (int i = 0; i < col_num; ++i)
					{
						if (cur_point.X == 1)
							cur_point.X = (col_num - 1) * (slot_w + 1) + 1;
						else
							cur_point.X -= slot_w + 1;

						if (plant_loc.X == 0)
							plant_loc.X = col_num - 1;
						else
							plant_loc.X -= 1;

						if (OK_to_plant(battle_field[plant_loc.Y][plant_loc.X]))
							break;
					}
					break;


				case 'w': //向上移动
					for (int i = 0; i < row_num; ++i)
					{
						if (cur_point.Y == 3)
							cur_point.Y = (row_num - 1) * (slot_h + 1) + 3;
						else
							cur_point.Y -= slot_h + 1;

						if (plant_loc.Y == 0)
							plant_loc.Y = row_num - 1;
						else
							plant_loc.Y -= 1;

						if (OK_to_plant(battle_field[plant_loc.Y][plant_loc.X]))
							break;
					}
					break;

				case 's': //向下移动
					for (int i = 0; i < row_num; ++i)
					{
						if (cur_point.Y == (row_num - 1) * (slot_h + 1) + 3)
							cur_point.Y = 3;
						else
							cur_point.Y += slot_h + 1;

						plant_loc.Y = (plant_loc.Y + 1) % row_num;
						if (OK_to_plant(battle_field[plant_loc.Y][plant_loc.X]))
							break;
					}
					break;


				case 13: //Enter键:添加相应的植物到战场上
					//根据类型来添加植物到战场上，然后退出商店.
					//还需要考虑资金是否充裕，能否购买的问题：已在选择植物的阶段解决该问题！
					//Plant *pl;
					switch (choosed_plant)
					{
					case Plant::SUNFLOWER:
						//向日葵是特殊植物。对向日葵的处理：每新添加一个向日葵，程序每个tick增加的阳光数增加一些
						battle_field[plant_loc.Y][plant_loc.X].push_back(new SunFlower);
						sun -= price[Plant::SUNFLOWER];
						sun_add += sun_plus;
						break;

					case Plant::PEASHOOTER:
						battle_field[plant_loc.Y][plant_loc.X].push_back(new Peashooter);
						sun -= price[Plant::PEASHOOTER];
						break;

					case Plant::DOUBLESHOOTER:
						battle_field[plant_loc.Y][plant_loc.X].push_back(new DoubleShooter);
						sun -= price[Plant::DOUBLESHOOTER];
						break;

					case Plant::NUT:
						battle_field[plant_loc.Y][plant_loc.X].push_back(new Nut);
						sun -= price[Plant::NUT];
						break;

					case Plant::MELON:
						battle_field[plant_loc.Y][plant_loc.X].push_back(new MelonPitcher);
						sun -= price[Plant::MELON];
						break;

					case Plant::PEPPER:
						battle_field[plant_loc.Y][plant_loc.X].push_back(new Pepper);
						sun -= price[Plant::PEPPER];
						break;

					case Plant::ICESHROOM:
						battle_field[plant_loc.Y][plant_loc.X].push_back(new Iceshroom);
						sun -= price[Plant::ICESHROOM];
						break;

					default:
						break;
					}

					exit_store(); //购买完成，退出商店
					break;

				default: break;
				}
			}
		}
	}

	else if (entered_shovel)
	{
		if (ch == 'x') //退出铲子模式
			exit_shovel();

		else
		{
			switch (ch)
			{
			case 'f':
				if (!move_to_next_plant(false))
					exit_shovel();
				break;


			case 13: //Enter键
					 //shovel_loc.Y = i;
					 //shovel_loc.X = j;
				for (int k = 0; k < battle_field[shovel_loc.Y][shovel_loc.X].size(); ++k)
				{
					if (battle_field[shovel_loc.Y][shovel_loc.X][k]->type == FieldObj::PLANT)
					{
						Plant *plant = static_cast<Plant*>(battle_field[shovel_loc.Y][shovel_loc.X][k]);
						delete plant;
						battle_field[shovel_loc.Y][shovel_loc.X].erase(battle_field[shovel_loc.Y][shovel_loc.X].begin() + k);
						break;
					}
				}

				exit_shovel();
				break;

			default: break;
			}
		}
	}
}


inline void Field::handle_sun()
{
	++sun_tick;
	if (sun_tick == sun_tick_cnt)
	{
		sun_tick = 0;
		sun += sun_add;
	}
}


void Field::generate_new_zombie()
{
	++zom_tick;
	if (zom_tick < zom_tick_cnt)
		return;

	/*开始产生新僵尸.
	1. 随机产生僵尸的位置:战场（3 * 7）上的位置
	2. 随机产生僵尸的种类. 不同种类的产生概率不一样！
	*/
	zom_tick = 0;
	int rtmp = rand();
	int x = rtmp % row_num;
	int y = col_num;
	
	rtmp = rand() % 100 + 1;

	//11.11 23:05:目前只有普通和智障两种僵尸，1-80是普通，81-100是智障
	//11.14 21:39:新增铁桶僵尸。
	if (rtmp <= 50)
		battle_field[x][y].push_back(new NormalZombie);
	else if (rtmp <= 60)
		battle_field[x][y].push_back(new ConeheadZombie);
	else if (rtmp <= 70)
		battle_field[x][y].push_back(new BucketZombie);
	else if (rtmp <= 90)
		battle_field[x][y].push_back(new PaperZombie); 
	else //if (rtmp <= 70)
		battle_field[x][y].push_back(new FootballZombie);
}


int Field::slot_type_f(vector<FieldObj*>& place)
{
	/*
	每个单元格中的内容有以下种情况：
	1. nothing: 0: 忽略该情况
	2. only bullet: 1
	3. only one plant: 1
	4. multiple bullets + one plant: 1 + x
	5. multiple zombies: x
	6. multiple bullets + multiple zombies: x + x

	情况1忽略，情况2 3 4（也即没有僵尸的情况）可以一并考虑。
	有僵尸的情况中：仅僵尸，僵尸与子弹共存，这两种情况要分开

	返回值：
	1. 无僵尸
	2. 仅僵尸
	3. 僵尸，子弹共存
	*/
	int zom = 0;
	int bullet = 0;
	auto end = place.end();

	for (auto it = place.begin(); it != end; ++it)
	{
		if ((*it)->type == FieldObj::ZOMBIE)
			++zom;
		else
			++bullet;
	}

	if (zom == 0) //无僵尸
		return 1;
	if (bullet == 0) //仅僵尸
		return 2;
	return 3;
}


bool Field::frame(char ch)
{
	/*
	战场每帧要做的事：
	1. 产生新僵尸
	2. 让炮弹移动
	3. 炮弹与僵尸的交互：炮弹击中僵尸，然后消失，僵尸扣血，血扣完了要死掉
	4. 僵尸与植物的交互：吃植物：植物扣血，血扣完了死掉；
	5. 僵尸的前进：若前方无植物，则前进；否则吃植物至其死掉为止，下一tick继续前进
	
	6. 若僵尸前进到底线：也即僵尸当前位置为列0，且仍未死，仍可以前进，则判断游戏结束。
	7. 特殊植物：向日葵：自动产生阳光
	8. 自动产生阳光。
	
	9. 商店的交互！
	*/


	//1. 处理键盘输入：商店的交互。
	handle_key_press(ch);

	//2. 自动产生阳光：系统自有光 + 向日葵的光
	handle_sun();
	
	//3. 生成新的僵尸
	generate_new_zombie();

	//4. 调用战场上各个已有元素的frame函数，实现交互
	/*
	僵尸的frame需要的参数：当前的格子的坐标，前方格子的内容（判断前方有无植物，需要扣植物的血，并阻止僵尸前进）
	植物的frame:不需要参数。植物的frame只需要判断是否需要发射炮弹即可（若该植物能够发射炮弹）,需要则产生炮弹
	子弹的frame需要的参数：子弹当前的坐标以及前方格子的内容：若前方有僵尸则扣僵尸的血并自己消失；若没有则进行移动

	每个单元格中的内容有以下种情况：
	1. nothing: 0: 忽略该情况
	2. only bullet: 1
	3. only one plant: 1
	4. multiple bullets + one plant: 1 + x
	5. multiple zombies: x
	6. multiple bullets + multiple zombies: x + x

	情况1忽略，情况2 3 4（也即没有僵尸的情况）可以一并考虑。
	有僵尸的情况中：仅僵尸，僵尸与子弹共存，这两种情况要分开

	*/
	Plant* plant;
	Zombie* zom;
	Bullet* bul = nullptr;
	Bullet* bul2 = nullptr;
	vector<FieldObj *>::iterator first_zom;
	vector<FieldObj *>::iterator zom_to_kill;
	bool killed;
	bool tomove_zom = false;
	bool tomove_bul = false;
	int size;
	int endcnt;
	int damage;
	int bul_pro;
	int first_zom_pro;
	for (int i = 0; i < row_num; ++i)
	{
		for (int j = 0; j <= col_num; ++j)
		{
			size = battle_field[i][j].size();
			if (size == 0)
				continue;

			auto it = battle_field[i][j].begin();
			switch (slot_type_f(battle_field[i][j]))
			{
			case 1: //无僵尸: 仅植物或子弹
				while (it != battle_field[i][j].end())
				{
					if ((*it)->type == FieldObj::BULLET) //是子弹
					{
						/*
						如果子弹移动到下一格之后，立刻碰到了僵尸怎么办？如何监测是否立刻碰到僵尸：只能手动监测
						*/
						tomove_bul = static_cast<Bullet *>(*it)->frame();
						if (tomove_bul) //子弹要移动一格
						{
							//将子弹从当前单元格删去，然后判断是否需要立刻打击下一单元格内的僵尸，若需要则立刻打击
							//并让子弹消失，否则添加子弹到下一单元格内
							bul = static_cast<Bullet *>(*it);
							it = battle_field[i][j].erase(it); //从当前单元格删去
							//还要考虑子弹是否已到达最后一格的问题
							if (j == col_num) //子弹需要消失
								delete bul;
							else //下一格没有僵尸，则添加到下一格
								battle_field[i][j + 1].push_back(bul);

							continue;
						}
					}


					else //是植物
					{
						/*植物要做的事：判断是否需要发射炮弹，需要则产生炮弹：若后面需要攻打僵尸，则炮弹不显示，直接对僵尸进行攻击
						火爆辣椒：杀死该行全部僵尸
						寒冰菇：冰冻全部僵尸
						*/
						plant = static_cast<Plant *>(*it);
						if (plant->frame()) //需要发炮
						{
							if (plant->planttype != Plant::PEPPER && plant->planttype != Plant::ICESHROOM)
							{
								//产生炮弹
								if (plant->planttype == Plant::PEASHOOTER)
									battle_field[i][j + 1].push_back(new PeashooterBullet);

								else if (plant->planttype == Plant::DOUBLESHOOTER)
								{
									battle_field[i][j + 1].push_back(new PeashooterBullet);
									battle_field[i][j + 1].push_back(new PeashooterBullet(2));
								}

								else if (plant->planttype == Plant::MELON)
									battle_field[i][j + 1].push_back(new MelonBullet);
							}

							else if (plant->planttype == Plant::PEPPER) //是火爆辣椒
							{
								//杀死该行全部僵尸
								for (int k = 0; k <= col_num; ++k)
								{
									zom_to_kill = battle_field[i][k].begin();
									while (zom_to_kill != battle_field[i][k].end())
									{
										if ((*zom_to_kill)->type == FieldObj::ZOMBIE)
										{
											zom = static_cast<Zombie *>(*zom_to_kill);
											score += zom->get_score();
											zom_to_kill = battle_field[i][k].erase(zom_to_kill);
											delete zom;
											continue;
										}
										++zom_to_kill;
									}
								}

								delete plant;
								it = battle_field[i][j].erase(it);
								continue;
							}

							else if (plant->planttype == Plant::ICESHROOM) //寒冰菇：冰冻战场上全部僵尸
							{
								for (int k = 0; k < row_num; ++k)
								{
									for (int w = 0; w <= col_num; ++w)
									{
										zom_to_kill = battle_field[k][w].begin();
										while (zom_to_kill != battle_field[k][w].end())
										{
											if ((*zom_to_kill)->type == FieldObj::ZOMBIE)
											{
												zom = static_cast<Zombie *>(*zom_to_kill);
												zom->freeze();
											}
											++zom_to_kill;
										}
									}
								}

								delete plant;
								it = battle_field[i][j].erase(it);
								continue;
							}
						}
					}

					++it;
				}
				break;


			case 2: //仅僵尸
				while (it != battle_field[i][j].end())
				{
					killed = false;
					if (j == 0) //是底线之前的第一格
					{
						tomove_zom = static_cast<Zombie *>(*it)->frame(battle_field[i][j], killed, true);
						if (tomove_zom)
							return true;
					}

					else
					{
						tomove_zom = static_cast<Zombie *>(*it)->frame(battle_field[i][j - 1], killed, false);
						if (tomove_zom) //需要移动
						{
							battle_field[i][j - 1].push_back(*it); //将这个物件添加到上一格
							it = battle_field[i][j].erase(it);
							continue;
						}

						else if (killed) //植物被杀死
						{
							for (auto t = battle_field[i][j - 1].begin(); t != battle_field[i][j - 1].end(); ++t)
							{
								if ((*t)->type == FieldObj::PLANT)
								{
									delete static_cast<Plant *>(*t);
									t = battle_field[i][j - 1].erase(t);
									if (entered_shovel && shovel_loc.X == j - 1 && shovel_loc.Y == i)
										exit_shovel();
									break;
								}
							}
						}
					}

					++it;
				}
				break;


			case 3: //僵尸，子弹共存
				sort(battle_field[i][j].begin(), battle_field[i][j].end(), cmp);

				while ((*it)->type != FieldObj::ZOMBIE)
					++it;
				first_zom_pro = static_cast<Zombie *>(*it)->get_progress();

				endcnt = 0;  //endcnt = 1;
				while (endcnt < size)
				{
					if (battle_field[i][j][endcnt]->type == FieldObj::BULLET) //是子弹
					{
						bul = static_cast<Bullet*>(battle_field[i][j][endcnt]);
						bul->frame();
						bul_pro = bul->get_progress();

						if (bul_pro + first_zom_pro >= slot_w - 1) //碰撞了
						{
							//开始造成伤害，子弹要消失，僵尸要扣血，僵尸死了也要消失
							damage = bul->get_damage();
							first_zom = battle_field[i][j].erase(battle_field[i][j].begin() + endcnt);//删去该子弹
							for (; (*first_zom)->type != FieldObj::ZOMBIE; ++first_zom); //来到首个僵尸处
							zom = static_cast<Zombie *>(*first_zom);
							first_zom_pro = zom->get_progress();

							while (first_zom != battle_field[i][j].end())
							{
								zom = static_cast<Zombie *>(*first_zom);
								if (first_zom_pro != zom->get_progress())
									break;

								if (zom->recv_damage(damage))
								{   //杀死了僵尸
									score += zom->get_score();
									first_zom = battle_field[i][j].erase(first_zom);
									delete zom;
									continue;
								}
								++first_zom;
							}

							if (bul->bullettype == Bullet::MELONBULLET) //对该格所有僵尸造成伤害
							{
								while (first_zom != battle_field[i][j].end())
								{
									zom = static_cast<Zombie *>(*first_zom);
									if (zom->recv_damage(damage))
									{   //杀死了僵尸
										score += zom->get_score();
										first_zom = battle_field[i][j].erase(first_zom);
										delete zom;
										continue;
									}
									++first_zom;
								}
							}

							for (first_zom = battle_field[i][j].begin(); first_zom != battle_field[i][j].end() \
								&& (*first_zom)->type != FieldObj::ZOMBIE; ++first_zom); //来到首个僵尸处
							if (first_zom != battle_field[i][j].end())
								first_zom_pro = static_cast<Zombie *>(*first_zom)->get_progress();
							else
								first_zom_pro = -3; //全部僵尸都死了


							delete bul;
							size = battle_field[i][j].size();
							continue;
						} //没碰撞则不用管，因此没有else
					}


					else //开始处理僵尸
					{
						//僵尸一定不会移动到下一单元格，因此只需前进一小格就行
						static_cast<Zombie *>(battle_field[i][j][endcnt])->frame(battle_field[i][j], killed, false);
					}

					size = battle_field[i][j].size();
					++endcnt;
					//++it;
				}

				break;

			default: break;
			}
		}
	}

	return false;
}


void Field::end_screen()
{
	for (int i = 0; i < dis_h + gap + store_h; ++i)
	{
		for (int j = 0; j < dis_w; ++j)
		{
			dis[i][j] = ' ';
		}
	}

	strncpy(dis[0], "Game Over! Your final score is: ", 32);

	char t[10];
	_itoa(score, t, 10);
	strncpy(dis[0] + 32, t, strlen(t));
}


void Field::loop()
{
	char ch = 0;
	while (true)
	{
		Sleep(INTERVAL); //单位是毫秒. Linux中不要用这个，用sleep
		if (_kbhit())
			ch = _getch();
		else
			ch = 0;

		//时间间隔到来之后：进行处理
		//应调用专门的处理函数来进行处理

		if (frame(ch))
			break;

		print_all();
	}

	//显示游戏结束页面
	end_screen();

	for (int i = 0; i < dis_h + gap + store_h; i++)
	{
		coord.Y = i;
		WriteConsoleOutputCharacterA(current, dis[i], dis_w, coord, &bytes);
	}
}