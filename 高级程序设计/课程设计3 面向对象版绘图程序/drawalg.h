#ifndef DRAWALG_H
#define DRAWALG_H

#include <QList>
#include <QPoint>

class Alg
{
public:
	void line_DDA(QList<QPoint>* line, int x1, int y1, int x2, int y2);
	void line_Bre(QList<QPoint>* line, int x1, int y1, int x2, int y2);

	void ellipse_mid(QList<QPoint>* elli, int x0, int y0, int a, int b);

	void Bezier(QList<QPoint>* plist, int pnum, QList<QPoint> *dots);
	void Bspline(QList<QPoint>* plist, int pnum, QList<QPoint> *dots);
};

#endif // DRAWALG_H
