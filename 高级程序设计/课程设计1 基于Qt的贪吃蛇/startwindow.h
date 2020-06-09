#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>

namespace Ui {
class StartWindow;
}

class StartWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit StartWindow(QWidget *parent = nullptr);
	~StartWindow();

signals:
	void easymode();
	void moderatemode();
	void hardmode();

private slots:
	void on_btnSimple_clicked(bool checked);

	void on_btnModerate_clicked(bool checked);

	void on_btnHard_clicked(bool checked);

private:
	Ui::StartWindow *ui;
};

#endif // STARTWINDOW_H
