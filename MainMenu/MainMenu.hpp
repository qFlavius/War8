#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <optional>
#include "../Themes/themesRead.h"
#include "../globalVar.hpp"
#include "../SoundEffects/SoundEffects.hpp"

struct GameMenu {
    AudioMenu audio;

    sf::Font font;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> texts;
    std::vector<std::string> labels;

    std::vector<sf::RectangleShape> boardSquares;
    std::vector<sf::CircleShape> boardPieces;

    std::optional<sf::Cursor> cursorHand;
    std::optional<sf::Cursor> cursorArrow;

    GameMenu() {
        if (!font.openFromFile("Themes/Inter_18pt-Bold.ttf")) {
            std::cerr << "Error loading font" << std::endl;
        }

        float screenWidth = 1920.f;
        float screenHeight = 1080.f;
        float buttonSpace = 543.f;

        float finalX = buttonSpace + (screenWidth - buttonSpace) / 2.0f;
        float finalY = screenHeight / 2.0f;

        float cellSize = 100.f;
        float boardSize = cellSize * 8.f;
        float startX = finalX - (boardSize / 2.0f);
        float startY = finalY - (boardSize / 2.0f);

        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                sf::RectangleShape square(sf::Vector2f(cellSize, cellSize));
                square.setPosition(sf::Vector2f(startX + j * cellSize, startY + i * cellSize));

                if ((i + j) % 2 != 0) {
                    if (i < 2) {
                        sf::CircleShape piece(cellSize * 0.29f);
                        piece.setOutlineThickness(cellSize * 0.1f);
                        piece.setOrigin(sf::Vector2f(piece.getRadius(), piece.getRadius()));
                        piece.setPosition(sf::Vector2f(startX + j * cellSize + cellSize / 2.f, startY + i * cellSize + cellSize / 2.f));
                        boardPieces.push_back(piece);
                    }
                    else if (i > 5) {
                        sf::CircleShape piece(cellSize * 0.29f);
                        piece.setOutlineThickness(cellSize * 0.1f);
                        piece.setOrigin(sf::Vector2f(piece.getRadius(), piece.getRadius()));
                        piece.setPosition(sf::Vector2f(startX + j * cellSize + cellSize / 2.f, startY + i * cellSize + cellSize / 2.f));
                        boardPieces.push_back(piece);
                    }
                }
                boardSquares.push_back(square);
            }
        }

        cursorHand = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
        cursorArrow = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);

        labels = {
            "Razboi in 8", "", "", "Play", "Learn", "Leaderboard",
            "", "", "Themes", "Settings"
        };

        float btnWidth = 543.f;
        float btnHeight = 108.f;

        for (size_t i = 0; i < labels.size(); ++i) {
            sf::RectangleShape rect(sf::Vector2f(btnWidth, btnHeight));
            rect.setPosition(sf::Vector2f(0.f, btnHeight * (static_cast<float>(i))));
            buttons.push_back(rect);

            if (!labels[i].empty()) {
                sf::Text txt(font);
                txt.setString(labels[i]);
                txt.setCharacterSize(i == 0 ? 36 : 28);

                sf::FloatRect textRect = txt.getLocalBounds();
                txt.setOrigin(sf::Vector2f(textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f));

                sf::FloatRect btnRect = buttons.back().getGlobalBounds();
                txt.setPosition(sf::Vector2f(btnRect.position.x + btnRect.size.x / 2.0f, btnRect.position.y + btnRect.size.y / 2.0f));

                texts.push_back(txt);
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

        for (size_t k = 0; k < boardSquares.size(); ++k) {
            int i = k / 8;
            int j = k % 8;
            if ((i + j) % 2 == 0) {
                boardSquares[k].setFillColor(themes[0].color_BoardSquare1);
            }
            else {
                boardSquares[k].setFillColor(themes[0].color_BoardSquare2);
            }
        }

        for (size_t k = 0; k < boardPieces.size(); ++k) {
            if (k < 8) {
                boardPieces[k].setFillColor(themes[0].color_peace1);
                boardPieces[k].setOutlineColor(themes[0].color_peaceOutline1);
            }
            else {
                boardPieces[k].setFillColor(themes[0].color_peace2);
                boardPieces[k].setOutlineColor(themes[0].color_peaceOutline2);
            }
        }

        bool isHovering = false;

        for (size_t i = 0; i < buttons.size(); ++i) {
            if (labels[i] == "") {
                buttons[i].setFillColor(themes[0].color_buttons);
                continue;
            }

            if (buttons[i].getGlobalBounds().contains(mousePosF)) {
                buttons[i].setFillColor(themes[0].color_HoverButton);
                isHovering = true;
            }
            else {
                buttons[i].setFillColor(themes[0].color_buttons);
            }
        }

        if (isHovering && cursorHand.has_value()) {
            window.setMouseCursor(*cursorHand);
        }
        else if (cursorArrow.has_value()) {
            window.setMouseCursor(*cursorArrow);
        }

        for (const auto& square : boardSquares) {
            window.draw(square);
        }
        for (const auto& piece : boardPieces) {
            window.draw(piece);
        }

        for (const auto& rect : buttons) window.draw(rect);

        for (auto& txt : texts) {
            txt.setFillColor(themes[0].color_text);
            window.draw(txt);
        }
    }

    void handleInput(const sf::Event& event, sf::RenderWindow& window) {
        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {

                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

                for (size_t i = 0; i < buttons.size(); ++i) {
                    if (buttons[i].getGlobalBounds().contains(mousePosF)) {
                        if (labels[i] == "Play") {
                            audio.playClick();
                            interfata = 2;
                        }
                        else if (labels[i] == "Learn") {
                            audio.playClick();
                            interfata = 3;
                        }
                        else if (labels[i] == "Leaderboard") {
                            audio.playClick();
                            interfata = 5;
                        }
                        else if (labels[i] == "Themes") {
                            audio.playClick();
                            interfata = 7;
                        }
                        else if (labels[i] == "Settings") {
                            audio.playClick();
                            interfata = 8;
                        }
                        else if (labels[i] == "Exit") {
                            audio.playClick();
                            window.close();
                        }
                    }
                }
            }
        }
    }
};