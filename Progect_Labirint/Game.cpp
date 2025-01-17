#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include <thread>
#include <fstream>


#include <termios.h>
#include <unistd.h>

using namespace std;

/**
 * @file Game.h
 * @brief Заголовочный файл для класса Game, который управляет логикой игры.
 *
 * Этот файл содержит определение класса Game, который отвечает за инициализацию
 * лабиринта, управление состоянием игры и обработку пользовательского ввода.
 * Включает функции для движения робота и Минотавра, обработки столкновений, победы и других аспектов игры.
 *
 * @author Viktorija Avanesova
 * @date Последнее изменение: 17 января 2025 года
 */



std::vector<std::string> activeEffects; // для удобства отображения активных предметов
std::vector<std::string> errorMessages; // для удобства отображения ошибок
std::vector<std::string> gameMessages; // для удобства отображения сообщений

std::ostringstream gameHistory;


char getCharInput() {
    /**
 * @brief Получает символ ввода без необходимости нажимать Enter.
 *
 * Эта функция позволяет пользователю ввести один символ без необходимости
 * нажимать клавишу Enter. Используется для удобства обработки ввода.
 *
 * @return char Символ, введённый пользователем.
 *
 * @exception нет
 */

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



/**
 * @brief Конструктор класса Game.
 *
 * Инициализирует параметры игры, такие как лабиринт, позиции робота и Минотавра,
 * а также параметры активных предметов.
 *
 * @param rows Количество строк в лабиринте.
 * @param cols Количество столбцов в лабиринте.
 * @param numItems  Количество предметов в лабиринте.
 */
Game::Game(int rows, int cols, int numItems)
        : labirint(rows, cols, numItems),
          robot(1, labirint.getEntranceColumn()), // Робот инициализируется и стартует под входом
          minotaur(0, 0), // Минотавр инициализируется
          shieldTurns(0),
          hammerTurns(0),
          swordTurns(0),
          fogTurns(0),
          swordActive(false) {
    auto [exitRow, exitCol] = labirint.getExit(); // Устанавливаем позицию Минотавра
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
/**
 * @brief Запускает игровой цикл и обрабатывает ввод пользователя.
 *
 * Эта функция инициализирует лабиринт с указанными пользователем размерами,
 * запускает основной цикл игры, обрабатывает ввод пользователя и проверяет
 * условие победы. В случае достижения условий победы или поражения вызывает метод endGame.
 *
 * @exception нет
 */
        // Запрашиваем параметры у пользователя для создания лабиринта
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
            return; // Выход, если размеры некорректны
        }
        if (numItems < 3) {
            cout << "Number of items must be at least 3. Exiting.\n";
            return; // Выход, если количество предметов некорректно
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
            updateGameState(); // Обновляем состояние игры
            cout << "Enter command (W/A/S/D to move, Q to quit): ";
            //cin >> input; //ввод с использованием Enter

            input = getCharInput(); // Получаем ввод без необходимости нажимать Enter

            if (input == 'Q' || input == 'q') {
                cout << "Exiting the game. Goodbye!\n";
                break;
            }

            processInput(input); // Обрабатываем ввод пользователя
            //updateGameState();
            //checkWinCondition(); // Проверяем условие победы
            if (checkWinCondition()) {
                endGame(true); // Игрок победил
                break; // Завершаем игровой цикл
            } else if (checkCollision()) {
                endGame(false); // Игрок проиграл
                break; // Завершаем игровой цикл
            }
        }
}


void Game::processInput(char input) {
/**
 * @brief Обрабатывает ввод пользователя и перемещает робота.
 *
 * Эта функция принимает символ, представляющий направление, и перемещает робота
 * в указанное направление. Если ввод неверный, выводится сообщение об ошибке.
 *
 * @param input Символ, представляющий направление ('W', 'A', 'S', 'D').
 *
 * @exception нет
 */
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
    //checkWinCondition();


}

