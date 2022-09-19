#include "snake.h"
#include <QTime>


#include <QtDebug>

Snake::Snake()
{
    clear_map();
    init_map();
}

void Snake::init_map()
{
    snake.enqueue(QPoint(SIZE/2 + 1, SIZE/2));
    snake.enqueue(QPoint(SIZE/2, SIZE/2));

    map[SIZE/2][SIZE/2] = HEAD;
    map[SIZE/2 + 1][SIZE/2] = BODY;
}

void Snake::clear_map()
{
    snake.clear();
    area.clear();
    extension = 0;
    direction = LEFT;
    isMoved = true;

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            map[i][j] = PATH;
}

void Snake::random_map()
{
    qsrand(QTime::currentTime().msec());
    int line_num = qrand() % 4 + 4;
    for (int i = 0; i< line_num; i++){
        int x = qrand() % (SIZE/4 * 3);
        int y = qrand() % (SIZE/4 * 3);
        int len = qrand() % (SIZE/2) + (SIZE/4);
        bool dir = qrand() % 2;
        if (dir)
            while (y < SIZE && len > 0){
                if (map[x][y] == PATH)
                    map[x][y] = WALL;
                y++;
                len--;
            }
        else
            while (x < SIZE && len > 0){
                if (map[x][y] == PATH)
                    map[x][y] = WALL;
                x++;
                len--;
            }
    }
    for (int i = SIZE/2 - 4; i< SIZE/2 + 4; i++)
        for (int j = SIZE/2 -4; j < SIZE/2 +4; j++)
            if (map[i][j] == WALL)
                map[i][j] = PATH;
    load_wall();
}

bool Snake::change_direction(int dir)
{
    if (!isMoved)
        return false;
    if ((dir == RIGHT || dir == LEFT) == (direction == UP || direction == DOWN)){
        direction = dir;
        isMoved = false;
        return true;
    }
    else
        return false;
}

bool Snake::move_forward(int& score)
{
    isMoved = true;
    QPoint thisPoint = snake.last();
    QPoint nextPoint;
    switch (direction) {
    case RIGHT:
        nextPoint = QPoint(thisPoint.x() + 1, thisPoint.y());
        break;
    case LEFT:
        nextPoint = QPoint(thisPoint.x() - 1, thisPoint.y());
        break;
    case UP:
        nextPoint = QPoint(thisPoint.x(), thisPoint.y() + 1);
        break;
    case DOWN:
        nextPoint = QPoint(thisPoint.x(), thisPoint.y() - 1);
        break;
    }
    if (nextPoint.x() >= SIZE || nextPoint.x() < 0 || nextPoint.y() >= SIZE || nextPoint.y() < 0)
        return false;
    switch (map[nextPoint.x()][nextPoint.y()]) {
    case PATH:
    {
        area.removeAt(area.indexOf(nextPoint));
        snake.enqueue(nextPoint);
        map[nextPoint.x()][nextPoint.y()] = HEAD;
        map[thisPoint.x()][thisPoint.y()] = BODY;
        break;
    }
    case FOOD:
    {
        score ++;
        int idx = area.indexOf(nextPoint);
        if (idx != -1)
            area.removeAt(idx);
        snake.enqueue(nextPoint);
        map[nextPoint.x()][nextPoint.y()] = HEAD;
        map[thisPoint.x()][thisPoint.y()] = BODY;

        extension += 3;
        int rnum = qrand() % area.size();
        map[area[rnum].x()][area[rnum].y()] = FOOD;
        break;
    }
    default:
        return false;
    }
    if (extension == 0){
        QPoint lastPoint = snake.dequeue();
        area.append(lastPoint);
        map[lastPoint.x()][lastPoint.y()] = PATH;
    }
    else
        extension--;
    return true;

}

bool Snake::set_wallorpath(int x, int y)
{
    if (map[x][y] == WALL){
        map[x][y] = PATH;
        return true;
    }
    else if (map[x][y] == PATH) {
        map[x][y] = WALL;
        return true;
    }
    else
        return false;
}

int Snake::get_status(int x, int y)
{
    return map[x][y];
}

void Snake::load_wall()
{
    for (int i =0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (map[i][j] == PATH)
                area.append(QPoint(i,j));
    qsrand(QTime::currentTime().msec());
    int rnum = qrand() % area.size();
    map[area[rnum].x()][area[rnum].y()] = FOOD;

}

void Snake::load(QTextStream& fin)
{
    int snake_size, area_size;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            fin >> map[i][j];
    fin >> snake_size;
    for (int i = 0; i <snake_size; i++){
        int x, y;
        fin >> x >> y;
        snake.enqueue(QPoint(x,y));
    }
    fin >> area_size;
    for (int i = 0; i < area_size; i++){
        int x, y;
        fin >> x >> y;
        area.append(QPoint(x,y));
    }
    fin >> direction;
}

void Snake::save(QTextStream& fout)
{
    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++)
            fout << map[i][j] << ' ';
        fout << endl;
    }
    fout << snake.size() << endl;
    for (auto i = snake.begin(); i != snake.end(); i++)
        fout << i->x() << ' ' << i->y() << endl;
    fout << area.size() << endl;
    for (auto i = area.begin(); i != area.end(); i++)
        fout << i->x() << ' ' << i->y() << endl;
    fout << direction <<endl;
}

