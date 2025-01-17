//
// Created by Виктория Аванесова on 20.12.2024.
//

#ifndef PROGECT_LABIRINT_ROBOT_H
#define PROGECT_LABIRINT_ROBOT_H

#include "Character.h"


/*
 * Класс Robot наследует от класса Character и реализует движение робота в лабиринте.
 * Он переопределяет метод move для обеспечения специфической логики перемещения,
 * связанной с роботом в контексте лабиринта.
 */

class Robot : public Character {
public:

    //Конструктор класса Robot.
    //Инициализирует робота в начальной позиции (x, y) в лабиринте.
    Robot(int x, int y);


    void move(int deltaX, int deltaY) override;
    /*
    * Переопределяет метод move для робота.
    * Позволяет роботу перемещаться на заданное количество клеток по осям x и y.
    *
    * Аргументы:
    * - deltaX: изменение позиции по оси x.
    * - deltaY: изменение позиции по оси y.
    */
};


#endif //PROGECT_LABIRINT_ROBOT_H
