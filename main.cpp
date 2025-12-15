/*
#include <SFML/Graphics.hpp>
#include "Themes/themesRead.h"
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
*/

#include <SFML/Graphics.hpp>

#include "Themes/themesRead.h"
#include "MainMenu/MainMenu.hpp"
#include "PlayScreen/PlayScreen.hpp"
#include "globalVar.hpp"

// variabila globala folosita in tot proiectul
int interfata = 1;

int main()
{
    Themes(); // initializeaza culorile (color_bg, etc.)

    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Razboi in 8");
    window.setFramerateLimit(60);

    GameMenu menu;

    // initializeaza modulul de joc (Play)
    Play_Init();

    int lastInterfata = interfata;

    while (window.isOpen()) {

        while (auto event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (interfata == 1) {
                menu.handleInput(*event, window);
            }
            else if (interfata == 2) {
                Play_HandleEvent(*event, window);
            }
        }

        // daca tocmai am intrat in Play, reseteaza jocul
        if (interfata == 2 && lastInterfata != 2) {
            Play_Reset();
        }
        lastInterfata = interfata;

        window.clear(color_bg);

        if (interfata == 1) {
            menu.draw(window);
        }
        else if (interfata == 2) {
            Play_Draw(window);
        }

        window.display();
    }

    return 0;
}