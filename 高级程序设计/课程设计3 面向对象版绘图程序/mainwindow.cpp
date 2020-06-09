#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	win = new display;

	connect(this, SIGNAL(start_file_drawing()), win, SLOT(draw()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_btn_File_clicked(bool checked)
{
	/*第一阶段：用户按下该按钮后：应该做：
	 发射相关信号，让win读取文件开始画图.

	 实例化的工作应该在MainWindow类的构造函数中就做，而不是这时候才实例化
	*/
	hide();
	emit start_file_drawing();
}


void MainWindow::on_btn_Mouse_clicked(bool checked)
{
	hide();
	drawwin = new DrawWindow;
	drawwin->show();
}
