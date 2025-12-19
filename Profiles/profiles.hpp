#pragma once
#include <SFML/Graphics.hpp>
#include <optional> 
#include "../Themes/themesRead.h"
#include "../globalVar.hpp"
#include "../SoundEffects/SoundEffects.hpp"

struct ProfilesMenu {
    AudioMenu audio;

	sf::Font font;
	sf::RectangleShape back;
	sf::Text backTxt;

	std::optional<sf::Cursor> cursorHand;
	std::optional<sf::Cursor> cursorArrow;

	ProfilesMenu() : backTxt(font) {
		if (!font.openFromFile("Themes/Kanit-Medium.ttf")) {
			std::cerr << "Error loading font" << std::endl;
		}
        back.setSize(sf::Vector2f(543.f, 108.f));
        back.setPosition(sf::Vector2f(0, 1080));
        back.setOrigin({ 0, 108.f });
        back.setFillColor(themes[0].color_buttons);

        backTxt.setFillColor(themes[0].color_text);
        backTxt.setString("Back");
        backTxt.setCharacterSize(35);

        sf::FloatRect textRect = backTxt.getLocalBounds();
        backTxt.setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });

        sf::FloatRect btnBounds = back.getGlobalBounds();

        backTxt.setPosition({
            btnBounds.position.x + btnBounds.size.x / 2.0f,
            btnBounds.position.y + btnBounds.size.y / 2.0f
            });

	}

    void handleInput(const sf::Event& event, sf::RenderWindow& window) {
        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {

                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

                if (back.getGlobalBounds().contains(mousePosF)) {
                    audio.playClick();
                    interfata = 1;
                }
            }

        }
    }

	void draw(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

        bool isHovering = false;
        if (back.getGlobalBounds().contains(mousePosF)) {
            back.setFillColor(themes[0].color_HoverButton);
            isHovering = true;
        }
        else {
            back.setFillColor(themes[0].color_buttons);
        }
        if (isHovering && cursorHand.has_value()) {
            window.setMouseCursor(*cursorHand);
        }
        else if (cursorArrow.has_value()) {
            window.setMouseCursor(*cursorArrow);
        }

        window.draw(back);
        window.draw(backTxt);
	}
};