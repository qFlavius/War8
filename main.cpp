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
#include "AI/AIPlayer.hpp"

// variabila globala folosita in tot proiectul
int interfata = 1;

int main()
{
    LoadPlayerData();
    Themes(); // initializeaza culorile (color_bg, etc.)

    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Razboi in 8");
    window.setFramerateLimit(60);

    // Initializeaza seed-ul pentru generatorul de numere pseudo-aleatoare al calculatorului.
    // Folosesc adrese (care de obicei difera intre rulari datorita ASLR) + dimensiunea ferestrei

    {
        std::uint32_t s = (std::uint32_t)(
            (std::uintptr_t)&window ^
            (std::uintptr_t)&interfata ^
            ((std::uint32_t)window.getSize().x << 16) ^
            (std::uint32_t)window.getSize().y
            );
        if (s == 0) s = 1;
        AI_Seed(s);
    }

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
