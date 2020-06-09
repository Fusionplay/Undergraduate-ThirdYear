#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "display.h"
#include "ui_display.h"

const int WIND_CO = 6;
//const int LINE_NUM = 19;


display::display(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::display)
{
	ui->setupUi(this);
	hehe = nullptr;
	pen.setWidth(1);
}


display::~display()
{
	delete ui;
	if (hehe != nullptr)
		delete hehe;
}


void display::paintEvent(QPaintEvent *event)
{
	QPainter p2(this);
	p2.drawPixmap(rect(), *hehe, QRect());  //自动调整比例大小关系
}


void display::draw()
{
	//如果size是100 * 100, 则drawRect时每个坐标轴都要加2，新建Pixmap时每个坐标轴要加1起始点也都要是-1才行，
	char line[100];
	FILE *fp = fopen("input.txt", "r");
	char *ptr;

	while (true)
	{
		if (!feof(fp))
			fgets(line, 100, fp);
		else
			break;
		if (line[0] == '\n' || line[0] == '\0')
			break;

		ptr = strtok(line, " \n");
		printf("commmand = %s\n", ptr);
		if (strcmp(ptr, "resetCanvas") == 0) //重设画布
		{
			//重设画布应该要做的事：清空原来的所有图元，重新建立一个画布，然后铺满白色
			ptr = strtok(NULL, " \n");
			w = atoi(ptr);
			ptr = strtok(NULL, " \n");
			h = atoi(ptr);
			printf("width = %d, height = %d\n\n", w, h);

			if (hehe != nullptr)
				delete hehe;
			hehe = new QPixmap(w + 1, h + 1);
			ini_pixmap();

			//删除已有的图形列表
			del_units();
		}


		else if (strcmp(ptr, "setColor") == 0)
		{
			ptr = strtok(NULL, " \n");
			int R = atoi(ptr);
			ptr = strtok(NULL, " \n");
			int G = atoi(ptr);
			ptr = strtok(NULL, " \n");
			int B = atoi(ptr);
			printf("R = %d, G = %d, B = %d\n\n", R, G, B);

			pen.setColor(QColor(R, G, B));
		}


		else if (strcmp(ptr, "saveCanvas") == 0)
		{
			ptr = strtok(NULL, " \n");

			char tmp[50];
			strcpy(tmp, ptr);
			strcat(tmp, ".bmp");

			draw_to_pixmap();  //在保存时才真的画图
			hehe->save(tmp);
		}


		else if (strcmp(ptr, "drawLine") == 0)
		{
			int id, x1, y1, x2, y2;
			char alg[10];

			ptr = strtok(NULL, " \n");
			id = atoi(ptr);
			ptr = strtok(NULL, " \n");
			x1 = atoi(ptr);
			ptr = strtok(NULL, " \n");
			y1 = atoi(ptr);
			ptr = strtok(NULL, " \n");
			x2 = atoi(ptr);
			ptr = strtok(NULL, " \n");
			y2 = atoi(ptr);
			ptr = strtok(NULL, " \n");
			strcpy(alg, ptr);

			printf("algorithm = %s\n\n", alg);
			drawLine(id, x1, y1, x2, y2, alg);
		}


		else if (strcmp(ptr, "drawEllipse") == 0)
		{
			int id, x0, y0, a, b;
			ptr = strtok(NULL, " \n");
			id = atoi(ptr);
			ptr = strtok(NULL, " \n");
			x0 = atoi(ptr);
			ptr = strtok(NULL, " \n");
			y0 = atoi(ptr);
			ptr = strtok(NULL, " \n");
			a = atoi(ptr);
			ptr = strtok(NULL, " \n");
			b = atoi(ptr);

			drawEllipse(id, x0, y0, a, b);
		}


		else if (strcmp(ptr, "translate") == 0)
		{
			int id, dx, dy;
			ptr = strtok(NULL, " \n");
			id = atoi(ptr);
			ptr = strtok(NULL, " \n");
			dx = atoi(ptr);
			ptr = strtok(NULL, " \n");
			dy = atoi(ptr);

			printf("translate: dx = %d, dy = %d\n\n", dx, dy);
			translate_unit(id, dx, dy);
		}


		else if (strcmp(ptr, "rotate") == 0)
		{
			int id, x, y, r;
			ptr = strtok(NULL, " \n");
			id = atoi(ptr);
			ptr = strtok(NULL, " \n");
			x = atoi(ptr);
			ptr = strtok(NULL, " \n");
			y = atoi(ptr);
			ptr = strtok(NULL, " \n");
			r = atoi(ptr);

			printf("rotate: id = %d, x = %d, y = %d, r = %d\n\n", id, x, y, r);
			rotate_unit(id, x, y, r);
		}


		else if (strcmp(ptr, "scale") == 0)
		{
			int id, x, y;
			double s;
			ptr = strtok(NULL, " \n");
			id = atoi(ptr);
			ptr = strtok(NULL, " \n");
			x = atoi(ptr);
			ptr = strtok(NULL, " \n");
			y = atoi(ptr);
			ptr = strtok(NULL, " \n");
			s = atof(ptr);

			printf("scale: id = %d, x = %d, y = %d, s = %lf\n\n", id, x, y, s);
			scale_unit(id, x, y, s);
		}


		else if (strcmp(ptr, "drawPolygon") == 0)
		{
			int id, pnum, x, y;
			char alg[10];
			ptr = strtok(NULL, " \n");
			id = atoi(ptr);
			ptr = strtok(NULL, " \n");
			pnum = atoi(ptr);
			ptr = strtok(NULL, " \n");
			strcpy(alg, ptr);

			QPoint *dots = new QPoint[pnum];
			fgets(line, 100, fp);
			for (int i = 0; i < pnum; i++)
			{
				if (i == 0)
					ptr = strtok(line, " \n");
				else
					ptr = strtok(NULL, " \n");
				x = atoi(ptr);
				ptr = strtok(NULL, " \n");
				y = atoi(ptr);
				dots[i] = QPoint(x, y);
			}

			drawPolygon(id, pnum, dots, alg);
			delete[] dots;
		}


		else if (strcmp(ptr, "drawCurve") == 0)
		{
			int id, pnum, x, y;
			char alg[20];
			ptr = strtok(NULL, " \n");
			id = atoi(ptr);
			ptr = strtok(NULL, " \n");
			pnum = atoi(ptr);
			ptr = strtok(NULL, " \n");
			strcpy(alg, ptr);

			QPoint *dots = new QPoint[pnum];
			fgets(line, 100, fp);
			for (int i = 0; i < pnum; i++)
			{
				if (i == 0)
					ptr = strtok(line, " \n");
				else
					ptr = strtok(NULL, " \n");
				x = atoi(ptr);
				ptr = strtok(NULL, " \n");
				y = atoi(ptr);
				dots[i] = QPoint(x, y);
			}

			drawCurve(id, pnum, dots, alg);
			delete[] dots;
		}


		else if (strcmp(ptr, "clip") == 0)
		{
			int id, x1, y1, x2, y2;
			char alg[20];

			ptr = strtok(NULL, " \n");
			id = atoi(ptr);
			ptr = strtok(NULL, " \n");
			x1 = atoi(ptr);
			ptr = strtok(NULL, " \n");
			y1 = atoi(ptr);
			ptr = strtok(NULL, " \n");
			x2 = atoi(ptr);
			ptr = strtok(NULL, " \n");
			y2 = atoi(ptr);
			ptr = strtok(NULL, " \n");
			strcpy(alg, ptr);

			printf("clip: algorithm = %s\n\n", alg);
			clip_line(id, x1, y1, x2, y2, alg);
		}
	}


	fclose(fp);
	resize(w * WIND_CO, h * WIND_CO);
	show(); //最后等画完了才show()
}


