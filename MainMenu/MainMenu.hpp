#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <optional> 
#include "../Themes/themes.h"
#include "../globalVar.hpp"

//inline - copiaza functia si o pune in main -> Timp de executie mai mic
inline void CenterText(sf::Text& text, const sf::RectangleShape& rect) {
    sf::FloatRect textBounds = text.getLocalBounds();

    // Pune originea in mijlocul textului, in loc de stanga sus(default)
    text.setOrigin({
        textBounds.position.x + textBounds.size.x / 2.0f,
        textBounds.position.y + textBounds.size.y / 2.0f
        });
    //Seteaza originea in mijlocul dreptunghiului
    text.setPosition({
        rect.getPosition().x + rect.getSize().x / 2.0f,
        rect.getPosition().y + rect.getSize().y / 2.0f
        });
}

struct GameMenu { // struct - alternativa pentru class
    sf::Font font;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> texts;
    std::vector<std::string> labels;

    sf::Texture TablaTexture;
    std::optional<sf::Sprite> TablaSprite;

    // Cursors 
    std::optional<sf::Cursor> cursorHand;
    std::optional<sf::Cursor> cursorArrow;

    GameMenu() { // Constructor
        if (!font.openFromFile("Themes/Kanit-Medium.ttf")) {
            std::cerr << "Error loading font" << std::endl;
        }
        if (!TablaTexture.loadFromFile("MainMenu/Tabla.png")) {
            std::cerr << "Error loading Image" << std::endl;
        }
        else {
            TablaSprite.emplace(TablaTexture);

            sf::Vector2u imgSize = TablaTexture.getSize();

            TablaSprite->setOrigin({ imgSize.x / 2.0f, imgSize.y / 2.0f });

            TablaSprite->setScale({ 1.2f, 1.2f });

            float screenWidth = 1920.f;
            float screenHeight = 1080.f;
            float buttonSpace = 543.f;

            // Calculam pozitia X (Orizontala)
            float finalX = buttonSpace + (screenWidth - buttonSpace) / 2.0f;
            // Calculam pozitia Y (Verticala) - Mijlocul ecranului
            float finalY = screenHeight / 2.0f;

            // Aplicam pozitia
            TablaSprite->setPosition({ finalX, finalY });
        }

        // Cursorul 
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
            rect.setPosition({ 0.f, btnHeight * static_cast<float>(i) });
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

    void draw(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

        bool isHovering = false;

        // Actualizeaza culorile cand tinem cursorul pe buton
        for (size_t i = 0; i < buttons.size(); ++i) {
            if (labels[i] == "") continue;

            if (buttons[i].getGlobalBounds().contains(mousePosF)) {
                buttons[i].setFillColor(color_HoverButton);
                isHovering = true;
            }
            else {
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

        // Desenam tot
        // MODIFICARE: Verificam daca exista sprite-ul si folosim *
        if (TablaSprite.has_value()) {
            window.draw(*TablaSprite);
        }
        for (const auto& rect : buttons) window.draw(rect);
        for (const auto& txt : texts) window.draw(txt);
    }

    void handleInput(const sf::Event& event, sf::RenderWindow& window) {
        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {

                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

                for (size_t i = 0; i < buttons.size(); ++i) {
                    if (buttons[i].getGlobalBounds().contains(mousePosF)) {
                        if (labels[i] == "Play") {
                            interfata = 2;
                        }
                        else if (labels[i] == "Learn") {
                            interfata = 3;
                        }
                        else if (labels[i] == "Stats") {
                            interfata = 4;
                        }
                        else if (labels[i] == "Leaderboard") {
                            interfata = 5;
                        }
                        else if (labels[i] == "Profiles") {
                            interfata = 6;
                        }
                        else if (labels[i] == "Themes") {
                            interfata = 7;
                        }
                        else if (labels[i] == "Settings") {
                            interfata = 8;
                        }
                        else if (labels[i] == "Exit") {
                            window.close();
                        }
                    }
                }
            }
        }
    }
};