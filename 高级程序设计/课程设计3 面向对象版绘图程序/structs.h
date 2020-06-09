#ifndef STRUCTS_H
#define STRUCTS_H

#include <QPen>
#include <QPainter>
#include <QList>
#include <math.h>
#include "drawalg.h"


class Item //图元基类
{
public:
	enum Type
	{
		ELLI, //椭圆
		LINE, //直线
		POLYGON, //多边形
		CURVE //曲线
	};

	Item(int idpa, QList<QPoint> *ppa, const QPen& p);
	virtual ~Item();

	//平移
	virtual void translate(int dx, int dy) = 0;
	void trans_point(QPoint &po, int dx, int dy);

	//旋转
	virtual void rotate(int x, int y, int r) = 0;
	void rotate_point(QPoint &po, int x, int y, double theta);

	//缩放
	virtual void scale(int x, int y, double s) = 0;
	void scale_point(QPoint &po, double s, double xtmp, double ytmp);

	//画图元
	void draw(QPainter& p);

	//图元的其它信息
	Type type; //图元的类型
	int id;    //图元ID
	QPen pen;  //画笔信息，用来存储绘制当前图元的画笔的颜色

protected:
	QList<QPoint> *plist; //图元的点集
	Alg algo; //绘图算法
};


//===============================================直线=======================================


class Line: public Item
{
public:
	Line(int idpa, QList<QPoint> *ppa, const QPen& p);

	void set_sp(QPoint spp);
	void set_ep(QPoint epp);
	QPoint get_sp() const;
	QPoint get_ep() const;

	int encode(QPoint p, int xmin, int xmax, int ymin, int ymax);
	bool clip_cohen(int x1, int y1, int x2, int y2); //cohen裁剪算法
	bool ClipT(int p, int q, float &u1, float &u2);
	bool clip_liang(int x1, int y1, int x2, int y2); //liang裁剪算法

	void translate(int dx, int dy);
	void rotate(int x, int y, int r);
	void scale(int x, int y, double s);

private:
	QPoint sp; //直线的起点
	QPoint ep; //直线的终点
};


//===============================================椭圆==========================================


class Elli: public Item
{
public:
	Elli(int idpa, QList<QPoint> *ppa, const QPen& p);

	void set_a(int ap);
	void set_b(int bp);
	void set_mid(QPoint midp);
	int get_a() const;
	int get_b() const;
	QPoint get_mid() const;

	void translate(int dx, int dy);
	void rotate(int x, int y, int r);
	void scale(int x, int y, double s);

private:
	QPoint mid; //椭圆的中心点
	int a; //椭圆的x轴半径
	int b; //椭圆的y轴半径
};


//===============================================多边形=======================================


class Poly: public Item
{
public:
	Poly(int idpa, QList<QPoint> *ppa, const QPen& p);
	~Poly();

	void set_nodelist(QList<QPoint> *plist);
	void set_nodelist(QPoint *plist, int pnum);
	QPoint get_1st() const;

	void translate(int dx, int dy);
	void rotate(int x, int y, int r);
	void scale(int x, int y, double s);

private:
	QList<QPoint> *nodelist = nullptr; //多边形的节点数组
};


//================================================曲线=====================================


class Curve: public Item
{
public:
	Curve(int idpa, QList<QPoint> *ppa, const QPen& p);
	~Curve();

	void set_nodelist(QList<QPoint> *plist);
	void set_nodelist(QPoint *plist, int pnum);
	QPoint get_1st() const;
	void set_curvetype(int t);
	int get_curvetype() const;

	void translate(int dx, int dy);
	void rotate(int x, int y, int r);
	void scale(int x, int y, double s);

private:
	QList<QPoint> *nodelist = nullptr; //曲线的节点数组
	int curvetype; //曲线的类型：0 = bezier 1 = B-spline
};

#endif // STRUCTS_H