void display::del_units()
{
	for (Item* it: item)
	{
		delete it;
	}

	item.clear();
}


void display::ini_pixmap()
{
	QPainter p(hehe);
	p.setBrush(QBrush(Qt::white));
	p.drawRect(-1, -1, w + 2, h + 2);
}


void display::draw_to_pixmap()
{
	ini_pixmap();
	QPainter p(hehe);
	for (Item *it: item)
	{
		it->draw(p);
	}
}


void display::drawLine(int id, int x1, int y1, int x2, int y2, char alg[])
{
	/*
	新建一个点列表;
	使用算法往点列表里添加点;
	往总的图元列表中添加当前的点列表;
	*/

	QList<QPoint>* line = new QList<QPoint>;

	if (strcmp(alg, "DDA") == 0) //DDA
		algo.line_DDA(line, x1, y1, x2, y2);
	else //Bresenham
	{
		algo.line_Bre(line, x1, y1, x2, y2);
		printf("\n\nin draw line Bre\n\n");
	}

	//添加到图元列表中
	Line* tmp = linefty.createItem(id, line, pen);
	tmp->set_sp(QPoint(x1, y1));
	tmp->set_ep(QPoint(x2, y2));
	item.append(tmp);
}


void display::drawEllipse(int id, int x0, int y0, int a, int b)
{
	//a:x轴， b:y轴
	QList<QPoint>* elli = new QList<QPoint>;
	algo.ellipse_mid(elli, x0, y0, a, b);

	Elli* tmp = ellifty.createItem(id, elli, pen);
	tmp->set_mid(QPoint(x0, y0));
	tmp->set_a(a);
	tmp->set_b(b);
	item.append(tmp);
}


