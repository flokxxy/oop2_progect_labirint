#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

#include <termios.h>
#include <unistd.h>

using namespace std;

std::vector<std::string> activeEffects; // для удобства отображения активных предметов
std::vector<std::string> errorMessages; // для удобства отображения сообщений
std::vector<std::string> gameMessages;

//ф-ия для того чтобы двигаться без enter
char getCharInput() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);          // Получаем текущие настройки терминала
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);        // Отключаем канонический ввод и эхо
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Устанавливаем новые настройки
    ch = getchar();                          // Считываем символ
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Восстанавливаем старые настройки
    return ch;
}


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
            cout << "Enter command (W/A/S/D to move, Q to quit): ";
            //cin >> input;

            input = getCharInput(); // подключение ходьбы без

            if (input == 'Q' || input == 'q') {
                cout << "Exiting the game. Goodbye!\n";
                break;
            }

            processInput(input);
            //updateGameState();


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
            gameMessages.push_back ("\033[33mYou used the hammer to break through the wall!\033[0m\n");
            hammerTurns--;
        }
        labirint.setCell(robotX, robotY, '.'); // Убираем старую позицию
        robotX = newX;
        robotY = newY;
        char cell = labirint.getCell(robotX, robotY);

        if (cell == 'P') {
            handleItem(robotX, robotY);
        }

        labirint.setCell(robotX, robotY, 'R'); // Обновляем новую позицию
    } else {
        errorMessages.push_back("\033[31m[ERROR] You hit a wall?????!\033[0m");
    }
}

void Game::moveMinotaur() {
    int dx = robotX - minotaurX;  // Разница по X
    int dy = robotY - minotaurY;  // Разница по Y
    int absDx = abs(dx);
    int absDy = abs(dy);

    // Если робот в радиусе 3 клеток, идем к нему
    if (absDx <= 3 && absDy <= 3) {
        if (absDx > absDy) { // Двигаемся в сторону, где больше разница
            if (dx > 0 && !labirint.isWall(minotaurX + 1, minotaurY)) {
                minotaurX++;
            } else if (dx < 0 && !labirint.isWall(minotaurX - 1, minotaurY)) {
                minotaurX--;
            }
        } else {
            if (dy > 0 && !labirint.isWall(minotaurX, minotaurY + 1)) {
                minotaurY++;
            } else if (dy < 0 && !labirint.isWall(minotaurX, minotaurY - 1)) {
                minotaurY--;
            }
        }
    }
        //Если робот далеко, ходим случайно
    else {
        int direction = rand() % 4;
        int newX = minotaurX, newY = minotaurY;

        switch (direction) {
            case 0: newX--; break; // Вверх
            case 1: newX++; break; // Вниз
            case 2: newY--; break; // Влево
            case 3: newY++; break; // Вправо
        }

        if (!labirint.isWall(newX, newY)) {
            minotaurX = newX;
            minotaurY = newY;
        }
    }
}

bool Game::checkCollision() {
    if (robotX == minotaurX && robotY == minotaurY) {
        if (shieldTurns > 0) {
            gameMessages.push_back("\033[34m[SHIELD] The Minotaur attacked, but your shield protected you!\033[0m");
            shieldTurns--;
            return false;
        } else if (swordActive) {
            gameMessages.push_back("\033[33m[VICTORY] You used the sword to destroy the Minotaur!\033[0m\n");

            // Вывод сообщения перед завершением игры
            std::cout << "\n\033[33m[VICTORY] You used the sword to destroy the Minotaur!\033[0m\n";
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Даем 2 секунды на отображение
            exit(0);
        } else {
            gameMessages.push_back("\033[31m[DEATH] Game over! The Minotaur caught you.\033[0m");

            // Вывод сообщения перед завершением игры
            std::cout << "\n\033[31m[DEATH] Game over! The Minotaur caught you.\033[0m\n";
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Даем 2 секунды на отображение
            exit(0);
        }
    }
    return false;
}


void Game::checkWinCondition() {
    /*
    std::cout << "Checking win condition...\n"; // Диагностика
    std::cout << "Robot position: (" << robotX << ", " << robotY << ")\n"; // Диагностика
    std::cout << "Exit position: (" << exitX << ", " << exitY << ")\n"; // Диагностика
     */
    if (robotX == exitX && robotY == exitY) {
        gameMessages.push_back("\033[32m[GAME OVER] Congratulations! You successfully escaped the labyrinth!\033[0m");
        exit(0);
    }
}