void Game::moveRobot(char direction) {
    /**
 * @brief Перемещает робота в указанном направлении.
 *
 * Эта функция обновляет позицию робота в лабиринте в зависимости от введённого
 * направления. Если новая позиция является стеной, проверяется, можно ли разрушить стену
 * с помощью молота.
 *
 * @param direction Направление движения робота ('W', 'A', 'S', 'D').
 *
 * @exception нет
 */
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

    /**
     *  @brief Перемещает Минотавра в зависимости от положения робота.
     * Если робот находится в радиусе 3 клеток, Минотавр движется к нему.
     * Если робот далеко, Минотавр перемещается случайным образом.
     * @exception нет
     */

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

    /**
 * @brief Проверяет столкновение робота с Минотавром.
 *
 * Эта функция проверяет, находится ли робот в одной клетке с Минотавром.
 * В случае столкновения выполняются различные действия в зависимости от состояния эффектов:
 * - Если у робота есть активированный щит, то он блокирует атаку Минотавра.
 * - Если у робота есть меч, то он побеждает Минотавра.
 * - В случае отсутствия щита и меча, Минотавр ловит робота, и игра заканчивается.
 *
 * @return bool Возвращает `false`, так как функция не требует возвращаемого значения для других операций.
 *
 * @exception При столкновении с Минотавром игра заканчивается, и программа завершает выполнение.
 */
    if (robotX == minotaurX && robotY == minotaurY) {
        if (shieldTurns > 0) { // Если у робота есть щит, он защищает от атаки Минотавра
            gameMessages.push_back("\033[34m[SHIELD] The Minotaur attacked, but your shield protected you!\033[0m");
            shieldTurns--;
            return false;
        } else if (swordActive) { // Если у робота есть меч, он побеждает Минотавра
            gameMessages.push_back("\033[33m[VICTORY] You used the sword to destroy the Minotaur!\033[0m\n");

            // Вывод сообщения перед завершением игры
            std::cout << "\n\033[33m[VICTORY] You used the sword to destroy the Minotaur!\033[0m\n";
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Даем 2 секунды на отображение
            return true;
        } else { // В случае, если у робота нет защиты и оружия, Минотавр ловит его, и игра заканчивается
            gameMessages.push_back("\033[31m[DEATH] Game over! The Minotaur caught you.\033[0m");
            // Вывод сообщения перед завершением игры
            std::cout << "\n\033[31m[DEATH] Game over! The Minotaur caught you.\033[0m\n";
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Даем 2 секунды на отображение
            //exit(0);
            return true;
        }
    }
    return false;
}


bool Game::checkWinCondition() {

    /**
 * @brief Проверяет условие победы.
 *
 * Если робот находится на выходе, игра считается выигранной.
 *
 * @exception нет
 */
    /*
    std::cout << "Checking win condition...\n"; // Диагностика
    std::cout << "Robot position: (" << robotX << ", " << robotY << ")\n"; // Диагностика
    std::cout << "Exit position: (" << exitX << ", " << exitY << ")\n"; // Диагностика
     */
    if (robotX == exitX && robotY == exitY) {
        gameMessages.push_back("\033[32m[GAME OVER] Congratulations! You successfully escaped the labyrinth!\033[0m");
        std::cout << "\n\033[32m[GAME OVER] Congratulations! You successfully escaped the labyrinth!\033[0m\n";
        std::this_thread::sleep_for(std::chrono::seconds(2)); // Даем 2 секунды на отображение
        return true;
    }
    return false;
}


