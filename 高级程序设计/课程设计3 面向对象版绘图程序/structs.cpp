#include "structs.h"

const double PI = 3.1416;

//=============================================Item基类===============================================

Item::Item(int idpa, QList<QPoint> *ppa, const QPen& p)
{id = idpa; plist = ppa; pen = p;}

Item::~Item()
{delete plist;}


void Item::draw(QPainter& p)
{
	p.setPen(pen);
	for (QPoint po: *plist)
		p.drawPoint(po);
}


void Item::trans_point(QPoint &po, int dx, int dy)
{
	po.rx() += dx;
	po.ry() += dy;
}


void Item::rotate_point(QPoint &po, int x, int y, double theta)
{
	int xtmp = po.x() - x;
	int ytmp = po.y() - y;
	po.rx() = (int)(xtmp * cos(theta) - ytmp * sin(theta) + x);
	po.ry() = (int)(xtmp * sin(theta) + ytmp * cos(theta) + y);
}


void Item::scale_point(QPoint &po, double s, double xtmp, double ytmp)
{
	po.rx() = (int)(po.x() * s + xtmp);
	po.ry() = (int)(po.y() * s + ytmp);
}


//=============================================Line子类===============================================


Line::Line(int idpa, QList<QPoint> *ppa, const QPen& p): Item(idpa, ppa, p)
{
	type = LINE;
}


void Line::set_sp(QPoint spp) {sp = spp;}
void Line::set_ep(QPoint epp) {ep = epp;}
QPoint Line::get_sp() const {return sp;}
QPoint Line::get_ep() const {return ep;}


int Line::encode(QPoint p, int xmin, int xmax, int ymin, int ymax)
{
	int code = 0;
	if (p.x() < xmin)      //左
		code += 1;
	else if (p.x() > xmax) //右
		code += 2;
	if (p.y() < ymin)      //下
		code += 4;
	else if (p.y() > ymax) //上
		code += 8;
	return code;
}


bool Line::ClipT(int p, int q, float &u1, float &u2)
{        //-dx,   p1.x - x1,    u1,      u2
	float r;
	if (p < 0)
	{
		r = (float)q / p;
		if (r > u2)
			return false;
		if (r > u1)
			u1 = r;
	}
	else if (p > 0)
	{
		r = (float)q / p;
		if (r < u1)
			return false;
		if (r < u2)
			u2 = r;
	}
	else
		return q >= 0;
	return true;
}


bool Line::clip_cohen(int x1, int y1, int x2, int y2) //cohen裁剪算法
{
/*
 *首先找出线段的首点和终点，encode之。
*/
	int xmin = x1;
	int xmax = x2;
	int ymin = y1;
	int ymax = y2;
	int codesp = encode(sp, xmin, xmax, ymin, ymax);
	int codeep = encode(ep, xmin, xmax, ymin, ymax);

	double slope;
	double slopemax = std::numeric_limits<double>::max();
	if (sp.y() == ep.y())
		slope = 0;
	else if (sp.x() == ep.x())
		slope = slopemax;
	else
		slope = ((ep.y() - sp.y()) * 1.0) / (ep.x() - sp.x());

	while (codesp != 0 || codeep != 0) //直到两者都等于0才结束
	{
		if ((codesp & codeep) != 0) //完全在窗口外
			return true; //要删除该item

		if ((codesp | codeep) == 0) //完全在窗口内
			break;

		//处理大于上界的情况
		if ((codesp & 8) != 0) //sp处于上方
		{
			if (slope != slopemax)
				sp.rx() -= (sp.y() - ymax) / slope;
			sp.ry() = ymax;
			codesp = encode(sp, xmin, xmax, ymin, ymax);
		}
		else if ((codeep & 8) != 0) //ep处于上方
		{
			if (slope != slopemax)
				ep.rx() -= (ep.y() - ymax) / slope;
			ep.ry() = ymax;
			codeep = encode(ep, xmin, xmax, ymin, ymax);
		}

		//处理小于下界的情况
		else if ((codesp & 4) != 0) //sp处于下方
		{
			if (slope != slopemax)
				sp.rx() -= (sp.y() - ymin) / slope;
			sp.ry() = ymin;
			codesp = encode(sp, xmin, xmax, ymin, ymax);
		}
		else if ((codeep & 4) != 0) //ep处于下方
		{
			if (slope != slopemax)
				ep.rx() -= (ep.y() - ymin) / slope;
			ep.ry() = ymin;
			codeep = encode(ep, xmin, xmax, ymin, ymax);
		}

		//处理大于右界的点
		else if ((codesp & 2) != 0)
		{
			sp.ry() -= slope * (sp.x() - xmax);
			sp.rx() = xmax;
			codesp = encode(sp, xmin, xmax, ymin, ymax);
		}
		else if ((codeep & 2) != 0)
		{
			ep.ry() -= slope * (ep.x() - xmax);
			ep.rx() = xmax;
			codeep = encode(ep, xmin, xmax, ymin, ymax);
		}

		//处理小于左边界的点
		else if ((codesp & 1) != 0)
		{
			sp.ry() -= slope * (sp.x() - xmin);
			sp.rx() = xmin;
			codesp = encode(sp, xmin, xmax, ymin, ymax);
		}
		else if ((codeep & 1) != 0)
		{
			ep.ry() -= slope * (ep.x() - xmin);
			ep.rx() = xmin;
			codeep = encode(ep, xmin, xmax, ymin, ymax);
		}
	}

	//重新绘制直线
	plist->clear();
	algo.line_DDA(plist, sp.x(), sp.y(), ep.x(), ep.y());
	return false;
}


