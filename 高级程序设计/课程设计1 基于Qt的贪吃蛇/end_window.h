#ifndef END_WINDOW_H
#define END_WINDOW_H

#include <QMainWindow>

namespace Ui {
class end_window;
}

class end_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit end_window(QWidget *parent = nullptr);
    ~end_window();

//public slots:
	void show_window(int score);  //这个槽显示结束窗口界面

private slots:
	void on_btnRestart_clicked(bool checked);

private:
    Ui::end_window *ui;

signals:
	void newgame();
};

#endif // END_WINDOW_H
