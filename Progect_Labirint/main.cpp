#include <iostream>
#include "Game.h"



using namespace std;

int main() {
    cout << "Welcome to the Maze Game!\n";

    // Создаем объект игры с временными значениями
    Game game(15, 15, 3); // Стартовые значения, будут переопределены в startGame()

    // Запуск игры с запросом параметров у пользователя
    game.startGame();


    // Создаем окно
    /* sf::RenderWindow window(sf::VideoMode(800, 600), "Labirint Game");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Рендерим лабиринт
        game.render(window);
    }
     */

    return 0;
}
