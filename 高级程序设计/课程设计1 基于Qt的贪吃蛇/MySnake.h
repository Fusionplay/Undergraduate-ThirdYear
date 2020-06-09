#ifndef SNAKE_H
#define SNAKE_H

#include <QGraphicsItem>
#include <QRectF>

class GameController;
class Food;

class Snake : public QGraphicsItem //蛇也是一个Item，会被添加到scene中
{
public:
    enum Direction  //蛇的移动方向：可以是不移动。
	{Still, Left, Right, Up, Down};

	Snake(GameController &, int);

    QRectF boundingRect() const; //蛇的边界矩形
	QPainterPath shape() const; //提供蛇的形状以供绘制
	QPainterPath wholeShape();  //完整的形状
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *); //画出蛇

    void setMoveDirection(Direction direction); //设置移动方向
	bool eat(Food *food);       //蛇吃食物

protected:
	void advance(int step);    //“帧”控制函数，控制每一“帧”中该item的行为

private:
	void moveLeft();    //根据移动方向来控制蛇的移动，下同
    void moveRight();
    void moveUp();
    void moveDown();
	void handleCollisions(); //处理蛇与食物的碰撞

    QPointF        head;    //蛇头的位置
	int            add_length; //蛇的待生长的长度
	int            speed;   //速度
	QList<QPointF> tail;    //蛇的各个节点的位置
	int            cnt;
    Direction      moveDirection; //当前的移动方向
	GameController &controller; //将控制器也纳入自己名下，以在需要时调用控制器进行相应的处理

	int normal_eaten;  //吃掉的普通食物的数量
};

#endif // SNAKE_H
