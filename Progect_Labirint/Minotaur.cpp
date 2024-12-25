#include "Minotaur.h"

Minotaur::Minotaur(int x, int y) : Character(x, y) {}

void Minotaur::move(int deltaX, int deltaY) {
    posX += deltaX;
    posY += deltaY;
}
