#include <SFML/Graphics.hpp>
#include "Themes/themes.h"
#include "MainMenu/MainMenu.hpp"
#include "globalVar.hpp"

int interfata = 1;

int main()
{
    Themes(); // Initializeaza culorile

    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Razboi in 8");
    window.setFramerateLimit(60);

    GameMenu menu; // Meniul Principal

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (interfata == 1) {
                menu.handleInput(*event, window);
            }

        }

        window.clear(color_bg);

        if (interfata == 1) {
            menu.draw(window);
        }

        window.display();
    }

    return 0;
}