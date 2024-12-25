//
// Created by Виктория Аванесова on 19.12.2024.
//

#ifndef PROGECT_LABIRINT_GAME_H
#define PROGECT_LABIRINT_GAME_H
#include <SFML/Graphics.hpp>

#include "Labirint.h"
#include "Robot.h"
#include "Minotaur.h"


class Game {
private:
    Labirint labirint;
    Robot robot;
    Minotaur minotaur;


    int robotX, robotY; // Robot's position
    int minotaurX, minotaurY; // Minotaur's position

    int shieldTurns = 0; // Turns remaining for shield effect
    int hammerTurns = 0; // Turns remaining for hammer effect
    bool swordActive = false;


    void moveRobot(char direction);
    void moveMinotaur();
    bool checkCollision();
    void handleItem(int x, int y); // Handle interaction with items
    void checkWinCondition(); // Check if robot has won

public:
    Game(int rows, int cols, int numItems);
    void startGame();
    void processInput(char input);
    void updateGameState();


    //вывод в отдельно окно
    //void render(sf::RenderWindow &window);

};


#endif //PROGECT_LABIRINT_GAME_H
