#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <iostream>



using namespace std;

Game::Game(int rows, int cols, int numItems)
        : labirint(rows, cols, numItems),
          robot(1, labirint.getEntranceColumn()), // Робот стартует под входом
          minotaur(0, 0), // Минотавр временно инициализируется, позиция позже обновится
          shieldTurns(0),
          hammerTurns(0),
          swordActive(false)
{
    // Инициализация положения Минотавра
    srand(time(nullptr));
    do {
        int randomX = rand() % rows;
        int randomY = rand() % cols;
        if (!labirint.isWall(randomX, randomY)) {
            minotaur.setPosition(randomX, randomY);
            break;
        }
    } while (true);
}

void Game::startGame() {
    // Запрашиваем параметры у пользователя
    int rows, cols, numItems;
    cout << "Enter the number of rows for the labirint: ";
    cin >> rows;
    cout << "Enter the number of columns for the labirint: ";
    cin >> cols;
    cout << "Enter the number of items in the labirint: ";
    cin >> numItems;

    // Проверяем корректность введенных данных
    if (rows < 15 || cols < 15) {
        cout << "Dimensions must be at least 15x15. Exiting.\n";
        return;
    }
    if (numItems < 3) {
        cout << "Number of items must be at least 3. Exiting.\n";
        return;
    }

    // Создаем новый лабиринт с указанными параметрами
    labirint = Labirint(rows, cols, numItems);

    // Инициализируем позицию робота и Минотавра
    robotX = 1; // Робот всегда под входом
    robotY = labirint.getEntranceColumn();
    labirint.setCell(robotX, robotY, 'R'); // Помещаем робота

    do {
        minotaurX = rand() % rows;
        minotaurY = rand() % cols;
    } while (labirint.isWall(minotaurX, minotaurY) || (minotaurX == robotX && minotaurY == robotY));
    labirint.setCell(minotaurX, minotaurY, 'M'); // Помещаем Минотавра

    // Основной игровой цикл
    char input;
    while (true) {
        system("cls");

       // system("clear");
        labirint.print(); // Печать лабиринта
        cout << "Enter command (W/A/S/D to move, Q to quit): ";
        cin >> input;

        if (input == 'Q' || input == 'q') {
            cout << "Exiting the game. Goodbye!\n";
            break;
        }

        processInput(input);
        updateGameState();

        if (checkCollision()) {
            if (swordActive) {
                cout << "You used the sword to destroy the Minotaur!\n";
                exit(0); // Завершаем игру победой
            } else if (shieldTurns > 0) {
                cout << "The Minotaur attacked, but your shield protected you!\n";
                shieldTurns--;
            } else {
                cout << "Game over! The Minotaur caught the robot.\n";
                break;
            }
        }

        checkWinCondition();
    }
}




void Game::processInput(char input) {
    switch (input) {
        case 'W': case 'w':
            moveRobot('W');
            break;
        case 'A': case 'a':
            moveRobot('A');
            break;
        case 'S': case 's':
            moveRobot('S');
            break;
        case 'D': case 'd':
            moveRobot('D');
            break;
        default:
            cout << "Invalid input! Use W/A/S/D for movement.\n";
    }
}

void Game::moveRobot(char direction) {
    int newX = robotX;
    int newY = robotY;

    switch (direction) {
        case 'W': newX--; break;
        case 'A': newY--; break;
        case 'S': newX++; break;
        case 'D': newY++; break;
    }

    if (!labirint.isWall(newX, newY) || hammerTurns > 0) {
        if (labirint.isWall(newX, newY)) {
            cout << "You used the hammer to break through the wall!\n";
            hammerTurns--;
        }
        labirint.setCell(robotX, robotY, '.'); // Убираем старую позицию
        robotX = newX;
        robotY = newY;
        labirint.setCell(robotX, robotY, 'R'); // Обновляем новую позицию
        handleItem(newX, newY);
    } else {
        cout << "Cannot move into a wall!\n";
    }
}

void Game::moveMinotaur() {
    int direction = rand() % 4; // 0: up, 1: down, 2: left, 3: right
    int newX = minotaurX;
    int newY = minotaurY;

    switch (direction) {
        case 0: newX--; break;
        case 1: newX++; break;
        case 2: newY--; break;
        case 3: newY++; break;
    }

    if (!labirint.isWall(newX, newY)) {
        minotaurX = newX;
        minotaurY = newY;
    }
}

bool Game::checkCollision() {
    return (robotX == minotaurX && robotY == minotaurY);
}

void Game::handleItem(int x, int y) {
    char item = labirint.getCell(x, y);
    if (item == 'P') {
        int effect = rand() % 4; // Random effect: 0 - sword, 1 - shield, 2 - hammer, 3 - fog
        switch (effect) {
            case 0:
                cout << "You found a sword! You can destroy the Minotaur if you collide with it.\n";
                swordActive = true;
                break;
            case 1:
                cout << "You found a shield! You are protected for 3 turns.\n";
                shieldTurns = 3;
                break;
            case 2:
                cout << "You found a hammer! You can break through walls for 3 turns.\n";
                hammerTurns = 3;
                break;
            case 3:
                cout << "You found a fog item! Your visibility is reduced.\n";
                // Implement fog of war logic here
                break;
        }
        labirint.setCell(x, y, '.');
    }
}

void Game::checkWinCondition() {
    if (robotX == labirint.getExit().first && robotY == labirint.getExit().second) {
        cout << "Congratulations! You reached the exit!\n";
        exit(0);
    }
}

void Game::updateGameState() {
    // Убираем старые позиции
    labirint.setCell(robotX, robotY, '.');
    labirint.setCell(minotaurX, minotaurY, '.');

    // Двигаем Минотавра
    moveMinotaur();

    // Устанавливаем новые позиции
    labirint.setCell(robotX, robotY, 'R');
    labirint.setCell(minotaurX, minotaurY, 'M');

    // Уменьшаем длительность эффектов
    if (shieldTurns > 0) shieldTurns--;
    if (hammerTurns > 0) hammerTurns--;
}


//вывод в окно


/*
void Game::render(sf::RenderWindow &window) {
    window.clear(sf::Color::Black);

    const float cellSize = 20.f; // Размер одной ячейки
    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));

    for (int i = 0; i < labirint.getRows(); ++i) {
        for (int j = 0; j < labirint.getCols(); ++j) {
            char value = labirint.getCell(i, j);

            // Устанавливаем цвет ячейки в зависимости от её содержимого
            if (value == '#') {
                cell.setFillColor(sf::Color::White); // Стена
            } else if (value == 'R') {
                cell.setFillColor(sf::Color::Green); // Робот
            } else if (value == 'M') {
                cell.setFillColor(sf::Color::Red); // Минотавр
            } else if (value == 'P') {
                cell.setFillColor(sf::Color::Yellow); // Предмет
            } else {
                cell.setFillColor(sf::Color::Black); // Проход
            }

            cell.setPosition(j * cellSize, i * cellSize);
            window.draw(cell);
        }
    }

    window.display();
}
 */

