#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

#include <SFML/Graphics.hpp>

using namespace std;

Game::Game(int rows, int cols, int numItems)
        : labirint(rows, cols, numItems),
          robot(1, labirint.getEntranceColumn()), // Робот стартует под входом
          minotaur(0, 0), // Минотавр временно инициализируется
          shieldTurns(0),
          hammerTurns(0),
          swordTurns(0),
          fogTurns(0),
          swordActive(false) {
    auto [exitRow, exitCol] = labirint.getExit();
    exitX = exitRow;
    exitY = exitCol;

    /*
    if (exitX != -1 && exitY != -1) {
        std::cout << "Exit initialized at: (" << exitX << ", " << exitY << ")\n"; // Диагностика
    } else {
        std::cerr << "Failed to initialize exit coordinates.\n"; // Диагностика
    }
     */

    // Инициализация Минотавра
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

        auto [exitRow, exitCol] = labirint.getExit();
        exitX = exitRow;
        exitY = exitCol;

        if (exitX != -1 && exitY != -1) {
            std::cout << "Exit initialized at: (" << exitX << ", " << exitY << ")\n"; // Диагностика
        } else {
            std::cerr << "Failed to initialize exit coordinates.\n"; // Диагностика
        }

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
            system("clear");
            updateGameState();
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
                    cout << "\033[31mGame over! The Minotaur caught the robot. \033[0m \n";
                    break;
                }
            }

            checkWinCondition();
        }
    }

    void Game::processInput(char input) {
        switch (input) {
            case 'W':
            case 'w':
                moveRobot('W');
                break;
            case 'A':
            case 'a':
                moveRobot('A');
                break;
            case 'S':
            case 's':
                moveRobot('S');
                break;
            case 'D':
            case 'd':
                moveRobot('D');
                break;
            default:
                std::cout << "Invalid input! Use W/A/S/D for movement.\n";
        }

        std::cout << "Robot position: (" << robotX << ", " << robotY << ")\n"; // Диагностика

        // Проверка победы
        checkWinCondition();
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
            std::cout << "\033[33mYou used the hammer to break through the wall!\033[0m\n";
            hammerTurns--;
        }
        labirint.setCell(robotX, robotY, '.'); // Убираем старую позицию
        robotX = newX;
        robotY = newY;
        char cell = labirint.getCell(robotX, robotY);

        if (cell == 'P') {
            std::cout << "Robot stepped on an item.\n"; // Отладочный вывод
            handleItem(robotX, robotY);
        }

        labirint.setCell(robotX, robotY, 'R'); // Обновляем новую позицию
    } else {
        std::cout << "\033[31mCannot move into a wall!\033[0m\n";
    }
}


void Game::moveMinotaur() {
        int direction = rand() % 4; // 0: up, 1: down, 2: left, 3: right
        int newX = minotaurX;
        int newY = minotaurY;

        switch (direction) {
            case 0:
                newX--;
                break;
            case 1:
                newX++;
                break;
            case 2:
                newY--;
                break;
            case 3:
                newY++;
                break;
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
        int effect = rand() % 4; // Случайный выбор эффекта: 0 - меч, 1 - щит, 2 - молот, 3 - туман
        switch (effect) {
            case 0: // Меч
                std::cout << "\033[33mYou found a sword! You can destroy the Minotaur if you collide with it.\033[0m\n";
                swordActive = true;
                swordTurns = 3;
                break;
            case 1: // Щит
                std::cout << "\033[34mYou found a shield! You are protected from the Minotaur for 3 turns.\033[0m\n";
                shieldTurns = 3;
                break;
            case 2: // Молот
                std::cout << "\033[35mYou found a hammer! You can break through walls for 3 turns.\033[0m\n";
                hammerTurns = 3;
                break;
            case 3: // Туман войны
                std::cout << "\033[36mYou found the Fog of War! Your visibility is reduced to a 3x3 grid for 3 turns.\033[0m\n";
                fogTurns = 3;
                break;
        }
        labirint.setCell(x, y, '.'); // Удаляем предмет с поля
    }
}




void Game::checkWinCondition() {
    /*
    std::cout << "Checking win condition...\n"; // Диагностика
    std::cout << "Robot position: (" << robotX << ", " << robotY << ")\n"; // Диагностика
    std::cout << "Exit position: (" << exitX << ", " << exitY << ")\n"; // Диагностика
     */
    if (robotX == exitX && robotY == exitY) {
        std::cout << "\033[32mCongratulations! You successfully escaped the labyrinth!\033[0m\n";
        exit(0); // Завершаем игру
    }
}


    void Game::updateGameState() {

        checkWinCondition();
        // Убираем старые позиции
        labirint.setCell(robotX, robotY, '.');
        labirint.setCell(minotaurX, minotaurY, '.');

        // Двигаем Минотавра
        moveMinotaur();

        // Устанавливаем новые позиции
        labirint.setCell(robotX, robotY, 'R');
        labirint.setCell(minotaurX, minotaurY, 'M');

        // Проверка: находится ли робот на выходе
        if (labirint.checkRobotAtExit(robotX, robotY)) {
            std::cout << "Congratulations! You successfully escaped the labyrinth!\n";
            exit(0); // Завершаем игру
        }

        // Уменьшаем длительность эффектов
        if (swordTurns > 0) swordTurns--;
        if (shieldTurns > 0) shieldTurns--;
        if (hammerTurns > 0) hammerTurns--;
        if (fogTurns > 0) fogTurns--;
        // Логика отображения тумана войны (если требуется)
        if (fogTurns > 0) {
            std::cout << "\033[35mFog of war is active! Your visibility is limited.\033[0m\n";
            displayFog();
        }
    }


void Game::displayFog() {
    int fogRadius = 1; // Радиус тумана
    for (int i = robotX - fogRadius; i <= robotX + fogRadius; ++i) {
        for (int j = robotY - fogRadius; j <= robotY + fogRadius; ++j) {
            if (i >= 0 && i < labirint.getRows() && j >= 0 && j < labirint.getCols()) {
                std::cout << labirint.getCell(i, j);
            } else {
                std::cout << '#'; // За границей лабиринта
            }
        }
        std::cout << "\n";
    }
}


void Game::renderLabirint() {
    if (fogTurns > 0) {
        std::cout << "\033[36mFog of War active! Visibility reduced.\033[0m\n";
        for (int i = robotX - 1; i <= robotX + 1; i++) {
            for (int j = robotY - 1; j <= robotY + 1; j++) {
                if (i >= 0 && i < labirint.getRows() && j >= 0 && j < labirint.getCols()) {
                    std::cout << labirint.getCell(i, j);
                } else {
                    std::cout << '#'; // Граница видимости
                }
            }
            std::cout << '\n';
        }
    } else {
        labirint.print();
    }
}


