#include <limits>
#include "drawalg.h"

void Alg::line_DDA(QList<QPoint> *line, int x1, int y1, int x2, int y2)
{
	int temp;
	if (x1 > x2)
	{
		temp = x2;
		x2 = x1;
		x1 = temp;
		temp = y2;
		y2 = y1;
		y1 = temp;
	}

	if (x1 == x2)
	{
		if (y1 > y2)
		{
			int temp = y2;
			y2 = y1;
			y1 = temp;
		}

		for (int i = y1; i <= y2; i++)
			line->append(QPoint(x1, i));
	}

	else if (y1 == y2)
	{
		for (int i = x1; i <= x2; i++)
			line->append(QPoint(i, y1));
	}

	else
	{
		int len;
		if ((x2 - x1) >= abs(y2 - y1))
			len = x2 - x1;
		else
			len = abs(y2 - y1);

		double delta_x = (x2 - x1) / (len * 1.0);
		double delta_y = (y2 - y1) / (len * 1.0);
		double xtemp = x1 + 0.5;
		double ytemp = y1 + 0.5;

		for (int i=0;i<len;i++)
		{
			line->append(QPoint((int)xtemp, (int)ytemp));
			xtemp += delta_x;
			ytemp += delta_y;
		}
		line->append(QPoint(x2, y2));
	}
}


void Alg::line_Bre(QList<QPoint> *line, int x1, int y1, int x2, int y2)
{
	int x, y, dx, dy, s1, s2, p, toswap, i;
	x = x1;
	y = y1;
	dx = abs(x2 - x1);
	dy = abs(y2 - y1);

	if (x2 > x1)
		s1 = 1;
	else
		s1 = -1;
	if (y2 > y1)
		s2 = 1;
	else
		s2 = -1;

	if (dy > dx)
	{
		int temp = dx;
		dx = dy;
		dy = temp;
		toswap = 1;
	}
	else
		toswap = 0;

	p = 2 * dy - dx;
	for (i = 1; i <= dx; i++)
	{
		line->append(QPoint(x, y));
		if (p >= 0)
		{
			if (toswap == 0)
				y += s2;
			else
				x += s1;
			p = p - 2 * dx;
		}
		if (toswap == 0)
			x += s1;
		else
			y += s2;

		p += 2 * dy;
	}
	line->append(QPoint(x2, y2));
}


inline void add4Points(QList<QPoint> *line, int x0, int y0, int x, int y)
{
	line->append(QPoint(x0 + x, y0 + y));
	line->append(QPoint(x0 + x, y0 - y));
	line->append(QPoint(x0 - x, y0 + y));
	line->append(QPoint(x0 - x, y0 - y));
}


void Alg::ellipse_mid(QList<QPoint> *elli, int x0, int y0, int a, int b)
{
	bool is_laying = true;

	if (b > a)
	{
		is_laying = false;
		int temp = b;
		b = a;
		a = temp;
	}

	double sqa = a * a;
	double sqb = b * b;

	double d = sqb + sqa * (0.25 - b);
	int x = 0;
	int y = b;

	//首个添加的
	if (is_laying)
		add4Points(elli, x0, y0, x, y); //x=0, y=b
	else
		add4Points(elli, x0, y0, y, x); //x=b, y=0
	//椭圆的第一象限的上半部分
	while (sqb*(x + 1) < sqa*(y - 0.5))
	{
		if (d < 0)
			d += sqb*(2 * x + 3);

		else
		{
			d += (sqb*(2 * x + 3) + sqa*((-2)*y + 2));
			y--;
		}

		x++;
		if (is_laying)
			add4Points(elli, x0, y0, x, y);
		else
			add4Points(elli, x0, y0, y, x);
	}

	//椭圆的第一象限的下半部分
	d = (b * (x + 0.5)) * 2 + (a * (y - 1)) * 2 - (a * b) * 2;
	while (y >= 0)
	{
		if (d < 0)
		{
			d += sqb * (2 * x + 2) + sqa * ((-2) * y + 3);
			x++;
		}

		else
			d += sqa * ((-2) * y + 3);
		y--;
		if (is_laying)
			add4Points(elli, x0, y0, x, y);
		else
			add4Points(elli, x0, y0, y, x);
	}
}


int fac(int n)
{
	int res = 1;
	for (int i = 1; i <= n; ++i)
		res *= i;
	return res;
}


int combination(int m, int n)
{
	return fac(m) / (fac(n) * fac(m - n));
}


double bez(int i, int n, double u)
{
	return combination(n, i) * pow(u, i) * pow(1 - u, n - i);
}


void Alg::Bezier(QList<QPoint> *plist, int pnum, QList<QPoint> *dots)
{
	int dotnum = 128 * pnum;
	double interval = 1.0 / dotnum;
	double u = 0.0;
	QPoint tmp;
	double t;
	double xt;
	double yt;

	for (int i = 0; i <= dotnum; ++i)
	{
		xt = yt = 0.0;
		for (int j = 0; j < pnum; ++j)
		{
			tmp = (*dots)[j];
			t = bez(j, pnum-1, u);
			xt += (double)tmp.x() * t;
			yt += (double)tmp.y() * t;
		}

		plist->append(QPoint((int)(xt + 0.5), (int)(yt + 0.5)));
		u += interval;
	}
}


double N1(int i, double u) //1阶
{
	double t = u - i;

	if (0 <= t && t < 1)
		return t;
	if (1 <= t && t < 2)
		return 2 - t;
	return 0;
}

double N2(int i, double u)
{
	double t = u - i;

	if (0 <= t && t < 1)
		return 0.5 * t * t;
	if (1 <= t && t < 2)
		return 3*t - t*t - 1.5;
	if (2 <= t && t < 3)
		return 0.5 * pow(3 - t, 2);
	return 0;
}

double N3(int i, double u)
{
	double t = u - i;
	double a = 1.0 / 6.0;

	if (0 <= t && t < 1)
		return a*t*t*t;
	if (1 <= t && t < 2)
		return a * (-3*pow(t-1, 3) + 3*pow(t-1, 2) + 3*(t-1) + 1);
	if (2 <= t && t < 3)
		return a * (3*pow(t-2, 3) - 6*pow(t-2, 2) + 4);
	if (3 <= t && t < 4)
		return a * pow(4-t, 3);
	return 0;
}

double N(int k, int i, double u)
{
	switch(k)
	{
	case 1:
		return N1(i, u);
	case 2:
		return N2(i, u);
	case 3:
		return N3(i, u);
	}
}


void Alg::Bspline(QList<QPoint> *plist, int pnum, QList<QPoint> *dots)
{
	int currentK = 2;
	QPoint tmp;
	double t;
	double xt;
	double yt;
	double intval = 1.0 / 256.0;

	for (double u = currentK; u < pnum; u += intval)
	{
		xt = yt = 0.0;
		for (int i = 0; i < pnum; ++i)
		{
			tmp = (*dots)[i];
			t = N(currentK, i, u);
			xt += (double)tmp.x() * t;
			yt += (double)tmp.y() * t;
		}
		plist->append(QPoint((int)(xt + 0.5), (int)(yt + 0.5)));
	}
}
