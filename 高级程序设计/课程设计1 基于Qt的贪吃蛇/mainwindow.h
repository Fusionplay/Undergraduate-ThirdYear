#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "end_window.h"
#include "startwindow.h"

class QGraphicsScene;
class QGraphicsView;
class GameController;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
	MainWindow(QWidget *parent = nullptr);


private slots:
	void adjustViewSize(); //调整view
	void show_end_wind();  //显示结束窗口
	void new_game();       //开始新游戏

	void easy();           //以简单模式开始游戏的槽函数
	void moderate();       //以中等模式开始游戏的槽函数
	void hard();           //以困难模式开始游戏的槽函数

private:
	void initScene();           //初始化场景的大小
	void initSceneBackground(); //初始化场景的背景

	void init(int speed);   //接收到游戏速度后，进行初始化并开始游戏

    QGraphicsScene *scene;  //场景
    QGraphicsView *view;    //视图
    GameController *game;   //控制整个游戏的game变量
	end_window *endwind;    //游戏结束时显示的窗口
	StartWindow *startwind; //游戏开始时的界面
};

#endif // MAINWINDOW_H
