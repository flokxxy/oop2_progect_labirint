//
// Created by Виктория Аванесова on 20.12.2024.
//

#ifndef PROGECT_LABIRINT_ROBOT_H
#define PROGECT_LABIRINT_ROBOT_H

#include "Character.h"


class Robot : public Character {
public:
    Robot(int x, int y);
    void move(int deltaX, int deltaY) override;
};


#endif //PROGECT_LABIRINT_ROBOT_H