void Game::handleItem(int x, int y) {
    /**
 * @brief Обрабатывает найденные предметы, активируя их эффекты.
 *
 * Если на текущей позиции робота находится предмет, эта функция активирует его,
 * выводится сообщение об активации предмета, а так же записывается в файл сохранения
 *
 * @param x Строка, в которой находится предмет.
 * @param y Столбец, в котором находится предмет.
 *
 * @exception нет
 */
    //присутствует таймер действия предметов
    char item = labirint.getCell(x, y);
    if (item == 'P') {
        int effect = rand() % 4;
        std::string message;
        switch (effect) {
            case 0: // Меч
                swordActive = true;
                swordTurns = 3;
                message = "[Sword] You can destroy the Minotaur! (" + std::to_string(swordTurns) + " turns left)";
                activeEffects.push_back("\033[33m[Sword] You can destroy the Minotaur! (" + std::to_string(swordTurns) + " turns left)\033[0m");
                break;
            case 1: // Щит
                shieldTurns = 3;
                message = "[Shield] You are protected from Minotaur! (" + std::to_string(shieldTurns) + " turns left)";
                activeEffects.push_back("\033[34m[Shield] You are protected from Minotaur! (" + std::to_string(shieldTurns) + " turns left)\033[0m");
                break;
            case 2: // Молот
                hammerTurns = 3;
                message = "[Hammer] You can break walls! (" + std::to_string(hammerTurns) + " turns left)";
                activeEffects.push_back("\033[35m[Hammer] You can break walls! (" + std::to_string(hammerTurns) + " turns left)\033[0m");
                break;
            case 3: // Туман
                fogTurns = 3;
                message = "[Fog] Visibility reduced to 3x3! (" + std::to_string(fogTurns) + " turns left)";
                activeEffects.push_back("\033[36m[Fog] Visibility reduced to 3x3! (" + std::to_string(fogTurns) + " turns left)\033[0m");
                break;
        }
        activeEffects.push_back(message);
        gameMessages.push_back(message);
        gameHistory << message << "\n"; // Добавляем сообщение в историю игры
        labirint.setCell(x, y, '.'); // Удаляем предмет с карты
    }
}


void Game::updateGameState() {


/**
 * @brief Обновляет состояние игры (например, статус предметов).
 *
 * Эта функция обновляет активные эффекты и другие параметры игры, отображая
 * соответствующие сообщения.
 *
 * @exception нет
 */

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

    // Добавляем текущее состояние лабиринта в gameHistory
    for (int i = 0; i < labirint.getRows(); ++i) {
        for (int j = 0; j < labirint.getCols(); ++j) {
            gameHistory << labirint.getCell(i, j);
        }
        gameHistory << '\n';
    }
    gameHistory << "\n---\n"; // Разделитель между состояниями



}

void Game::displayFog() {

    /**
 * @brief Отображает лабиринт с эффектом тумана.
 *
 * Эта функция выводит лабиринт, скрывая клетки, находящиеся за пределами
 * видимости робота, с учётом эффекта тумана. Видимость ограничена радиусом,
 * который определяется переменной `fogRadius`. Все клетки, находящиеся
 * в пределах радиуса видимости, отображаются, остальные — скрыты.
 * В пределах видимости могут быть показаны разные объекты, такие как робот,
 * минотавр и предметы, с соответствующими цветами для лучшего восприятия.
 *
 * @note Эта функция также учитывает границы лабиринта, которые всегда отображаются.
 *
 * @exception нет
 */

    int fogRadius = 1;

    for (int i = 0; i < labirint.getRows(); ++i) {
        for (int j = 0; j < labirint.getCols(); ++j) {
            // Отображаем границы лабиринта
            if (i == 0 || i == labirint.getRows() - 1 || j == 0 || j == labirint.getCols() - 1) {
                std::cout << "#"; // Отображение стены
            }
            // Отображаем клетки внутри тумана
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


void Game::endGame(bool won) {

    /**
 * @brief Завершает игру и сохраняет результаты в файл.
 *
 * Этот метод записывает окончательные результаты игры, включая все состояния лабиринта
 * во время игры, события, связанные с действиями игрока, и итоговый результат (победу или поражение).
 *
 * @param won Булевый флаг, указывающий на победу (true) или поражение (false) игрока.
 */
    std::ofstream file("../game_result.txt", std::ios::out); // Открываем файл в режиме записи

    if (file.is_open()) {

        // Записываем историю состояний лабиринта во время игры
        file << "Labyrinth States During the Game:\n";
        file << gameHistory.str();

        // Записываем сообщения о событиях игры
        file << "Game Events:\n" << gameHistory.str() << "\n";


        // Записываем результат игры
        file << "Result: " << (won ? "Victory" : "Defeat") << "\n\n";

        file.close(); // Закрываем файл после записи
        std::cout << "Game results saved to 'game_result.txt'.\n";
    } else {
        std::cerr << "Unable to open file.\n";
    }
}

