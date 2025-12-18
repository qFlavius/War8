#include "themesRead.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream> 
#include <cstdint> // Pentru std::uint8_t

std::string themeAdress[5];
Theme themes[5];

// Convert hex string "#RRGGBB" to sf::Color
sf::Color HexToColor(std::string hex) {
    if (hex.empty()) return sf::Color::White;
    if (hex[0] == '#') hex.erase(0, 1);
    
    unsigned int value = 0;
    try {
        value = std::stoul(hex, nullptr, 16);
    } catch (...) {
        return sf::Color::White; 
    }

    std::uint8_t r = (value >> 16) & 0xFF;
    std::uint8_t g = (value >> 8) & 0xFF;
    std::uint8_t b = value & 0xFF;

    return sf::Color(r, g, b);
}

void Themes() {
    if (themeAdress[0].empty()) {
        themeAdress[0] = "Themes/Default/Default.txt";
    }
    std::string key, value;
    themeAdress[1] = "Themes/Default/Default.txt";
    themeAdress[2] = "Themes/Default_Dark/defaultDark.txt";
    themeAdress[3] = "Themes/B&W/B&W.txt";
    themeAdress[4] = "Themes/Custom/custom.txt";
    for (int i = 0; i <= 4; i++) {
        std::ifstream fin(themeAdress[i]);
        while (fin >> key >> value) {
            for (auto& c : key) c = tolower(c);

            if (key == "background:") {
                themes[i].color_bg = HexToColor(value);
            }
            else if (key == "buttons:") {
                themes[i].color_buttons = HexToColor(value);
            }
            else if (key == "hover-buttons:") {
                themes[i].color_HoverButton = HexToColor(value);
            }
            else if (key == "text:") {
                themes[i].color_text = HexToColor(value);
            }
            else if (key == "boardsquare1:") {
                themes[i].color_BoardSquare1 = HexToColor(value);
            }
            else if (key == "boardsquare2:") {
                themes[i].color_BoardSquare2 = HexToColor(value);
            }
            else if (key == "peace1:") {
                themes[i].color_peace1 = HexToColor(value);
            }
            else if (key == "peaceoutline1:") {
                themes[i].color_peaceOutline1 = HexToColor(value);
            }
            else if (key == "peace2:") {
                themes[i].color_peace2 = HexToColor(value);
            }
            else if (key == "peaceoutline2:") {
                themes[i].color_peaceOutline2 = HexToColor(value);
            }
        }
        fin.close();
    }
}