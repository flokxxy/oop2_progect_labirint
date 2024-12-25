#include "Robot.h"

Robot::Robot(int x, int y) : Character(x, y) {}

void Robot::move(int deltaX, int deltaY) {
    posX += deltaX;
    posY += deltaY;
}
