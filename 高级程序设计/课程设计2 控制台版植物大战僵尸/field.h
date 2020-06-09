#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WIN32_WINNT 0x0501
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include <conio.h>
#include <tchar.h>
#include <cassert>

#include "field_object.h"
#include "plant.h"
#include "zombie.h"
#include "bullet.h"
using namespace std;


//要解决如何显示战场上的物件的问题
class Field //包括商店在内的战场
{
public:
	Field();
	
	void loop();


private:
	void print_all();   //根据battle_field数组的内容，将元素打印到战场上

	bool frame(char ch);

	//CONSTANTS
	const int INTERVAL = 70; //时间间隔为70ms

	const int sun_tick_cnt = 20;  //每多少帧才更新一次阳光
	const int zom_tick_cnt = 20;  //每多少帧才产生新僵尸

	const int num_of_plants_per_row = 3;
	const int gap_of_plant_name = 23;


	static const int row_num = 3;
	static const int col_num = 7;
	static const int slot_w = 15; //宽度为15
	static const int slot_h = 6; //高度为6

	static const int dis_h = 1 + row_num * (slot_h + 1); // = 22
	static const int dis_w = 1 + (col_num + 1) * (slot_w + 1); // = 129

	static const int gap = 2;  //商店与战场之间的空隙
	static const int store_h = 7; //商店的高度
	static const int store_plantline_h = 3; //商店中植物区的高度


	static const int plant_num = 10;
	static const int zom_name_len = 7;

	
	//变量
	vector<FieldObj *> battle_field[row_num][col_num + 1];  //战场：僵尸，植物，炮弹。多一列的原因：僵尸要一列
	char dis[dis_h + gap + store_h][1 + dis_w]; //实际显示的战场:包括商店

	//游戏主体逻辑用变量
	int sun; //当前阳光总数
	int score; //当前分数
	int price[plant_num + 1]; //各个植物的价格
	int sun_add;   //自然阳光数
	int sun_plus;  //每个向日葵在每个tick产生的阳光数
	int sun_tick; //阳光更新计数变量
	int zom_tick;                 //僵尸产生计数变量
	

	//显示到控制台的过程中用的变量
	HANDLE h1, current;//控制台屏幕缓冲区句柄
	COORD coord; //双缓冲处理显示
	DWORD bytes;
	COORD cur_point;
	CONSOLE_CURSOR_INFO cci;   //控制光标的显示

	//商店交互用变量
	bool plant_selected; //判断当前是否处于已经选好植物，准备放置的阶段
	bool entered_store;  //判断当前是否已进入商店
	bool entered_shovel; //是否已经选中了铲子
	Plant::PlantType choosed_plant; //当前已经选择好，准备放置的植物
	COORD plant_loc;
	COORD shovel_loc;

	//其它函数
	void setup_field(); //初始化战场
	void init_store_vars();
	void init_shovel();
	bool move_to_1st_available_slot();
	bool move_to_next_plant(bool first = true);
	bool OK_to_plant(vector<FieldObj *> &place);
	void exit_store();
	void exit_shovel();
	void handle_key_press(char ch);
	void handle_sun();
	void generate_new_zombie();
	int slot_type_f(vector<FieldObj *> &place);
	void end_screen();
};
