#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <optional>
#include "../Themes/themesRead.h"
#include "../globalVar.hpp"


struct GameThemes {
    sf::RectangleShape back, themeLabel, square1, square2, square3, square4, selectButton1;
    sf::RectangleShape themeLabel_2, square1_2, square2_2, square3_2, square4_2, selectButton1_2;

    sf::CircleShape peace1, peace2;
    sf::CircleShape peace1_2, peace2_2;

    std::optional<sf::Text> themeName, selectBtn;
    std::optional<sf::Text> themeName_2, selectBtn_2;

    std::optional<sf::Cursor> cursorHand;
    std::optional<sf::Cursor> cursorArrow;
    sf::Font font;

    GameThemes() {
        if (!font.openFromFile("Themes/Kanit-Medium.ttf")) {
            std::cerr << "Error loading font" << std::endl;
        }
        themeName.emplace(font);
        selectBtn.emplace(font);
        themeName_2.emplace(font);
        selectBtn_2.emplace(font);

        cursorHand = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
        cursorArrow = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);

        float screenWidth = 1920.f;
        float screenHeight = 1080.f;

        // ==================================================================================
        // SETUL 1 (DEFAULT) - POZITIONAT LA 1/4 DIN ECRAN
        // ==================================================================================

        themeLabel.setSize(sf::Vector2f(731.f, 343.f));
        themeLabel.setFillColor(themes[0].color_buttons);
        sf::Vector2f size = themeLabel.getSize();

        // 731 / 4 = aprox 182 px stanga/dreapta fata de centru
        float offsetX = size.x / 4.0f;

        themeLabel.setOrigin({ size.x / 2.0f, size.y / 2.0f });
        themeLabel.setPosition({ screenWidth / 4.0f, (screenHeight - 108.f) / 4.0f });

        sf::Vector2f center = themeLabel.getPosition();

        sf::Vector2f sqSize(125.f, 125.f);
        square1.setSize(sqSize); square2.setSize(sqSize);
        square3.setSize(sqSize); square4.setSize(sqSize);

        square1.setFillColor(themes[0].color_BoardSquare1);
        square2.setFillColor(themes[0].color_BoardSquare2);
        square3.setFillColor(themes[0].color_BoardSquare2);
        square4.setFillColor(themes[0].color_BoardSquare1);

        square1.setOrigin({ 125.f, 125.f });
        square2.setOrigin({ 0.f, 125.f });
        square3.setOrigin({ 125.f, 0.f });
        square4.setOrigin({ 0.f, 0.f });

        sf::Vector2f squaresPos1 = { center.x + offsetX, center.y };
        square1.setPosition(squaresPos1);
        square2.setPosition(squaresPos1);
        square3.setPosition(squaresPos1);
        square4.setPosition(squaresPos1);

        peace1.setRadius(33.f);
        peace1.setFillColor(themes[0].color_peace1);
        peace1.setOutlineThickness(14.f);
        peace1.setOutlineColor(themes[0].color_peaceOutline1);
        float r = peace1.getRadius();
        peace1.setOrigin({ r, r });
        peace1.setPosition(square2.getGlobalBounds().getCenter());

        peace2.setRadius(33.f);
        peace2.setFillColor(themes[0].color_peace2);
        peace2.setOutlineThickness(14.f);
        peace2.setOutlineColor(themes[0].color_peaceOutline2);
        r = peace2.getRadius();
        peace2.setOrigin({ r, r });
        peace2.setPosition(square3.getGlobalBounds().getCenter());

        selectButton1.setSize(sf::Vector2f(316.f, 125.f));
        selectButton1.setFillColor(themes[0].color_HoverButton);
        selectButton1.setOrigin({ 125.f , 0 });
        selectButton1.setPosition({ center.x - offsetX, themeLabel.getPosition().y });

