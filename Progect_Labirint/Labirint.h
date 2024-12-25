//
// Created by Виктория Аванесова on 19.12.2024.
//

#ifndef PROGECT_LABIRINT_LABIRINT_H
#define PROGECT_LABIRINT_LABIRINT_H
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Labirint {
private:
    int rows, cols;
    std::vector<std::vector<char>> grid;
    std::pair<int, int> entrance; // Entrance coordinates
    std::pair<int, int> exit;     // Exit coordinates

    void generateWalls();
    void placeEntranceAndExit();
    void placeItems(int numItems);

public:
    Labirint(int rows, int cols, int numItems);
    void printMaze() const;
    bool isWall(int x, int y) const;
    char getCell(int x, int y) const;
    void setCell(int x, int y, char value);
    int getEntranceColumn() const;

    int getCols() const;
    int getRows() const;

    pair<int, int> getExit() const;

    void print() const;

};


#endif //PROGECT_LABIRINT_LABIRINT_H
