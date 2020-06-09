#include "startwindow.h"
#include "ui_startwindow.h"

StartWindow::StartWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::StartWindow)
{
	ui->setupUi(this);
}

StartWindow::~StartWindow()
{
	delete ui;
}

void StartWindow::on_btnSimple_clicked(bool checked)
{
	emit easymode();
}

void StartWindow::on_btnModerate_clicked(bool checked)
{
	emit moderatemode();
}

void StartWindow::on_btnHard_clicked(bool checked)
{
	emit hardmode();
}