//присутствует таймер действия предметов
void Game::handleItem(int x, int y) {
    char item = labirint.getCell(x, y);
    if (item == 'P') {
        int effect = rand() % 4;
        switch (effect) {
            case 0: // Меч
                swordActive = true;
                swordTurns = 3;
                activeEffects.push_back("\033[33m[Sword] You can destroy the Minotaur! (" + std::to_string(swordTurns) + " turns left)\033[0m");
                break;
            case 1: // Щит
                shieldTurns = 3;
                activeEffects.push_back("\033[34m[Shield] You are protected from Minotaur! (" + std::to_string(shieldTurns) + " turns left)\033[0m");
                break;
            case 2: // Молот
                hammerTurns = 3;
                activeEffects.push_back("\033[35m[Hammer] You can break walls! (" + std::to_string(hammerTurns) + " turns left)\033[0m");
                break;
            case 3: // Туман
                fogTurns = 3;
                activeEffects.push_back("\033[36m[Fog] Visibility reduced to 3x3! (" + std::to_string(fogTurns) + " turns left)\033[0m");
                break;
        }
        labirint.setCell(x, y, '.'); // Удаляем предмет с карты
    }
}



void Game::updateGameState() {
    labirint.setCell(robotX, robotY, '.');
    labirint.setCell(minotaurX, minotaurY, '.');

    moveMinotaur();

    labirint.setCell(robotX, robotY, 'R');
    labirint.setCell(minotaurX, minotaurY, 'M');

    if (swordTurns > 0) swordTurns--;
    if (shieldTurns > 0) shieldTurns--;
    if (hammerTurns > 0) hammerTurns--;
    if (fogTurns > 0) {
        displayFog();
        fogTurns--;
    } else {
        labirint.print();
    }

    // Проверка: находится ли робот на выходе
    if (labirint.checkRobotAtExit(robotX, robotY)) {
        gameMessages.push_back("\033[33m[VICTORY]Congratulations! You successfully escaped the labyrinth!\033[0m");
        exit(0); // Завершаем игру
    }

    if (checkCollision()) {
        gameMessages.push_back("\033[31m[DEATH] The Minotaur caught you! You have been killed!\033[0m");
        return; // Если игра окончена, дальше ничего не делаем
    }

    // Вывод заголовка сообщений
    cout << "\n\033[32m===  MESSAGES  ===\033[0m\n";

    if (gameMessages.empty()) {
        std::cout << "No messages.\n";
    } else {
        for (const auto& msg : gameMessages) {
            std::cout << msg << "\n";
        }
    }
    gameMessages.clear();

    // Обновляем таймер эффектов и удаляем истекшие
    for (auto it = activeEffects.begin(); it != activeEffects.end(); ) {
        if (it->find("[Sword]") != std::string::npos && --swordTurns <= 0) {
            it = activeEffects.erase(it);
        } else if (it->find("[Shield]") != std::string::npos && --shieldTurns <= 0) {
            it = activeEffects.erase(it);
        } else if (it->find("[Hammer]") != std::string::npos && --hammerTurns <= 0) {
            it = activeEffects.erase(it);
        } else if (it->find("[Fog]") != std::string::npos && --fogTurns <= 0) {
            it = activeEffects.erase(it);
        } else {
            ++it;
        }
    }

    // Выводим все активные эффекты
    std::cout << "\n\033[32m=== ACTIVE EFFECTS ===\033[0m\n";
    if (activeEffects.empty()) {
        std::cout << "No active effects.\n";
    } else {
        for (const auto& effect : activeEffects) {
            std::cout << effect << "\n";
        }
    }

    // Выводим ошибки
    if (!errorMessages.empty()) {
        std::cout << "\n\033[31m=== ERROR MESSAGES ===\033[0m\n";
        for (const auto& error : errorMessages) {
            std::cout << error << "\n";
        }
        errorMessages.clear(); // Очищаем ошибки перед следующим шагом
    }
}

void Game::displayFog() {
    int fogRadius = 1;

    for (int i = 0; i < labirint.getRows(); ++i) {
        for (int j = 0; j < labirint.getCols(); ++j) {
            if (i == 0 || i == labirint.getRows() - 1 || j == 0 || j == labirint.getCols() - 1) {
                std::cout << "#";
            }
            else if (std::abs(i - robotX) <= fogRadius && std::abs(j - robotY) <= fogRadius) {
                char cell = labirint.getCell(i, j);

                if (cell == 'R') {
                    std::cout << "\033[32mR\033[0m"; // Зеленый робот
                } else if (cell == 'M') {
                    std::cout << "\033[31mM\033[0m"; // Красный минотавр
                } else if (cell == 'P') {
                    std::cout << "\033[34mP\033[0m"; // Синий предмет
                } else {
                    std::cout << cell;
                }
            }
                else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
}

