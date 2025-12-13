#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <optional> // Pentru std::optional
#include "../Themes/themes.h"

inline void CenterText(sf::Text &text, const sf::RectangleShape &rect) {
    sf::FloatRect textBounds = text.getLocalBounds();
    
    text.setOrigin({
        textBounds.position.x + textBounds.size.x / 2.0f,
        textBounds.position.y + textBounds.size.y / 2.0f
    });

    text.setPosition({
        rect.getPosition().x + rect.getSize().x / 2.0f,
        rect.getPosition().y + rect.getSize().y / 2.0f
    });
}

struct GameMenu {
    sf::Font font;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> texts;
    std::vector<std::string> labels;

    //Cursors (se foloseste std::optioinal pentru ca createFromSystem ar putea esua)
    std::optional<sf::Cursor> cursorHand;
    std::optional<sf::Cursor> cursorArrow;

    GameMenu() {
        if (!font.openFromFile("Themes/Kanit-Medium.ttf")) {
            std::cerr << "Error loading font" << std::endl;
        }
        
        // Cursorul (Cant tinem cursorul pe ceva acesta se va schimba)
        cursorHand = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
        cursorArrow = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);

        labels = {
            "Razboi in 8", "", "Play", "Learn", "Stats",
            "Leaderboard", "Profiles", "", "Themes", "Settings"
        };

        float btnWidth = 543.f;
        float btnHeight = 108.f;

        for (size_t i = 0; i < labels.size(); ++i) {
            sf::RectangleShape rect(sf::Vector2f(btnWidth, btnHeight));
            rect.setFillColor(color_buttons);
            rect.setPosition({0.f, btnHeight * static_cast<float>(i)});
            buttons.push_back(rect);

            if (!labels[i].empty()) {
                sf::Text txt(font);
                txt.setString(labels[i]);
                txt.setFillColor(sf::Color::Black);
                txt.setCharacterSize(i == 0 ? 36 : 28);
                
                CenterText(txt, buttons.back());
                texts.push_back(txt);
            }
        }
    }
    
    void draw(sf::RenderWindow &window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF = static_cast<sf::Vector2f>(mousePos);
        
        bool isHovering = false;

        // Actualizeaza culorile cand tinem cursorul pe buton
        for (size_t i = 0; i < buttons.size(); ++i) {
            if (labels[i] == "") continue;

            if (buttons[i].getGlobalBounds().contains(mousePosF)) {
                buttons[i].setFillColor(color_HoverButton);
                isHovering = true;
            } else {
                buttons[i].setFillColor(color_buttons);
            }
        }

        // Actualizam cursorul
        if (isHovering && cursorHand.has_value()) {
             window.setMouseCursor(*cursorHand);
        }
        else if (cursorArrow.has_value()) {
             window.setMouseCursor(*cursorArrow);
        }

        //  Desenam orice
        for (const auto &rect : buttons) window.draw(rect);
        for (const auto &txt : texts) window.draw(txt);
    }
};