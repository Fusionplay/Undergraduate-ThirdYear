#include <QPainter>
#include <stdio.h>
#include <assert.h>

#include "constants.h"
#include "gamecontroller.h"
#include "MySnake.h"
#include "food.h"


const qreal SNAKE_SIZE = 11;
const qreal MOVE_SIZE = 10;

Snake::Snake(GameController &controller, int speed) :
    head(0, 0), //头部初始化在中心位置
	add_length(4), //初始长度
	speed(speed),
    cnt(0),
	moveDirection(Still),
    controller(controller), //tail链表没有初始化
	normal_eaten(0)        //初始化为0
{
	tail << head;
}


QRectF Snake::boundingRect() const  //蛇的边界矩形：计算起来非常复杂！
{
    qreal minX = head.x(); //这些变量先初始化为蛇头的值
    qreal minY = head.y();
    qreal maxX = head.x();
    qreal maxY = head.y();

	for (QPointF p: tail)  //遍历蛇身的每个节点
    {
        maxX = p.x() > maxX ? p.x() : maxX;
        maxY = p.y() > maxY ? p.y() : maxY;
        minX = p.x() < minX ? p.x() : minX;
        minY = p.y() < minY ? p.y() : minY;
    }

    QPointF tl = mapFromScene(QPointF(minX, minY)); //top left
    QPointF br = mapFromScene(QPointF(maxX, maxY)); //bottom right

    //+ snake size?
    //why加10（snake size）：因为那个边界矩形是假的，要加上10才能完全包住
    QRectF bound = QRectF(tl.x(),  // x
                          tl.y(),  // y
                          br.x() - tl.x() + SNAKE_SIZE,      // width
                          br.y() - tl.y() + SNAKE_SIZE       //height
                          );
    return bound;
}


QPainterPath Snake::shape() const
{
    QPainterPath path; //A painter path is an object composed of a
    //number of graphical building blocks, such as rectangles, ellipses, lines, and curves

	path.setFillRule(Qt::WindingFill);

	QPointF p;
	for (int i = 0; i < tail.length() - 1; i++)
	{
		p = tail[i];
		QPointF itemp = mapFromScene(p); //场景scene坐标转换为视图view坐标
		path.addRect(QRectF(itemp.x(), itemp.y(), SNAKE_SIZE, SNAKE_SIZE));
		//path.addRect()函数将一个矩形路径添加到要绘图的地方去，后面调用时一次性把全部要绘制的图形都绘制出来
	}

	return path;
}


void Snake::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) //画出蛇
{
	painter->save();

	int ind = tail.length() - 1;
	QPointF p = mapFromScene(tail[ind]);
	QPainterPath path1;
	path1.setFillRule(Qt::WindingFill);
	path1.addRect(QRectF(p.x(), p.y(), SNAKE_SIZE, SNAKE_SIZE));
	painter->fillPath(path1, Qt::darkGreen);  //蛇头为暗绿色

	if (tail.length() > 1)
		painter->fillPath(shape(), Qt::green); //蛇身颜色设为绿色

    painter->restore();
}


QPainterPath Snake::wholeShape()
{
	QPainterPath path;
	path.setFillRule(Qt::WindingFill);

	for (QPointF p: tail)
	{
		QPointF itemp = mapFromScene(p); //场景scene坐标转换为视图view坐标
		path.addRect(QRectF(itemp.x(), itemp.y(), SNAKE_SIZE, SNAKE_SIZE));
	}

	return path;
}


void Snake::setMoveDirection(Direction direction)
{
	if (moveDirection == Left || moveDirection == Right)
    {
		if (direction != Left && direction != Right)
            moveDirection = direction;
    }

	else if (moveDirection == Up || moveDirection == Down)
    {
		if (direction != Up && direction != Down)
            moveDirection = direction;
    }

    else //初始情况，为nomove时
        moveDirection = direction;
}


bool Snake::eat(Food *food) //蛇吃食物
{//返回值表明下一个将要生成的食物是不是特殊食物

    if (food->Is_special_food())
	{
        return false;
    }

    else
    {
        normal_eaten++;
        if (normal_eaten == 5)
        {
			normal_eaten = 0;
            return true;
        }

        return false;
    }
}


void Snake::advance(int step)
{
	if (!step || cnt++ % speed != 0 || moveDirection == Still)
		return;

	if (add_length > 0)
	{
        tail << head; //直接在食物的位置增长！
		add_length -= 1;
    }

    else
    {
		tail.takeFirst(); //去除掉蛇尾的节点
		tail << head;     //追加head到头部
    }

	switch (moveDirection) //设置下一次advance后，蛇转向哪里移动
    {
		case Left: moveLeft(); break;
		case Right: moveRight(); break;
		case Up: moveUp(); break;
		case Down: moveDown(); break;
        default: break;
    }

	setPos(head); //设置蛇的位置
	handleCollisions();
}


void Snake::moveLeft()
{
	head.rx() -= MOVE_SIZE;
    if (head.rx() < -100)
        head.rx() = 90;
}


void Snake::moveRight()
{
	head.rx() += MOVE_SIZE;
    if (head.rx() >= 100)
        head.rx() = -100;
}

void Snake::moveUp()
{
	head.ry() -= MOVE_SIZE;
    if (head.ry() < -100)
        head.ry() = 90;
}


void Snake::moveDown()
{
	head.ry() += MOVE_SIZE;
    if (head.ry() >= 100)
        head.ry() = -100;
}


void Snake::handleCollisions()
{   //2种碰撞情况：吃到食物，吃到自己
	QList<QGraphicsItem *> collisions = collidingItems(); //获取all与当前对象发生碰撞的物件

	if (!collisions.isEmpty())
	{
		controller.snakeEatFood((Food *)collisions[0]);
		add_length += 1;
		return;
	}

    // Check snake eating itself
    for (QPointF p: tail) //fix one bug
    {
        if (p == head)
		{
			controller.gameOver();
            return;
        }
	}
}
