#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "field.h"
using namespace std;


int main()
{
	system("mode con cols=150 lines=32"); //cols为控制台的宽度，lines则代表控制台的高度。
	system("CHCP 65001");

	cout << "=====================Welcome to the Mini PvZ Game!=====================\n";
	cout << "=====================Press the Enter Key to Start!=====================\n";
	cin.get();


	Field f;
	f.loop();

	cin.get();
	return 0;
}