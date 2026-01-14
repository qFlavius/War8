#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "../Themes/themesRead.h"
#include "../globalVar.hpp"

inline std::string ColorToHex(sf::Color color) {
    std::stringstream ss;
    ss << "#" << std::hex << std::uppercase << std::setfill('0');

    ss << std::setw(2) << (int)color.r;
    ss << std::setw(2) << (int)color.g;
    ss << std::setw(2) << (int)color.b;

    return ss.str();
}

inline void LoadPlayerData() {
    std::ifstream fin("Profile/playerData.txt");

    if (!fin.is_open()) return;

    std::string key, value;
    while (fin >> key >> value) {
        for (auto& c : key) c = tolower(c);

        if (key == "theme:") {
            themeAdress[0] = value;
        }
        else if (key == "fps:") {
            fps = value;
        }
        else if (key == "music:") {
            Music = value;
        }
        else if (key == "soundeffects:") {
            soundEffects = value;
        }
        else if (key == "language:") {
            language = value;
        }
    }
    fin.close();
    std::cout << "[INFO] Player data loaded: " << themeAdress[0] << std::endl;
}

inline void SavePlayerData() {
    std::ofstream foutTheme("Themes/Custom/custom.txt");

    if (foutTheme.is_open()) {
        foutTheme << "Background: " << ColorToHex(themes[4].color_bg) << std::endl;
        foutTheme << "Buttons: " << ColorToHex(themes[4].color_buttons) << std::endl;
        foutTheme << "Hover-Buttons: " << ColorToHex(themes[4].color_HoverButton) << std::endl;
        foutTheme << "Text: " << ColorToHex(themes[4].color_text) << std::endl;
        foutTheme << "BoardSquare1: " << ColorToHex(themes[4].color_BoardSquare1) << std::endl;
        foutTheme << "BoardSquare2: " << ColorToHex(themes[4].color_BoardSquare2) << std::endl;
        foutTheme << "Peace1: " << ColorToHex(themes[4].color_peace1) << std::endl;
        foutTheme << "PeaceOutline1: " << ColorToHex(themes[4].color_peaceOutline1) << std::endl;
        foutTheme << "Peace2: " << ColorToHex(themes[4].color_peace2) << std::endl;
        foutTheme << "PeaceOutline2: " << ColorToHex(themes[4].color_peaceOutline2) << std::endl;

        foutTheme.close();
    }
    else {
        std::cerr << "Error opening Themes/Custom/custom.txt" << std::endl;
    }

    std::ofstream foutProfile("Profile/playerData.txt");

    if (!foutProfile.is_open()) {
        std::cerr << "Error saving Profile/playerData.txt" << std::endl;
        return;
    }

    foutProfile << "theme: " << themeAdress[0] << std::endl;
    foutProfile << "fps: " << fps << std::endl;
    foutProfile << "music: " << Music << std::endl;
    foutProfile << "soundeffects: " << soundEffects << std::endl;
    foutProfile << "language: " << language << std::endl;

    foutProfile.close();

    std::cout << "[INFO] Profile saved: " << themeAdress[0] << std::endl;
}