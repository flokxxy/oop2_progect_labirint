#ifndef MINOTAUR_H
#define MINOTAUR_H

#include "Character.h"

class Minotaur : public Character {
public:
    Minotaur(int x, int y);
    void move(int deltaX, int deltaY) override;
};

#endif // MINOTAUR_H
