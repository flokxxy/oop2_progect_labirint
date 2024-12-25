//
// Created by Виктория Аванесова on 20.12.2024.
//

#ifndef PROGECT_LABIRINT_CHARACTER_H
#define PROGECT_LABIRINT_CHARACTER_H


class Character {
protected:
    int posX, posY;

public:
    Character(int x, int y);
    virtual ~Character() = default;

    int getX() const;
    int getY() const;
    void setPosition(int x, int y);
    virtual void move(int deltaX, int deltaY) = 0; // Pure virtual method
};


#endif //PROGECT_LABIRINT_CHARACTER_H
