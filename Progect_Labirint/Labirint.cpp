#include "Labirint.h"
#include <cstdlib>
#include <ctime>

using namespace std;

Labirint::Labirint(int rows, int cols, int numItems) : rows(rows), cols(cols) {
    grid.resize(rows, vector<char>(cols, '.'));
    generateWalls();
    placeEntranceAndExit();
    placeItems(numItems);
}

void Labirint::generateWalls() {
    for (int i = 0; i < rows; ++i) {
        grid[i][0] = '#';
        grid[i][cols - 1] = '#';
    }
    for (int j = 0; j < cols; ++j) {
        grid[0][j] = '#';
        grid[rows - 1][j] = '#';
    }
    int wallCount = 2 * (rows + cols);
    srand(time(nullptr));
    for (int i = 0; i < wallCount; ++i) {
        int x = rand() % (rows - 2) + 1;
        int y = rand() % (cols - 2) + 1;
        grid[x][y] = '#';
    }
}

void Labirint::placeEntranceAndExit() {
    grid[0][1] = 'U'; // Entrance
    grid[rows - 1][cols - 2] = 'I'; // Exit
}

void Labirint::placeItems(int numItems) {
    srand(time(nullptr));
    for (int i = 0; i < numItems; ++i) {
        int x, y;
        do {
            x = rand() % rows;
            y = rand() % cols;
        } while (grid[x][y] != '.');
        grid[x][y] = 'P';
    }
}

void Labirint::setCell(int x, int y, char value) {
    grid[x][y] = value;
}

char Labirint::getCell(int x, int y) const {
    return grid[x][y];
}

bool Labirint::isWall(int x, int y) const {
    return grid[x][y] == '#';
}

int Labirint::getRows() const {
    return rows;
}

int Labirint::getCols() const {
    return cols;
}

std::pair<int, int> Labirint::getExit() const {
    return {rows - 1, cols - 2};
}

int Labirint::getEntranceColumn() const {
    return 1;
}

void Labirint::print() const {
    for (const auto& row : grid) {
        for (char cell : row) {
            cout << cell;
        }
        cout << endl;
    }
}
