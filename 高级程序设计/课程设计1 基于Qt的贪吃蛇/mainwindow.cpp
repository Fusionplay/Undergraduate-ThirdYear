#include <QGraphicsView>
#include <QTimer>

#include "constants.h"
#include "gamecontroller.h"
#include "mainwindow.h"

const int EASY = 3;
const int MODERATE = 2;
const int HARD = 1;
const int WINDSIZE = 600;


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
	endwind = new end_window(this);
	startwind = new StartWindow(this);

	//新开游戏的信号
	connect(endwind, SIGNAL(newgame()), this, SLOT(new_game()));
	connect(startwind, SIGNAL(easymode()), this, SLOT(easy()));
	connect(startwind, SIGNAL(moderatemode()), this, SLOT(moderate()));
	connect(startwind, SIGNAL(hardmode()), this, SLOT(hard()));

	startwind->show();
}


void MainWindow::init(int speed)
{
	scene = new QGraphicsScene(this);  //初始化scene
	view = new QGraphicsView(scene, this); //用scene来初始化view
	game = new GameController(*scene, speed, this); //初始化整个游戏的控制变量

	//将游戏结束的信号连接到用于显示结束界面的槽函数
	connect(game, SIGNAL(send_end_signal()), this, SLOT(show_end_wind()));

	//对显示窗口进行设置
	setCentralWidget(view); //把view这个widget设为显示窗口的中心widget
	setFixedSize(WINDSIZE, WINDSIZE);//设置显示窗口的大小，并不允许用户修改窗口大小

	initScene();            //设置场景矩形
	initSceneBackground();  //设置场景的背景：纵横网格

	QTimer::singleShot(0, this, SLOT(adjustViewSize())); //放入事件队列，下个循环周期开始调用
}


void MainWindow::adjustViewSize() //调整view的大小
{
	view->fitInView(scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
}


void MainWindow::show_end_wind()
{
	int score = game->get_score();
	endwind->show_window(score);
}


void MainWindow::new_game()
{
	endwind->hide();
	game->new_game();
}


void MainWindow::easy()
{
	startwind->hide();
	init(EASY);
	show();
}


void MainWindow::moderate()
{
	startwind->hide();
	init(MODERATE);
	show();
}


void MainWindow::hard()
{
	startwind->hide();
	init(HARD);
	show();
}


void MainWindow::initScene()
{
	scene->setSceneRect(-100, -100, 200, 200); //设置场景矩形：200 * 200的长度
}


void MainWindow::initSceneBackground()
{
	QPixmap bg(TILE_SIZE, TILE_SIZE); //tile_size = 10
    QPainter p(&bg);
	p.setBrush(QBrush(Qt::gray)); //bg是一个paint device,在它上面绘制之后，bg就有了东西！
	p.drawRect(0, 0, TILE_SIZE, TILE_SIZE);
	//bg.save("bgggg.bmp");
	scene->setBackgroundBrush(QBrush(bg));
}



