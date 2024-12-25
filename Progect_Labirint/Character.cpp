#include "Character.h"

Character::Character(int x, int y) : posX(x), posY(y) {}

int Character::getX() const {
    return posX;
}

int Character::getY() const {
    return posY;
}

void Character::setPosition(int x, int y) {
    posX = x;
    posY = y;
}
