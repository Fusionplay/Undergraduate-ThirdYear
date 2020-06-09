#include "end_window.h"
#include "ui_end_window.h"

end_window::end_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::end_window)
{
    ui->setupUi(this);
}

end_window::~end_window()
{
	delete ui;
}


void end_window::show_window(int score)
{
	//设置比分
	QString str = QString("游戏结束! 您的得分是：%1。").arg(score);
	ui->txtlabel->setText(str);
	show();
}

void end_window::on_btnRestart_clicked(bool checked)
{
	emit newgame();
}


