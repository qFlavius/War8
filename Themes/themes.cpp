#include "themes.h"
#include <fstream>
#include <iostream>
#include <sstream> 
#include <cstdint> // Pentru std::uint8_t

sf::Color color_buttons;
sf::Color color_bg;
sf::Color color_HoverButton;
sf::Color color_text;

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
    std::ifstream fin("Themes/Default/default.txt");
    if (!fin.is_open()) return;

    std::string key, value;
    
    while (fin >> key >> value) {
        for (auto &c : key) c = tolower(c);

        if (key == "background:") {
            color_bg = HexToColor(value);
        }
        else if (key == "buttons:") {
            color_buttons = HexToColor(value);
        }
        else if (key == "hover-buttons:") {
            color_HoverButton = HexToColor(value);
        }
        else if (key == "text:") {
            color_text = HexToColor(value);
        }
    }
    fin.close();
}