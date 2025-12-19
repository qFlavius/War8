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
#include <iostream>

#include "globalVar.hpp"

#include "Themes/themesRead.h"
#include "Themes/Themes.hpp"
#include "MainMenu/MainMenu.hpp"
#include "PlayScreen/PlayScreen.hpp"
#include "Profile/playerData.hpp"
#include "Stats/stats.hpp"
#include "Learn/learn.hpp"
#include "LeaderBoard/leaderboard.hpp"
#include "Profiles/profiles.hpp"
#include "Settings/settings.hpp"

// variabila globala folosita in tot proiectul
int interfata = 1;

int main()
{
    LoadPlayerData();
    Themes(); // initializeaza culorile (color_bg, etc.)
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Razboi in 8");
    window.setFramerateLimit(60);

    GameMenu menu;
    GameThemes themePage;
    StatsMenu stats;
    SettingsMenu settings;
    ProfilesMenu profiles;
    LeaderboardMenu leaderboard;
    LearnMenu learn;

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
            else if (interfata == 3) {
                learn.handleInput(*event, window);
            }
            else if (interfata == 4) {
                stats.handleInput(*event, window);
            }
            else if (interfata == 5) {
                leaderboard.handleInput(*event, window);
            }
            else if (interfata == 6) {
                profiles.handleInput(*event, window);
            }
            else if (interfata == 7) {
                themePage.handleInput(*event, window);
            }
            else if (interfata == 8) {
                settings.handleInput(*event, window);
            }
        }

        // daca tocmai am intrat in Play, reseteaza jocul
        if (interfata == 2 && lastInterfata != 2) {
            Play_Reset();
        }
        lastInterfata = interfata;

        window.clear(themes[0].color_bg);

        if (interfata == 1) {
            menu.draw(window);
        }
        else if (interfata == 2) {
            Play_Draw(window);
        }
        else if (interfata == 3) {
            learn.draw(window);
        }
        else if (interfata == 4) {
            stats.draw(window);
        }
        else if (interfata == 5) {
            leaderboard.draw(window);
        }
        else if (interfata == 6) {
            profiles.draw(window);
        }
        else if (interfata == 7) {
            themePage.draw(window);
        }
        else if (interfata == 8) {
            settings.draw(window);
        }

        window.display();
    }

    return 0;
}