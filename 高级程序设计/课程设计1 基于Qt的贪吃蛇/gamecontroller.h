#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QTimer>

class QGraphicsScene;
class QKeyEvent;

class Snake;
class Food;


class GameController : public QObject
{
    Q_OBJECT

public:
	GameController(QGraphicsScene &scene, int speed, QObject *parent = nullptr);

	void snakeEatFood(Food *food); //蛇吃东西
	void del_spec(Food *food);     //删除场上的特殊食物，添加普通食物
	int get_score() const {return score;}  //获取最终得分
	void init_score() {score = 0;} //游戏开始时，将得分初始化为0
	void new_game();               //开始新游戏

signals:
	void send_end_signal();        //发送游戏结束的信号给主窗体

public slots:
	void pause();    //暂停游戏
	void resume();   //继续游戏
    void gameOver(); //游戏结束

protected:
     bool eventFilter(QObject *object, QEvent *event); //事件过滤器:仅接受键盘事件

private:
	void handleKeyPressed(QKeyEvent *event); //按键的响应
	void addNewFood(bool is_food_spec);      //增加新的食物

    QTimer timer; //计时器
	QGraphicsScene &scene; //场景的引用,使得游戏控制器得以控制场景
    Snake *snake;  //蛇对象

	Food *spec_food;  //指向场上的特殊食物的指针
	int score;        //得分
	int game_speed;   //游戏速度
};

#endif // GAMECONTROLLER_H
