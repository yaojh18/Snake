#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QPainter>

#include <QtDebug>

Mainwindow::Mainwindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Mainwindow)
{
    ui->setupUi(this);
    ui->startPauseButton->setDefaultAction(ui->startAction);
    ui->loadButton->setDefaultAction(ui->loadAction);
    ui->saveButton->setDefaultAction(ui->saveAction);
    ui->restartButton->setDefaultAction(ui->restartAction);
    ui->quitButton->setDefaultAction(ui->quitAction);

    setFocusPolicy(Qt::StrongFocus);

    timer = new QTimer(this);
    timer->setInterval(200);
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_clock()));

    player = new QMediaPlayer;
    player->setMedia(QUrl("qrc:/music/music/summer.mp3"));
    player->setVolume(50);

    isBegin = false;
    currentTime = 0;
    score = 0;
}

Mainwindow::~Mainwindow()
{
    delete ui;
    delete timer;
    delete player;
}

void Mainwindow::on_startAction_triggered()
{
    ui->startAction->setVisible(false);
    ui->loadAction->setEnabled(false);
    ui->pauseAction->setVisible(true);
    ui->startPauseButton->setDefaultAction(ui->pauseAction);
    ui->setDifficultyAction->setEnabled(false);
    ui->setMapAction->setEnabled(false);
    isBegin = true;
    map.load_wall();
    update();
}

void Mainwindow::on_loadAction_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "加载脚本文件", QDir::currentPath(), "文本文件(*.txt)");
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox mymessage(QMessageBox::Warning, "错误", "找不到文件！");
        mymessage.setWindowIcon(QIcon(":/icon/images/snake.png"));
        mymessage.addButton(("确定"), QMessageBox::YesRole);
        mymessage.exec();
        return;
    }
    else{
        ui->loadAction->setEnabled(false);
        ui->startAction->setVisible(false);
        ui->restartAction->setEnabled(false);
        ui->saveAction->setEnabled(true);
        ui->continueAction->setVisible(true);
        ui->restartAction->setEnabled(true);
        ui->startPauseButton->setDefaultAction(ui->continueAction);
        ui->setDifficultyAction->setEnabled(false);
        ui->setMapAction->setEnabled(false);
    }
    QTextStream fin(&file);
    map.clear_map();
    map.load(fin);
    fin >> currentTime;
    ui->timeLcd->display(currentTime);
    fin >> score;
    ui->scoreLcd->display(score);
    update();
    file.close();
}

void Mainwindow::on_saveAction_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,"保存脚本文件", QDir::currentPath(), "文本文件(*.txt)");
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox mymessage(QMessageBox::Warning, "错误", "无法在该路径下创建文件！");
        mymessage.setWindowIcon(QIcon(":/icon/images/snake.png"));
        mymessage.addButton(("确定"), QMessageBox::YesRole);
        mymessage.exec();
        return;
    }
    QTextStream fout(&file);
    map.save(fout);
    fout << currentTime << endl;
    fout << score << endl;
    file.close();

}

void Mainwindow::on_continueAction_triggered()
{
    ui->continueAction->setVisible(false);
    ui->restartAction->setEnabled(false);
    ui->saveAction->setEnabled(false);
    ui->pauseAction->setVisible(true);
    ui->startPauseButton->setDefaultAction(ui->pauseAction);
    isBegin = true;
}

void Mainwindow::on_pauseAction_triggered()
{
    ui->pauseAction->setVisible(false);
    ui->continueAction->setVisible(true);
    ui->restartAction->setEnabled(true);
    ui->saveAction->setEnabled(true);
    ui->startPauseButton->setDefaultAction(ui->continueAction);
    isBegin = false;
    timer->stop();
}

void Mainwindow::on_restartAction_triggered()
{
    if(ui->saveAction->isEnabled() && ui->continueAction->isEnabled()){
        ui->saveAction->setEnabled(false);
        ui->continueAction->setVisible(false);
    }
    ui->restartAction->setEnabled(false);
    ui->startAction->setVisible(true);
    ui->loadAction->setEnabled(true);
    ui->startPauseButton->setDefaultAction(ui->startAction);
    ui->setDifficultyAction->setEnabled(true);
    ui->setMapAction->setEnabled(true);
    isBegin = false;
    currentTime = 0;
    score = 0;
    ui->timeLcd->display(currentTime);
    ui->scoreLcd->display(score);
    map.clear_map();
    map.init_map();
    update();

}

void Mainwindow::on_quitAction_triggered()
{
    QMessageBox mymessage(QMessageBox::Information, "提示", "你确定要退出吗？");
    QPushButton *btnYes = mymessage.addButton(("是"), QMessageBox::YesRole);
    mymessage.addButton(("否"), QMessageBox::NoRole);
    mymessage.setWindowIcon(QIcon(":/icon/images/snake.png"));
    mymessage.exec();
    if ((QPushButton*)mymessage.clickedButton() == btnYes)
        qApp->quit();
}