bool Line::clip_liang(int x1, int y1, int x2, int y2) //liang裁剪算法
{
	float u1 = 0;
	float u2 = 1;
	QPoint p1 = sp;
	QPoint p2 = ep;
	bool t = false;
	int dx = p2.x() - p1.x();
	int dy = p2.y() - p1.y();
	if (ClipT(-dx, p1.x() - x1, u1, u2))
	{
		if (ClipT(dx, x2 - p1.x(), u1, u2))
		{
			if (ClipT(-dy, p1.y() - y1, u1, u2))
			{
				if (ClipT(dy, y2 - p1.y(), u1, u2))
					t = true;
			}
		}
	}

	if (t) //重新绘制直线
	{
		plist->clear();
		sp = QPoint(p1.x() + u1 * dx, p1.y() + u1 * dy);
		ep = QPoint(p1.x() + u2 * dx, p1.y() + u2 * dy);
		algo.line_DDA(plist, sp.x(), sp.y(), ep.x(), ep.y());
		return false;
	}

	else
		return true;
}


void Line::translate(int dx, int dy)
{
	for (QPoint &po: *plist)
		trans_point(po, dx, dy);
	trans_point(sp, dx, dy);
	trans_point(ep, dx, dy);
}

void Line::rotate(int x, int y, int r)
{
	double theta = (PI / 180) * (double)r;
	plist->clear(); //清空点集
	rotate_point(sp, x, y, theta);
	rotate_point(ep, x, y, theta);
	algo.line_DDA(plist, sp.x(), sp.y(), ep.x(), ep.y()); //使用DDA算法重新生成线段
}

void Line::scale(int x, int y, double s)
{
	double xtmp = x * (1 - s);
	double ytmp = y * (1 - s);
	plist->clear(); //清空点集
	scale_point(sp, s, xtmp, ytmp);
	scale_point(ep, s, xtmp, ytmp);
	algo.line_DDA(plist, sp.x(), sp.y(), ep.x(), ep.y()); //使用DDA算法重新生成线段
}


//=============================================Elli子类===============================================


Elli::Elli(int idpa, QList<QPoint> *ppa, const QPen& p): Item(idpa, ppa, p)
{
	type = ELLI;
}

void Elli::set_a(int ap) {a = ap;}
void Elli::set_b(int bp) {b = bp;}
void Elli::set_mid(QPoint midp) {mid = midp;}
int Elli::get_a() const {return a;}
int Elli::get_b() const {return b;}
QPoint Elli::get_mid() const {return mid;}


void Elli::translate(int dx, int dy)
{
	for (QPoint &po: *plist)
		trans_point(po, dx, dy);
	trans_point(mid, dx, dy);
}

void Elli::rotate(int x, int y, int r)
{
	double theta = (PI / 180) * (double)r;
	plist->clear();
	rotate_point(mid, x, y, theta);
	algo.ellipse_mid(plist, mid.x(), mid.y(), a, b);
}

