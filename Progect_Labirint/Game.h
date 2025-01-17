//
// Created by Виктория Аванесова on 19.12.2024.
//

// Этот заголовочный файл определяет класс Game, который управляет состоянием игры,
// включая лабиринт, робота и минотавра. Он обрабатывает ввод пользователя,
// обновляет состояние игры и отображает элементы игры.


#ifndef PROGECT_LABIRINT_GAME_H
#define PROGECT_LABIRINT_GAME_H
#include <SFML/Graphics.hpp>

#include "Labirint.h"
#include "Robot.h"
#include "Minotaur.h"


class Game {
private:
    Labirint labirint; // The labyrinth where the game takes place
    Robot robot; // The player's robot
    Minotaur minotaur; // The enemy minotaur


    int robotX, robotY; // Robot's position
    int minotaurX, minotaurY; // Minotaur's position
    int exitX, exitY; // Exit's position

    // Items and their durations
    bool swordActive = false; // Indicates if the sword is active
    int swordTurns = 0; // Remaining turns for the sword
    int shieldTurns = 0; // Remaining turns for the shield
    int hammerTurns = 0; // Remaining turns for the hammer
    int fogTurns = 0; // Remaining turns for the fog of war

    // Function to display fog in the labyrinth
    void displayFog();

    // Moves the robot in the specified direction
    // Character representing the direction (W/A/S/D)
    void moveRobot(char direction);

    // Moves the minotaur randomly within the labyrinth
    void moveMinotaur();

    // Checks for collisions between the robot and the minotaur
    // True if a collision occurs, false otherwise
    bool checkCollision();

    // Handles interaction with items at the specified coordinates
    // @param x: X-coordinate of the item
    // @param y: Y-coordinate of the item
    void handleItem(int x, int y);

    // Checks if the robot has met the winning condition
    bool checkWinCondition();

public:
    // Constructor to initialize the game with specified parameters
    // rows: Number of rows in the labyrinth
    // cols: Number of columns in the labyrinth
    // numItems: Number of items to place in the labyrinth
    Game(int rows, int cols, int numItems);

    // Starts the game loop and handles user input
    void startGame();

    // Processes user input for movement and actions
    // input: Character input from the user
    void processInput(char input);

    // Updates the game state, including positions and item effects
    void updateGameState();

    void endGame(bool won);

};


#endif //PROGECT_LABIRINT_GAME_H
