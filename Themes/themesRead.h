#pragma once
#include <string>
#include <SFML/Graphics.hpp>

extern std::string themeAdress[5];

struct Theme {
	sf::Color color_buttons;
	sf::Color color_bg;
	sf::Color color_HoverButton;
	sf::Color color_text;
	sf::Color color_BoardSquare1;
	sf::Color color_BoardSquare2;
	sf::Color color_peace1;
	sf::Color color_peace2;
	sf::Color color_peaceOutline1;
	sf::Color color_peaceOutline2;
};
extern Theme themes[5];
void Themes();