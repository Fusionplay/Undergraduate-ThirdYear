#include <QPainter>
#include "constants.h"
#include "food.h"
#include "gamecontroller.h"



Food::Food(qreal x, qreal y, bool normal, GameController *control, int speed):
    is_spec(normal), //如果normal为false，则为普通食物
    cnt(0),
	control(control),
	speed(speed),
	frames(0)
{
	setPos(x, y);  //设置其在父坐标系,也即scene中的位置
}


QRectF Food::boundingRect() const
{
	return QRectF(-TILE_SIZE, -TILE_SIZE, TILE_SIZE * 2, TILE_SIZE * 2);
}


void Food::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) //画出图形
{
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);

    if (!is_spec)
        painter->fillPath(shape(), Qt::red);
    else
        painter->fillPath(shape(), Qt::blue); //特殊食物是蓝色的

    painter->restore();
}


QPainterPath Food::shape() const
{   //返回的是相应的painterpath
    QPainterPath p;
	p.addEllipse(QPointF(TILE_SIZE / 2 + 0.45, TILE_SIZE / 2 + 0.45), FOOD_RADIUS, FOOD_RADIUS);
	return p;
}


void Food::advance(int step)
{
	if (0 == step || cnt++ % speed != 0 || is_spec == false) //不是特殊食物：不处理
        return;

	frames++;
	if (frames == TIMELAP)
		control->del_spec(this);
}


