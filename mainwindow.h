#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define PAINT_X 330
#define PAINT_Y 106
#define BLOCK_SIZE 13

#include <QMainWindow>
#include <QTimer>
#include "snake.h"
#include <QMediaPlayer>
namespace Ui {
class Mainwindow;
}

class Mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit Mainwindow(QWidget *parent = nullptr);
    ~Mainwindow();

private slots:
    void on_startAction_triggered();

    void on_loadAction_triggered();

    void on_saveAction_triggered();

    void on_continueAction_triggered();

    void on_pauseAction_triggered();

    void on_restartAction_triggered();

    void on_quitAction_triggered();

    void on_helpAction_triggered();

    void on_setMapAction_triggered();

    void on_setDifficultyAction_triggered();

    void on_setMusicAction_triggered();

    void on_showGridAction_triggered();

public slots:
    void end_game();
    void timer_clock();

private:
    Ui::Mainwindow *ui;
    QTimer* timer;
    QMediaPlayer* player;
    Snake map;
    bool isBegin;
    int currentTime;
    int score;

protected:
    void keyPressEvent(QKeyEvent* e);
    void mousePressEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent* e);

};

#endif // MAINWINDOW_H