void Elli::scale(int x, int y, double s)
{
	double xtmp = x * (1 - s);
	double ytmp = y * (1 - s);
	plist->clear(); //清空点集
	a = (int)(a * s);
	b = (int)(b * s);
	scale_point(mid, s, xtmp, ytmp);
	algo.ellipse_mid(plist, mid.x(), mid.y(), a, b);
}


//=============================================Poly子类===============================================


Poly::Poly(int idpa, QList<QPoint> *ppa, const QPen& p): Item(idpa, ppa, p)
{
	type = POLYGON;
}

Poly::~Poly() {if (nodelist != nullptr) delete nodelist;}

void Poly::set_nodelist(QList<QPoint> *plist)
{
	nodelist = new QList<QPoint>;
	*nodelist = *plist;
}

void Poly::set_nodelist(QPoint *plist, int pnum)
{
	nodelist = new QList<QPoint>;
	for (int i = 0; i < pnum; ++i)
		nodelist->append(plist[i]);
}

QPoint Poly::get_1st() const {return nodelist->first();}


void Poly::translate(int dx, int dy)
{
	for (QPoint &po: *plist)
		trans_point(po, dx, dy);
	for (QPoint &po: *nodelist)
		trans_point(po, dx, dy);
}

void Poly::rotate(int x, int y, int r)
{
	double theta = (PI / 180) * (double)r;
	plist->clear(); //清空点集
	int len = nodelist->size();
	for (int i = 0; i < len; ++i) //旋转多边形的各个节点
		rotate_point((*nodelist)[i], x, y, theta);

	for (int i = 0; i < len; ++i) //重新生成各条直线
	{
		algo.line_DDA(plist, (*nodelist)[i].x(), (*nodelist)[i].y(), \
				 (*nodelist)[(i+1)%len].x(), (*nodelist)[(i+1)%len].y());
	}
}

void Poly::scale(int x, int y, double s)
{
	double xtmp = x * (1 - s);
	double ytmp = y * (1 - s);
	plist->clear(); //清空点集
	int len = nodelist->size();
	for (int i = 0; i < len; ++i) //旋转多边形的各个节点
		scale_point((*nodelist)[i], s, xtmp, ytmp);

	for (int i = 0; i < len; ++i) //重新生成各条直线
	{
		algo.line_DDA(plist, (*nodelist)[i].x(), (*nodelist)[i].y(), \
				 (*nodelist)[(i+1)%len].x(), (*nodelist)[(i+1)%len].y());
	}
}


//=============================================Poly子类===============================================


Curve::Curve(int idpa, QList<QPoint> *ppa, const QPen& p): Item(idpa, ppa, p)
{
	type = CURVE;
}

Curve::~Curve() {if (nodelist != nullptr) delete nodelist;}

void Curve::set_nodelist(QList<QPoint> *plist)
{
	nodelist = new QList<QPoint>;
	*nodelist = *plist;
}

void Curve::set_nodelist(QPoint *plist, int pnum)
{
	nodelist = new QList<QPoint>;
	for (int i = 0; i < pnum; ++i)
		nodelist->append(plist[i]);
}

QPoint Curve::get_1st() const {return plist->first();}
void Curve::set_curvetype(int t) {curvetype = t;}
int Curve::get_curvetype() const {return curvetype;}

void Curve::translate(int dx, int dy)
{
	for (QPoint &po: *plist)
		trans_point(po, dx, dy);
	for (QPoint &po: *nodelist)
		trans_point(po, dx, dy);
}

void Curve::rotate(int x, int y, int r)
{
	double theta = (PI / 180) * (double)r;
	plist->clear(); //清空点集
	int len = nodelist->size();
	for (int i = 0; i < len; ++i) //旋转曲线的各个节点
		rotate_point((*nodelist)[i], x, y, theta);

	//重新生成曲线
	if (curvetype == 0) //bezier
		algo.Bezier(plist, len, nodelist);
	else
		algo.Bspline(plist, len, nodelist);
}

void Curve::scale(int x, int y, double s)
{
	double xtmp = x * (1 - s);
	double ytmp = y * (1 - s);
	plist->clear(); //清空点集
	int len = nodelist->size();
	for (int i = 0; i < len; ++i) //旋转曲线的各个节点
		scale_point((*nodelist)[i], s, xtmp, ytmp);

	//重新生成曲线
	if (curvetype == 0) //bezier
		algo.Bezier(plist, len, nodelist);
	else
		algo.Bspline(plist, len, nodelist);
}