void display::drawPolygon(int id, int pnum, QPoint *dots, char *alg)
{
	QList<QPoint>* line = new QList<QPoint>;
	if (strcmp(alg, "DDA") == 0) //DDA
	{
		for (int i = 0; i <= pnum - 1; i++)
			algo.line_DDA(line, dots[i].x(), dots[i].y(), dots[(i+1)%pnum].x(), dots[(i+1)%pnum].y());
	}

	else
	{
		for (int i = 0; i <= pnum - 1; i++)
			algo.line_Bre(line, dots[i].x(), dots[i].y(), dots[(i+1)%pnum].x(), dots[(i+1)%pnum].y());
	}

	Poly* tmp = polyfty.createItem(id, line, pen);
	tmp->set_nodelist(dots, pnum);
	item.append(tmp);
}


void display::drawCurve(int id, int pnum, QPoint *dots, char *alg)
{
	QList<QPoint>* pl = new QList<QPoint>;
	QList<QPoint>* ctrlp = new QList<QPoint>;
	for (int i = 0; i < pnum; ++i)
		ctrlp->append(dots[i]);
	if (strcmp(alg, "Bezier") == 0) //Bezier
		algo.Bezier(pl, pnum, ctrlp);
	else
		algo.Bspline(pl, pnum, ctrlp);

	Curve* tmp = curvefty.createItem(id, pl, pen);
	tmp->set_nodelist(ctrlp);
	if (strcmp(alg, "Bezier") == 0) //Bezier
		tmp->set_curvetype(0);
	else
		tmp->set_curvetype(1);
	item.append(tmp);
}


void display::translate_unit(int id, int dx, int dy)
{
	for (Item* it: item)
	{
		if (it->id == id)
		{
			it->translate(dx, dy);
			break;
		}
	}
}


void display::rotate_unit(int id, int x, int y, int r)
{
	for (Item* it: item)
	{
		if (it->id == id)
		{
			it->rotate(x, y, r);
			break;
		}
	}
}


void display::scale_unit(int id, int x, int y, double s)
{
	for (Item* it: item)
	{
		if (it->id == id)
		{
			it->scale(x, y, s);
			break;
		}
	}
}


void display::clip_line(int id, int x1, int y1, int x2, int y2, char *alg)
{
	bool todel = false;
	for (int i = 0; i < item.size(); ++i)
	{
		if (item[i]->id == id)
		{
			if (strcmp(alg, "Cohen-Sutherland") == 0) //cohen
				todel = static_cast<Line*>(item[i])->clip_cohen(x1, y1, x2, y2);
			else
				todel = static_cast<Line*>(item[i])->clip_liang(x1, y1, x2, y2);
			if (todel)
			{
				delete item[i];
				item.removeAt(i);
			}

			break;
		}
	}
}