        themeName->setFillColor(sf::Color::Black);
        themeName->setString("Default");
        themeName->setCharacterSize(60);
        sf::FloatRect textRect = themeName->getLocalBounds();
        themeName->setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });

        themeName->setPosition({
            selectButton1.getGlobalBounds().getCenter().x,
            square1.getGlobalBounds().getCenter().y
            });

        selectBtn->setFillColor(sf::Color::Black);
        selectBtn->setString("Select");
        selectBtn->setCharacterSize(35);
        textRect = selectBtn->getLocalBounds();
        selectBtn->setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });
        selectBtn->setPosition(selectButton1.getGlobalBounds().getCenter());

        // ==================================================================================
        // SETUL 2 (TEMA 2) - POZITIONAT LA 2/4 (Jumatate) DIN ECRAN
        // ==================================================================================

        themeLabel_2.setSize(sf::Vector2f(731.f, 343.f));
        themeLabel_2.setFillColor(themes[3].color_bg);
        themeLabel_2.setOrigin({ size.x / 2.0f, size.y / 2.0f });
        themeLabel_2.setPosition({ 3.0f * (screenWidth / 4.0f), (screenHeight - 108.f) / 4.0f });

        center = themeLabel_2.getPosition();

        square1_2.setSize(sqSize); square2_2.setSize(sqSize);
        square3_2.setSize(sqSize); square4_2.setSize(sqSize);

        square1_2.setFillColor(themes[3].color_BoardSquare1);
        square2_2.setFillColor(themes[3].color_BoardSquare2);
        square3_2.setFillColor(themes[3].color_BoardSquare2);
        square4_2.setFillColor(themes[3].color_BoardSquare1);

        square1_2.setOrigin({ 125.f, 125.f });
        square2_2.setOrigin({ 0.f, 125.f });
        square3_2.setOrigin({ 125.f, 0.f });
        square4_2.setOrigin({ 0.f, 0.f });

        sf::Vector2f squaresPos2 = { center.x + offsetX, center.y };
        square1_2.setPosition(squaresPos2);
        square2_2.setPosition(squaresPos2);
        square3_2.setPosition(squaresPos2);
        square4_2.setPosition(squaresPos2);

        peace1_2.setRadius(33.f);
        peace1_2.setFillColor(themes[3].color_peace1);
        peace1_2.setOutlineThickness(14.f);
        peace1_2.setOutlineColor(themes[3].color_peaceOutline1);
        r = peace1_2.getRadius();
        peace1_2.setOrigin({ r, r });
        peace1_2.setPosition(square2_2.getGlobalBounds().getCenter());

        peace2_2.setRadius(33.f);
        peace2_2.setFillColor(themes[3].color_peace2);
        peace2_2.setOutlineThickness(14.f);
        peace2_2.setOutlineColor(themes[3].color_peaceOutline2);
        r = peace2_2.getRadius();
        peace2_2.setOrigin({ r, r });
        peace2_2.setPosition(square3_2.getGlobalBounds().getCenter());

        selectButton1_2.setSize(sf::Vector2f(316.f, 125.f));
        selectButton1_2.setFillColor(themes[3].color_HoverButton);
        selectButton1_2.setOrigin({ 125.f , 0 });
        selectButton1_2.setPosition({ center.x - offsetX, themeLabel_2.getPosition().y });

        themeName_2->setFillColor(sf::Color::White);
        themeName_2->setString("B & W");
        themeName_2->setCharacterSize(60);
        textRect = themeName_2->getLocalBounds();
        themeName_2->setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });
        themeName_2->setPosition({
            selectButton1_2.getGlobalBounds().getCenter().x,
            square1_2.getGlobalBounds().getCenter().y
            });

        selectBtn_2->setFillColor(sf::Color::White);
        selectBtn_2->setString("Select");
        selectBtn_2->setCharacterSize(35);
        textRect = selectBtn_2->getLocalBounds();
        selectBtn_2->setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });
        selectBtn_2->setPosition(selectButton1_2.getGlobalBounds().getCenter());
    }

    void draw(sf::RenderWindow& window) {
        // Setul 1
        window.draw(themeLabel);
        window.draw(square1);
        window.draw(square2);
        window.draw(square3);
        window.draw(square4);
        window.draw(peace1);
        window.draw(peace2);
        window.draw(selectButton1);

        if (themeName.has_value()) window.draw(*themeName);
        if (selectBtn.has_value()) window.draw(*selectBtn);

        // Setul 2
        window.draw(themeLabel_2);
        window.draw(square1_2);
        window.draw(square2_2);
        window.draw(square3_2);
        window.draw(square4_2);
        window.draw(peace1_2);
        window.draw(peace2_2);
        window.draw(selectButton1_2);

        if (themeName_2.has_value()) window.draw(*themeName_2);
        if (selectBtn_2.has_value()) window.draw(*selectBtn_2);
    }
};