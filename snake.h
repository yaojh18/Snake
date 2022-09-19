#ifndef SNAKE_H
#define SNAKE_H

#define SIZE 40

#define PATH 0
#define BODY 1
#define WALL 2
#define FOOD 3
#define HEAD 4

#define UP 10
#define DOWN 11
#define LEFT 12
#define RIGHT 13

#include <QList>
#include <QQueue>
#include <QTextStream>

class Snake
{
    int map[SIZE][SIZE];

    QQueue<QPoint> snake;
    QList<QPoint> area;

    int direction;
    int extension;
    bool isMoved;
public:
    Snake();
    bool move_forward(int& score);
    bool change_direction(int dir);
    bool set_wallorpath(int x, int y);
    int get_status(int x, int y);
    void init_map();
    void clear_map();
    void random_map();
    void load_wall();
    void save(QTextStream& out);
    void load(QTextStream& in);
};

#endif // SNAKE_H