void Mainwindow::end_game()
{
    isBegin = false;
    ui->pauseAction->setVisible(false);
    ui->restartAction->setEnabled(true);
    timer->stop();
}

void Mainwindow::timer_clock()
{
    currentTime++;
    int oldScore = score;
    ui->timeLcd->display(currentTime);
    if (map.move_forward(score)){
        if (oldScore != score)
            ui->scoreLcd->display(score);
        update();
    }
    else
        end_game();

}
void Mainwindow::keyPressEvent(QKeyEvent* e)
{
    if (isBegin){
        if (!timer->isActive())
            timer->start();
        switch (e->key()){
            case Qt::Key_Up:
                map.change_direction(DOWN);
            break;
            case Qt::Key_Down:
                map.change_direction(UP);
            break;
            case Qt::Key_Left:
                map.change_direction(LEFT);
            break;
            case Qt::Key_Right:
                map.change_direction(RIGHT);
            break;
        }
    }
    QMainWindow::keyPressEvent(e);
}
void Mainwindow::mousePressEvent(QMouseEvent *e)
{
    if (ui->startAction->isEnabled() && e->button() == Qt::LeftButton)
    {
        int x = (e->pos().x() - PAINT_X) / BLOCK_SIZE;
        int y = (e->pos().y() - PAINT_Y) / BLOCK_SIZE;
        if (x < SIZE && y < SIZE && x >= 0 && y >= 0)
            if (!map.set_wallorpath(x, y)){
                QMessageBox mymessage(QMessageBox::Information, "提示", "你不能在贪吃蛇上设置障碍！");
                mymessage.addButton(("确定"), QMessageBox::YesRole);
                mymessage.setWindowIcon(QIcon(":/icon/images/snake.png"));
                mymessage.exec();
            }
            else
                update();
    }
    QMainWindow::mousePressEvent(e);
}
void Mainwindow::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.translate(PAINT_X, PAINT_Y);
    if (ui->showGridAction->isChecked())
        painter.setPen(QColor(100,150,0,80));
    else
        painter.setPen(QColor(0,0,0,0));
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++){
            switch (map.get_status(i, j)) {
            case PATH:
                painter.setBrush(Qt::white);
                break;
            case WALL:
                painter.setBrush(Qt::darkGray);
                break;
            case BODY:
                painter.setBrush(Qt::lightGray);
                break;
            case HEAD:
                painter.setBrush(Qt::gray);
                break;
            default:
                painter.setBrush(Qt::red);
            }
            painter.drawRect(i*BLOCK_SIZE, j*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
        }

    QPainterPath frame;
    frame.moveTo(0,0);
    frame.lineTo(SIZE * BLOCK_SIZE, 0);
    frame.lineTo(SIZE * BLOCK_SIZE, SIZE * BLOCK_SIZE);
    frame.lineTo(0, SIZE * BLOCK_SIZE);
    frame.closeSubpath();
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::black,3,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));

    painter.drawPath(frame);
    QMainWindow::paintEvent(e);
}


void Mainwindow::on_helpAction_triggered()
{
    QFile file(":/helptxt/help/ReadMe.txt");
    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox mymessage(QMessageBox::Warning,"错误","找不到帮助文档！");
        mymessage.addButton(("确定"), QMessageBox::YesRole);
        mymessage.setWindowIcon(QIcon(":/icon/images/snake.png"));
        mymessage.exec();
    }
    else{
        QString txt = file.readAll();
        QMessageBox mymessage(QMessageBox::NoIcon,"帮助",txt);
        mymessage.addButton(("确定"), QMessageBox::YesRole);
        mymessage.setWindowIcon(QIcon(":/icon/images/snake.png"));
        mymessage.exec();
    }

}

void Mainwindow::on_setMapAction_triggered()
{
    ui->loadAction->setEnabled(false);
    ui->startAction->setVisible(false);
    ui->restartAction->setEnabled(false);
    ui->saveAction->setEnabled(true);
    ui->continueAction->setVisible(true);
    ui->restartAction->setEnabled(true);
    ui->startPauseButton->setDefaultAction(ui->continueAction);
    ui->setDifficultyAction->setEnabled(false);
    ui->setMapAction->setEnabled(false);
    map.clear_map();
    map.init_map();
    map.random_map();
    isBegin = true;
    update();
}

void Mainwindow::on_setDifficultyAction_triggered()
{
    if (ui->setDifficultyAction->isChecked()){
        timer->setInterval(100);
        ui->setDifficultyAction->setText("难度：困难");
    }
    else{
        timer->setInterval(200);
        ui->setDifficultyAction->setText("难度：简单");
    }
}

void Mainwindow::on_setMusicAction_triggered()
{
    if (ui->setMusicAction->isChecked()){
        player->play();
        ui->setMusicAction->setText("音乐：开");
    }
    else{
        player->stop();
        ui->setMusicAction->setText("音乐：关");
    }
}

void Mainwindow::on_showGridAction_triggered()
{
    if (ui->showGridAction->isChecked())
        ui->showGridAction->setText("栅格：显示");
    else
        ui->showGridAction->setText("栅格：不显示");
}
