#ifndef FOOD_H
#define FOOD_H

#include <QGraphicsItem>
class GameController;


class Food : public QGraphicsItem //food是一个graphics item
{
public:
	Food(qreal x, qreal y, bool normal, GameController *control, int speed); //坐标,以及是否是普通食物
    
    QRectF boundingRect() const; //计算边界矩形
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=Q_NULLPTR); //画出食物
	QPainterPath shape() const;  //提供食物的形状以供绘制

	bool Is_special_food() const {return is_spec;} //返回当前食物是否是特殊食物

private:
    bool is_spec; //是否是普通食物，初始化为false
	int cnt;
	GameController *control; //将控制器也纳入自己名下，以在需要时调用控制器进行相应的处理
	int speed;
	int frames;

protected:
    void advance(int step); //食物的每帧控制
};

#endif // FOOD_H
