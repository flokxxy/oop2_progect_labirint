#ifndef LABIRINT_H
#define LABIRINT_H

#include <vector>
#include <string>
#include <utility>

class Labirint {
private:
    int rows, cols, numItems;
    std::vector<std::vector<char>> grid;

    void generate(); // Основной метод генерации
    void generateWithEller(); // Генерация алгоритмом Эллера
    void createSemicircle(unsigned int centerRow, unsigned int centerCol,
                          unsigned int radius, bool topHalf);
    bool isPathExists(unsigned int sr, unsigned int sc, unsigned int er, unsigned int ec); // Проверка пути


public:

    void placeItems(int numItems); // Метод для размещения предметов
    int getRows() const { return rows; }
    int getCols() const { return cols; }

    Labirint(int rows, int cols, int numItems);
    void print() const;
    char getCell(int x, int y) const;
    void setCell(int x, int y, char value);

    int getEntranceColumn() const; // Возвращает колонку входа
    bool isWall(int x, int y) const; // Проверяет, является ли клетка стеной
    std::pair<int, int> getExit() const; // Возвращает координаты выхода


    bool hasPathToExit() const; // Проверяет, существует ли путь от входа к выходу
    bool isExit(int x, int y) const; // Проверяет, является ли клетка выходом
    bool checkRobotAtExit(int robotX, int robotY) const; // Проверяет, находится ли робот на выходе



};

#endif // LABIRINT_H
