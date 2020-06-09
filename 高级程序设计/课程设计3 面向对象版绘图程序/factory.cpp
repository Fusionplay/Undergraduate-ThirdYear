#include "factory.h"

Line* LineFty::createItem(int idpa, QList<QPoint> *ppa, const QPen& p)
{
	return new Line(idpa, ppa, p);
}

Elli* ElliFty::createItem(int idpa, QList<QPoint> *ppa, const QPen& p)
{
	return new Elli(idpa, ppa, p);
}

Poly* PolyFty::createItem(int idpa, QList<QPoint> *ppa, const QPen& p)
{
	return new Poly(idpa, ppa, p);
}

Curve* CurveFty::createItem(int idpa, QList<QPoint> *ppa, const QPen& p)
{
	return new Curve(idpa, ppa, p);
}
