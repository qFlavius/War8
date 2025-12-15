#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <optional>
#include "../Themes/themesRead.h"
#include "../globalVar.hpp"


struct GameThemes {
	sf::Font font;
	sf::RectangleShape back;
	std::vector<sf::Text> textButtons, textTitles;
	std::vector<std::string> labels;

	std::optional<sf::Cursor> cursorHand;
	std::optional<sf::Cursor> cursorArrow;

	GameThemes() {
		if (!font.openFromFile("Themes/Kanit-Medium.ttf")) {
			std::cerr << "Error loading font" << std::endl;
		}
		cursorHand = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
		cursorArrow = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);

	}
};