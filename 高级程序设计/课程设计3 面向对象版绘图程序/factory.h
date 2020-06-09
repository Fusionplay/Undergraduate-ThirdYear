#ifndef FACTORY_H
#define FACTORY_H

#include "drawalg.h"
#include "structs.h"


class Factory //工厂基类
{
public:
	virtual Item* createItem(int idpa, QList<QPoint> *ppa, const QPen& p) = 0;
};

//=============================================直线工厂子类================================

class LineFty: public Factory
{
public:
	Line* createItem(int idpa, QList<QPoint> *ppa, const QPen& p);
};

//=============================================椭圆工厂子类================================

class ElliFty: public Factory
{
public:
	Elli* createItem(int idpa, QList<QPoint> *ppa, const QPen& p);
};

//=============================================多边形工厂子类================================

class PolyFty: public Factory
{
public:
	Poly* createItem(int idpa, QList<QPoint> *ppa, const QPen& p);
};

//=============================================曲线工厂子类================================

class CurveFty: public Factory
{
public:
	Curve* createItem(int idpa, QList<QPoint> *ppa, const QPen& p);
};



#endif // FACTORY_H
