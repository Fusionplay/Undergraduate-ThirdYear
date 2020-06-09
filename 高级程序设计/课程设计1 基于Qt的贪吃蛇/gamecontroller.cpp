#include <QEvent>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QTime>
#include <assert.h>
#include <stdio.h>
#include "gamecontroller.h"
#include "food.h"
#include "MySnake.h"


GameController::GameController(QGraphicsScene &scene, int speed, QObject *parent) :
    QObject(parent),
    scene(scene),
	snake(new Snake(*this, speed)),
	game_speed(speed)
{
    timer.start( 1000 / 30 );  //定时器设置每一次响应的间隔
	spec_food = nullptr;       //一开始没有特殊食物
	Food *first_food = new Food(-80, -50, false, this, game_speed); //设置第一个食物的位置
	scene.addItem(first_food); //往场景中添加蛇和首个食物
	scene.addItem(snake);      //场景中添加snake
    scene.installEventFilter(this); //启动事件过滤器

	init_score(); //初始化比分
	qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
	resume();     //正式开始游戏
}


//void GameController::snakeAteFood(Food *food)
void GameController::snakeEatFood(Food *food)
{
    bool next_spec = snake->eat(food); //next_spec表明下一个将要生成的食物是不是特殊食物
    bool eaten_spec = food->Is_special_food();  //eaten_spec表明目前吃到的食物是不是特殊食物
    scene.removeItem(food);
    delete food;

    if (!eaten_spec) //吃到的不是特殊食物
	{
		addNewFood(next_spec);
		score++;
	}

    else
    {
        assert(food == spec_food); //他们指向同一地址
        spec_food = nullptr;
		score += 5;
    }
}


void GameController::del_spec(Food *food) //超时，删除场上的特殊食物
{
    assert(food == spec_food); //他们指向同一地址
    scene.removeItem(food);
    delete food;
	spec_food = nullptr;
}


void GameController::new_game()
{
	scene.clear();
	spec_food = nullptr;

	snake = new Snake(*this, game_speed);
	scene.addItem(snake);
	addNewFood(false);
	init_score();
	resume();
}


void GameController::handleKeyPressed(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Left:
			snake->setMoveDirection(Snake::Left);
            break;
        case Qt::Key_Right:
			snake->setMoveDirection(Snake::Right);
            break;
        case Qt::Key_Up:
			snake->setMoveDirection(Snake::Up);
            break;
        case Qt::Key_Down:
			snake->setMoveDirection(Snake::Down);
            break;

        case Qt::Key_A:
            pause();
            break;

        case Qt::Key_S:
            resume();
            break;
    }
}


void GameController::addNewFood(bool is_food_spec)
{
	int x, y;

    if (!is_food_spec && spec_food == nullptr) //不是特殊食物，且场上没有特殊食物
    {   //此时添加普通食物
        do
		{
			x = (qrand() % 20) * 10 - 100;
			y = (qrand() % 20) * 10 - 100;
		} while (snake->wholeShape().contains(snake->mapFromScene(QPointF(x + 5, y + 5))));

		Food *food1 = new Food(x , y, false, this, game_speed);   //添加普通食物
        scene.addItem(food1);
    }


    //else if (!is_food_spec && spec_food != nullptr) //不是特殊食物，且场上有特殊食物
    else if (spec_food != nullptr)
    {   //此时添加普通食物，需要考虑与特殊食物的碰撞问题
        do
		{
			x = (qrand() % 20) * 10 - 100;
			y = (qrand() % 20) * 10 - 100;
		} while (snake->wholeShape().contains(snake->mapFromScene(QPointF(x + 5, y + 5))) || \
                 spec_food->shape().contains(spec_food->mapFromScene(QPointF(x + 5, y + 5))));

		Food *food1 = new Food(x , y, false, this, game_speed);   //添加普通食物
        scene.addItem(food1);
    }


    else if (is_food_spec && spec_food == nullptr) //是特殊食物，且场上没有特殊食物
    {   //此时应添加两个食物！一个特殊一个不特殊
        do
		{
			x = (qrand() % 20) * 10 - 100;
			y = (qrand() % 20) * 10 - 100;
		} while (snake->wholeShape().contains(snake->mapFromScene(QPointF(x + 5, y + 5))));

		spec_food = new Food(x , y, true, this, game_speed);   //添加特殊食物
        scene.addItem(spec_food);


        do
		{
			x = (qrand() % 20) * 10 - 100;
			y = (qrand() % 20) * 10 - 100;
		} while (snake->wholeShape().contains(snake->mapFromScene(QPointF(x + 5, y + 5))) || \
                 spec_food->shape().contains(spec_food->mapFromScene(QPointF(x + 5, y + 5))));

		Food *food1 = new Food(x , y, false, this, game_speed);   //添加普通食物
        scene.addItem(food1);
    }
}


void GameController::gameOver()
{
	pause();
	emit send_end_signal();
}


void GameController::pause()
{
    disconnect(&timer, SIGNAL(timeout()), &scene, SLOT(advance())); //每当timeout时，就执行advance()
}


void GameController::resume()
{   //执行完这个connect后，整个程序的游戏循环就已经开始了
    connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()), Qt::UniqueConnection);
}


bool GameController::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
		handleKeyPressed(static_cast<QKeyEvent *>(event));
        return true;
    }

    else
        return QObject::eventFilter(object, event); //do nothing
}
