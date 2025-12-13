#include <SFML/Graphics.hpp>
#include "Themes/themes.h"
#include "MainMenu/MainMenu.hpp"

int main()
{
    Themes(); // Initializeaza culorile

    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Razboi in 8");
    window.setFramerateLimit(60);

    // Meniul Principal
    GameMenu menu;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(color_bg);

        // Meniul Principal
        menu.draw(window);

        window.display();
    }

    return 0;
}