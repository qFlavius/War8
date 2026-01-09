#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <optional>
#include <vector>
#include "../Themes/themesRead.h"
#include "../globalVar.hpp"
#include "../Profile/playerData.hpp"
#include "../SoundEffects/SoundEffects.hpp"

struct GameThemes {
    AudioMenu audio;

    sf::Font font;

    sf::RectangleShape back, themeLabel, square1, square2, square3, square4, selectButton1;
    sf::RectangleShape themeLabel_2, square1_2, square2_2, square3_2, square4_2, selectButton1_2;
    sf::RectangleShape themeLabel_3, square1_3, square2_3, square3_3, square4_3, selectButton1_3;
    sf::RectangleShape themeLabel_4, square1_4, square2_4, square3_4, square4_4, selectButton1_4;
    sf::RectangleShape themeLabel_5, square1_5, square2_5, square3_5, square4_5, selectButton1_5;

    sf::RectangleShape popupBG;

    sf::CircleShape peace1, peace2;
    sf::CircleShape peace1_2, peace2_2;
    sf::CircleShape peace1_3, peace2_3;
    sf::CircleShape peace1_4, peace2_4;
    sf::CircleShape peace1_5, peace2_5;

    sf::Text themeName, selectBtn, backTxt;
    sf::Text themeName_2, selectBtn_2;
    sf::Text themeName_3, selectBtn_3;
    sf::Text themeName_4, selectBtn_4;
    sf::Text themeName_5, selectBtn_5;

    std::vector<sf::Text> popUpTxt;
    std::vector<std::string> labels;
    std::vector<sf::CircleShape> SetColors;
    std::vector<sf::CircleShape> PickColors;
    std::vector<sf::Color> PickColorsOptions;
    std::optional<sf::Cursor> cursorHand;
    std::optional<sf::Cursor> cursorArrow;

    sf::Texture SavedIconT;
    std::optional<sf::Sprite> SavedIconS;

    bool popUp = false;
    float screenWidth = 1920.f;
    float screenHeight = 1080.f;

    GameThemes() :
        themeName(font), selectBtn(font), backTxt(font),
        themeName_2(font), selectBtn_2(font),
        themeName_3(font), selectBtn_3(font),
        themeName_4(font), selectBtn_4(font),
        themeName_5(font), selectBtn_5(font)
    {
        if (!font.openFromFile("Themes/Kanit-Medium.ttf")) {
            std::cerr << "Error loading font" << std::endl;
        }
        if (!SavedIconT.loadFromFile("Themes/selected.png")) {
            std::cerr << "Error loading Icon" << std::endl;
        }
        else {
            SavedIconS.emplace(SavedIconT);
            SavedIconS->setOrigin({ 0.f, static_cast<float>(SavedIconT.getSize().y) });

            SavedIconS->setScale({ 0.1f, 0.1f });
        }

        cursorHand = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
        cursorArrow = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);

        // ==================================================================================
        // SETUL 1 (DEFAULT)
        // ==================================================================================

        themeLabel.setSize(sf::Vector2f(731.f, 343.f));
        themeLabel.setFillColor(themes[1].color_bg);
        sf::Vector2f size = themeLabel.getSize();
        float offsetX = size.x / 4.0f;

        themeLabel.setOrigin({ size.x / 2.0f, size.y / 2.0f });
        themeLabel.setPosition({ screenWidth / 4.0f, (screenHeight - 108.f) / 4.0f });

        sf::Vector2f center = themeLabel.getPosition();
        sf::Vector2f sqSize(125.f, 125.f);

        square1.setSize(sqSize); square2.setSize(sqSize);
        square3.setSize(sqSize); square4.setSize(sqSize);
        square1.setFillColor(themes[1].color_BoardSquare1);
        square2.setFillColor(themes[1].color_BoardSquare2);
        square3.setFillColor(themes[1].color_BoardSquare2);
        square4.setFillColor(themes[1].color_BoardSquare1);
        square1.setOrigin({ 125.f, 125.f });
        square2.setOrigin({ 0.f, 125.f });
        square3.setOrigin({ 125.f, 0.f });
        square4.setOrigin({ 0.f, 0.f });
        sf::Vector2f squaresPos1 = { center.x + offsetX, center.y };
        square1.setPosition(squaresPos1); square2.setPosition(squaresPos1);
        square3.setPosition(squaresPos1); square4.setPosition(squaresPos1);

        peace1.setRadius(33.f); peace1.setFillColor(themes[1].color_peace1);
        peace1.setOutlineThickness(14.f); peace1.setOutlineColor(themes[1].color_peaceOutline1);
        float r = peace1.getRadius(); peace1.setOrigin({ r, r });
        peace1.setPosition(square2.getGlobalBounds().getCenter());

        peace2.setRadius(33.f); peace2.setFillColor(themes[1].color_peace2);
        peace2.setOutlineThickness(14.f); peace2.setOutlineColor(themes[1].color_peaceOutline2);
        r = peace2.getRadius(); peace2.setOrigin({ r, r });
        peace2.setPosition(square3.getGlobalBounds().getCenter());

        selectButton1.setSize(sf::Vector2f(316.f, 125.f));
        selectButton1.setFillColor(themes[1].color_HoverButton);
        selectButton1.setOrigin({ 125.f , 0 });
        selectButton1.setPosition({ center.x - offsetX, themeLabel.getPosition().y });

        themeName.setFillColor(sf::Color::Black);
        themeName.setString("Default");
        themeName.setCharacterSize(60);
        sf::FloatRect textRect = themeName.getLocalBounds();
        themeName.setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });
        themeName.setPosition({
            selectButton1.getGlobalBounds().getCenter().x,
            square1.getGlobalBounds().getCenter().y
            });

        selectBtn.setFillColor(sf::Color::Black);
        selectBtn.setString("Select");
        selectBtn.setCharacterSize(35);
        textRect = selectBtn.getLocalBounds();
        selectBtn.setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });
        selectBtn.setPosition(selectButton1.getGlobalBounds().getCenter());

        // ==================================================================================
        // SETUL 2
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
        square1_2.setOrigin({ 125.f, 125.f }); square2_2.setOrigin({ 0.f, 125.f });
        square3_2.setOrigin({ 125.f, 0.f }); square4_2.setOrigin({ 0.f, 0.f });
        sf::Vector2f squaresPos2 = { center.x + offsetX, center.y };
        square1_2.setPosition(squaresPos2); square2_2.setPosition(squaresPos2);
        square3_2.setPosition(squaresPos2); square4_2.setPosition(squaresPos2);

        peace1_2.setRadius(33.f); peace1_2.setFillColor(themes[3].color_peace1);
        peace1_2.setOutlineThickness(14.f); peace1_2.setOutlineColor(themes[3].color_peaceOutline1);
        r = peace1_2.getRadius(); peace1_2.setOrigin({ r, r });
        peace1_2.setPosition(square2_2.getGlobalBounds().getCenter());

        peace2_2.setRadius(33.f); peace2_2.setFillColor(themes[3].color_peace2);
        peace2_2.setOutlineThickness(14.f); peace2_2.setOutlineColor(themes[3].color_peaceOutline2);
        r = peace2_2.getRadius(); peace2_2.setOrigin({ r, r });
        peace2_2.setPosition(square3_2.getGlobalBounds().getCenter());

        selectButton1_2.setSize(sf::Vector2f(316.f, 125.f));
        selectButton1_2.setFillColor(themes[3].color_HoverButton);
        selectButton1_2.setOrigin({ 125.f , 0 });
        selectButton1_2.setPosition({ center.x - offsetX, themeLabel_2.getPosition().y });

        themeName_2.setFillColor(sf::Color::White);
        themeName_2.setString("B & W");
        themeName_2.setCharacterSize(60);
        textRect = themeName_2.getLocalBounds();
        themeName_2.setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });
        themeName_2.setPosition({
            selectButton1_2.getGlobalBounds().getCenter().x,
            square1_2.getGlobalBounds().getCenter().y
            });

        selectBtn_2.setFillColor(sf::Color::White);
        selectBtn_2.setString("Select");
        selectBtn_2.setCharacterSize(35);
        textRect = selectBtn_2.getLocalBounds();
        selectBtn_2.setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });
        selectBtn_2.setPosition(selectButton1_2.getGlobalBounds().getCenter());

        // ==================================================================================
        // SETUL 3
        // ==================================================================================
        themeLabel_3.setSize(sf::Vector2f(731.f, 343.f));
        themeLabel_3.setFillColor(themes[2].color_bg);
        themeLabel_3.setOrigin({ size.x / 2.0f, size.y / 2.0f });
        themeLabel_3.setPosition({ 3.0f * (screenWidth / 12.f), (screenHeight * 0.75f - 108.f) });
        center = themeLabel_3.getPosition();

        square1_3.setSize(sqSize); square2_3.setSize(sqSize);
        square3_3.setSize(sqSize); square4_3.setSize(sqSize);
        square1_3.setFillColor(themes[2].color_BoardSquare1);
        square2_3.setFillColor(themes[2].color_BoardSquare2);
        square3_3.setFillColor(themes[2].color_BoardSquare2);
        square4_3.setFillColor(themes[2].color_BoardSquare1);
        square1_3.setOrigin({ 125.f, 125.f }); square2_3.setOrigin({ 0.f, 125.f });
        square3_3.setOrigin({ 125.f, 0.f }); square4_3.setOrigin({ 0.f, 0.f });
        squaresPos2 = { center.x + offsetX, center.y };
        square1_3.setPosition(squaresPos2); square2_3.setPosition(squaresPos2);
        square3_3.setPosition(squaresPos2); square4_3.setPosition(squaresPos2);

        peace1_3.setRadius(33.f); peace1_3.setFillColor(themes[2].color_peace1);
        peace1_3.setOutlineThickness(14.f); peace1_3.setOutlineColor(themes[2].color_peaceOutline1);
        r = peace1_3.getRadius(); peace1_3.setOrigin({ r, r });
        peace1_3.setPosition(square2_3.getGlobalBounds().getCenter());

        peace2_3.setRadius(33.f); peace2_3.setFillColor(themes[2].color_peace2);
        peace2_3.setOutlineThickness(14.f); peace2_3.setOutlineColor(themes[2].color_peaceOutline2);
        r = peace2_3.getRadius(); peace2_3.setOrigin({ r, r });
        peace2_3.setPosition(square3_3.getGlobalBounds().getCenter());

        selectButton1_3.setSize(sf::Vector2f(316.f, 125.f));
        selectButton1_3.setFillColor(themes[2].color_HoverButton);
        selectButton1_3.setOrigin({ 125.f , 0 });
        selectButton1_3.setPosition({ center.x - offsetX, themeLabel_3.getPosition().y });

        themeName_3.setFillColor(sf::Color::White);
        themeName_3.setString("Default Dark");
        themeName_3.setCharacterSize(60);
        textRect = themeName_3.getLocalBounds();
        themeName_3.setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });
        themeName_3.setPosition({
            selectButton1_3.getGlobalBounds().getCenter().x,
            square1_3.getGlobalBounds().getCenter().y
            });

        selectBtn_3.setFillColor(sf::Color::White);
        selectBtn_3.setString("Select");
        selectBtn_3.setCharacterSize(35);
        textRect = selectBtn_3.getLocalBounds();
        selectBtn_3.setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });
        selectBtn_3.setPosition(selectButton1_3.getGlobalBounds().getCenter());

        // ==================================================================================
        // SETUL 4
        // ==================================================================================
        themeLabel_4.setSize(sf::Vector2f(731.f, 343.f));
        themeLabel_4.setFillColor(themes[4].color_bg);
        themeLabel_4.setOrigin({ size.x / 2.0f, size.y / 2.0f });
        themeLabel_4.setPosition({ 3.0f * (screenWidth / 4.f), (screenHeight * 0.75f - 108.f) });
        center = themeLabel_4.getPosition();

        square1_4.setSize(sqSize); square2_4.setSize(sqSize);
        square3_4.setSize(sqSize); square4_4.setSize(sqSize);
        square1_4.setFillColor(themes[4].color_BoardSquare1);
        square2_4.setFillColor(themes[4].color_BoardSquare2);
        square3_4.setFillColor(themes[4].color_BoardSquare2);
        square4_4.setFillColor(themes[4].color_BoardSquare1);
        square1_4.setOrigin({ 125.f, 125.f }); square2_4.setOrigin({ 0.f, 125.f });
        square3_4.setOrigin({ 125.f, 0.f }); square4_4.setOrigin({ 0.f, 0.f });
        squaresPos2 = { center.x + offsetX, center.y };
        square1_4.setPosition(squaresPos2); square2_4.setPosition(squaresPos2);
        square3_4.setPosition(squaresPos2); square4_4.setPosition(squaresPos2);

        peace1_4.setRadius(33.f); peace1_4.setFillColor(themes[4].color_peace1);
        peace1_4.setOutlineThickness(14.f); peace1_4.setOutlineColor(themes[4].color_peaceOutline1);
        r = peace1_4.getRadius(); peace1_4.setOrigin({ r, r });
        peace1_4.setPosition(square2_4.getGlobalBounds().getCenter());

        peace2_4.setRadius(33.f); peace2_4.setFillColor(themes[4].color_peace2);
        peace2_4.setOutlineThickness(14.f); peace2_4.setOutlineColor(themes[4].color_peaceOutline2);
        r = peace2_4.getRadius(); peace2_4.setOrigin({ r, r });
        peace2_4.setPosition(square3_4.getGlobalBounds().getCenter());

        selectButton1_4.setSize(sf::Vector2f(316.f, 125.f));
        selectButton1_4.setFillColor(themes[4].color_HoverButton);
        selectButton1_4.setOrigin({ 125.f , 0 });
        selectButton1_4.setPosition({ center.x - offsetX, themeLabel_4.getPosition().y });

        themeName_4.setFillColor(sf::Color::Black);
        themeName_4.setString("Custom");
        themeName_4.setCharacterSize(60);
        textRect = themeName_4.getLocalBounds();
        themeName_4.setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });
        themeName_4.setPosition({
            selectButton1_4.getGlobalBounds().getCenter().x,
            square1_4.getGlobalBounds().getCenter().y
            });

        selectBtn_4.setFillColor(sf::Color::Black);
        selectBtn_4.setString("Select");
        selectBtn_4.setCharacterSize(35);
        textRect = selectBtn_4.getLocalBounds();
        selectBtn_4.setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });
        selectBtn_4.setPosition(selectButton1_4.getGlobalBounds().getCenter());

        back.setSize(sf::Vector2f(543.f, 108.f));
        back.setPosition(sf::Vector2f(0, 1080));
        back.setOrigin({ 0, 108.f });
        back.setFillColor(themes[0].color_buttons);

        backTxt.setFillColor(themes[0].color_text);
        backTxt.setString("Back");
        backTxt.setCharacterSize(35);

        textRect = backTxt.getLocalBounds();
        backTxt.setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });

        sf::FloatRect btnBounds = back.getGlobalBounds();

        backTxt.setPosition({
            btnBounds.position.x + btnBounds.size.x / 2.0f,
            btnBounds.position.y + btnBounds.size.y / 2.0f
            });

        //////////////////////////
        // POPUP
        //////////////////////////
        float sizex = 1700, sizey = 850;
        popupBG.setSize(sf::Vector2f(sizex, sizey));
        popupBG.setFillColor(themes[0].color_bg);
        popupBG.setOrigin({ sizex / 2.0f, sizey / 2.0f });
        popupBG.setPosition({ screenWidth / 2.f, screenHeight / 2.f - 52.f });
        
        themeLabel_5.setSize(sf::Vector2f(731.f, 343.f));
        themeLabel_5.setFillColor(themes[4].color_bg);
        themeLabel_5.setOrigin({ size.x / 2.0f, size.y / 2.0f });
        themeLabel_5.setPosition({ 3.0f * (screenWidth / 4.f), (screenHeight * 0.5f - 52.f) });
        center = themeLabel_5.getPosition();

        square1_5.setSize(sqSize); square2_5.setSize(sqSize);
        square3_5.setSize(sqSize); square4_5.setSize(sqSize);
        square1_5.setFillColor(themes[4].color_BoardSquare1);
        square2_5.setFillColor(themes[4].color_BoardSquare2);
        square3_5.setFillColor(themes[4].color_BoardSquare2);
        square4_5.setFillColor(themes[4].color_BoardSquare1);
        square1_5.setOrigin({ 125.f, 125.f }); square2_5.setOrigin({ 0.f, 125.f });
        square3_5.setOrigin({ 125.f, 0.f }); square4_5.setOrigin({ 0.f, 0.f });
        squaresPos2 = { center.x + offsetX, center.y };
        square1_5.setPosition(squaresPos2); square2_5.setPosition(squaresPos2);
        square3_5.setPosition(squaresPos2); square4_5.setPosition(squaresPos2);

        peace1_5.setRadius(33.f); peace1_5.setFillColor(themes[4].color_peace1);
        peace1_5.setOutlineThickness(14.f); peace1_5.setOutlineColor(themes[4].color_peaceOutline1);
        r = peace1_5.getRadius(); peace1_5.setOrigin({ r, r });
        peace1_5.setPosition(square2_5.getGlobalBounds().getCenter());

        peace2_5.setRadius(33.f); peace2_5.setFillColor(themes[4].color_peace2);
        peace2_5.setOutlineThickness(14.f); peace2_5.setOutlineColor(themes[4].color_peaceOutline2);
        r = peace2_5.getRadius(); peace2_5.setOrigin({ r, r });
        peace2_5.setPosition(square3_5.getGlobalBounds().getCenter());

        selectButton1_5.setSize(sf::Vector2f(316.f, 125.f));
        selectButton1_5.setFillColor(themes[4].color_HoverButton);
        selectButton1_5.setOrigin({ 125.f , 0 });
        selectButton1_5.setPosition({ center.x - offsetX, themeLabel_5.getPosition().y });

        themeName_5.setFillColor(sf::Color::Black);
        themeName_5.setString("Custom");
        themeName_5.setCharacterSize(60);
        textRect = themeName_5.getLocalBounds();
        themeName_5.setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });
        themeName_5.setPosition({
            selectButton1_5.getGlobalBounds().getCenter().x,
            square1_5.getGlobalBounds().getCenter().y
            });

        selectBtn_5.setFillColor(sf::Color::Black);
        selectBtn_5.setString("Confirm");
        selectBtn_5.setCharacterSize(35);
        textRect = selectBtn_5.getLocalBounds();
        selectBtn_5.setOrigin({
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
            });
        selectBtn_5.setPosition(selectButton1_5.getGlobalBounds().getCenter());

        labels = {
            "Background", "Buttons", "Hovering Color",
            "Text", "Board Square 1", "Board Square 2",
            "Peace 1", "Peace 2",
            "Peace Outline 1", "Peace Outline 2"
        };
        std::vector<sf::Color> themeColors = {
            themes[4].color_bg,
            themes[4].color_buttons,
            themes[4].color_HoverButton,
            themes[4].color_text,
            themes[4].color_BoardSquare1,
            themes[4].color_BoardSquare2,
            themes[4].color_peace1,
            themes[4].color_peace2,
            themes[4].color_peaceOutline1,
            themes[4].color_peaceOutline2
        };
        for (size_t i = 0; i < labels.size(); ++i) {
            sf::Text txt(font);
            txt.setFillColor(themes[0].color_text);
            txt.setString(labels[i]);
            txt.setCharacterSize(30);
            textRect = txt.getLocalBounds();
            txt.setOrigin({
                textRect.position.x + textRect.size.x / 2.0f,
                textRect.position.y + textRect.size.y / 2.0f
                });
            txt.setPosition({
                popupBG.getOrigin().x / 2,
                popupBG.getPosition().y - popupBG.getOrigin().y + 80.f * (i + 1.f)
                });
            popUpTxt.push_back(txt);

            sf::CircleShape color;
            color.setRadius(30.f);

            sf::Color currentColor;
            if (i < themeColors.size()) {
                currentColor = themeColors[i];
            }
            else {
                currentColor = sf::Color::Magenta;
            }
            color.setFillColor(currentColor);

            float luminance = (0.299f * currentColor.r) + (0.587f * currentColor.g) + (0.114f * currentColor.b);
            color.setOutlineThickness(3.f);

            if (luminance > 140.f) {
                color.setOutlineColor(sf::Color::Black);
            }
            else {
                color.setOutlineColor(sf::Color::White);
            }

            r = color.getRadius(); color.setOrigin({ r, r });
            color.setPosition({ txt.getPosition().x + 250.f, txt.getPosition().y });
            SetColors.push_back(color);
        }

        PickColorsOptions = {
            sf::Color(243, 22, 176),
            sf::Color(171, 89, 152),
            sf::Color(224, 62, 26),
            sf::Color(246, 80, 9),
            sf::Color(212, 105, 59),
            sf::Color(243, 193, 27),
            sf::Color(25, 143, 81),
            sf::Color(103, 142, 121),
            sf::Color(12, 140, 233),
            sf::Color(61, 56, 245),
            sf::Color(138, 56, 245),
            sf::Color(127, 105, 155),
            sf::Color(0, 0, 0),
            sf::Color(255, 255, 255)
        };

        float i = 0;
        for (const auto& color2 : PickColorsOptions) {
            sf::CircleShape color;
            color.setRadius(30.f);
            color.setFillColor(color2);

            float luminance = (0.299f * color2.r) + (0.587f * color2.g) + (0.114f * color2.b);
            color.setOutlineThickness(3.f);
            if (luminance > 140.f) {
                color.setOutlineColor(sf::Color::Black);
            }
            else {
                color.setOutlineColor(sf::Color::White);
            }

            r = color.getRadius();
            color.setOrigin({ r, r });
            color.setPosition({
                popupBG.getOrigin().x / 2 + i * 87.f,
                popupBG.getPosition().y - popupBG.getOrigin().y
                });
            PickColors.push_back(color);
            i++;
        }

    }

    void pop_Up() {
        popUp = true;
    }

    void handleInput(const sf::Event& event, sf::RenderWindow& window) {
        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {

                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

                popupBG.setFillColor(themes[0].color_bg);
                if (popUp) {
                    // CONFIRM button
                    if (selectButton1_5.getGlobalBounds().contains(mousePosF)) {
                        audio.playClick();
                        themes[0] = themes[4];
                        themeAdress[0] = themeAdress[4];
                        themeLabel_4.setFillColor(themes[4].color_bg);
                        SavePlayerData();
                        popUp = false;
                        return;
                    }
                    // BACK button
                    if (back.getGlobalBounds().contains(mousePosF)) {
                        audio.playClick();
                        popUp = false;
                        return;
                    }

                    // CIRCLE selection
                    bool clickedOnASetColor = false;
                    for (auto& circle : SetColors) {
                        if (circle.getGlobalBounds().contains(mousePosF)) {
                            clickedOnASetColor = true;
                            break;
                        }
                    }

                    if (clickedOnASetColor) {
                        for (auto& circle : SetColors) {
                            if (circle.getGlobalBounds().contains(mousePosF)) {
                                audio.playClick();
                                circle.setScale({ 1.3f, 1.3f });
                            }
                            else {
                                circle.setScale({ 1.f, 1.f });
                            }
                        }
                    }

                    // APPLYING colors
                    for (const auto& pickCircle : PickColors) {
                        if (pickCircle.getGlobalBounds().contains(mousePosF)) {
                            audio.playClick();

                            sf::Color newColor = pickCircle.getFillColor();

                            for (size_t i = 0; i < SetColors.size(); ++i) {
                                if (SetColors[i].getScale().x > 1.1f) {

                                    SetColors[i].setFillColor(newColor);

                                    float luminance = (0.299f * newColor.r) + (0.587f * newColor.g) + (0.114f * newColor.b);
                                    if (luminance > 140.f) SetColors[i].setOutlineColor(sf::Color::Black);
                                    else SetColors[i].setOutlineColor(sf::Color::White);

                                    switch (i) {
                                    case 0: themes[4].color_bg = newColor; break;
                                    case 1: themes[4].color_buttons = newColor; break;
                                    case 2: themes[4].color_HoverButton = newColor; break;
                                    case 3: themes[4].color_text = newColor; break;
                                    case 4: themes[4].color_BoardSquare1 = newColor; break;
                                    case 5: themes[4].color_BoardSquare2 = newColor; break;
                                    case 6: themes[4].color_peace1 = newColor; break;
                                    case 7: themes[4].color_peace2 = newColor; break;
                                    case 8: themes[4].color_peaceOutline1 = newColor; break;
                                    case 9: themes[4].color_peaceOutline2 = newColor; break;
                                    }

                                    themeLabel_5.setFillColor(themes[4].color_bg);
                                    square1_5.setFillColor(themes[4].color_BoardSquare1);
                                    square4_5.setFillColor(themes[4].color_BoardSquare1);
                                    square2_5.setFillColor(themes[4].color_BoardSquare2);
                                    square3_5.setFillColor(themes[4].color_BoardSquare2);
                                    peace1_5.setFillColor(themes[4].color_peace1);
                                    peace1_5.setOutlineColor(themes[4].color_peaceOutline1);
                                    peace2_5.setFillColor(themes[4].color_peace2);
                                    peace2_5.setOutlineColor(themes[4].color_peaceOutline2);
                                    selectButton1_5.setFillColor(themes[4].color_HoverButton);
                                    themeName_5.setFillColor(themes[4].color_text);
                                }
                            }
                        }
                    }
                    return;
                }

                if (selectButton1.getGlobalBounds().contains(mousePosF)) {
                    audio.playClick();
                    themes[0] = themes[1]; themeAdress[0] = themeAdress[1]; SavePlayerData();
                }
                else if (selectButton1_2.getGlobalBounds().contains(mousePosF)) {
                    audio.playClick();
                    themes[0] = themes[3]; themeAdress[0] = themeAdress[3]; SavePlayerData();
                }
                else if (selectButton1_3.getGlobalBounds().contains(mousePosF)) {
                    audio.playClick();
                    themes[0] = themes[2]; themeAdress[0] = themeAdress[2]; SavePlayerData();
                }
                else if (selectButton1_4.getGlobalBounds().contains(mousePosF)) {
                    audio.playClick();
                    popUp = true;
                }
                else if (back.getGlobalBounds().contains(mousePosF)) {
                    audio.playClick();
                    interfata = 1;
                }
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

        back.setFillColor(themes[0].color_buttons);
        bool isHovering = false;
        if (back.getGlobalBounds().contains(mousePosF)) {
            back.setFillColor(themes[0].color_HoverButton);
            isHovering = true;
        }
        else if (selectButton1.getGlobalBounds().contains(mousePosF)) {
            selectButton1.setFillColor(themes[1].color_HoverButton);
            isHovering = true;
        }
        else if (selectButton1_2.getGlobalBounds().contains(mousePosF)) {
            selectButton1_2.setFillColor(themes[3].color_HoverButton);
            isHovering = true;
        }
        else if (selectButton1_3.getGlobalBounds().contains(mousePosF)) {
            selectButton1_3.setFillColor(themes[2].color_HoverButton);
            isHovering = true;
        }
        else if (selectButton1_4.getGlobalBounds().contains(mousePosF)) {
            selectButton1_4.setFillColor(themes[4].color_HoverButton);
            isHovering = true;
        }
        else if (selectButton1_5.getGlobalBounds().contains(mousePosF)) {
            selectButton1_5.setFillColor(themes[4].color_HoverButton);
            isHovering = true;
        }
        else {
            back.setFillColor(themes[0].color_buttons);
            selectButton1.setFillColor(themes[1].color_buttons);
            selectButton1_2.setFillColor(themes[3].color_buttons);
            selectButton1_3.setFillColor(themes[2].color_buttons);
            selectButton1_4.setFillColor(themes[4].color_buttons);
            selectButton1_5.setFillColor(themes[4].color_buttons);
        }
        backTxt.setFillColor(themes[0].color_text);
        window.draw(back);
        window.draw(backTxt);
        // Setul 1
        window.draw(themeLabel);
        window.draw(square1);
        window.draw(square2);
        window.draw(square3);
        window.draw(square4);
        window.draw(peace1);
        window.draw(peace2);
        window.draw(selectButton1);

        window.draw(themeName);
        window.draw(selectBtn);

        // Setul 2
        window.draw(themeLabel_2);
        window.draw(square1_2);
        window.draw(square2_2);
        window.draw(square3_2);
        window.draw(square4_2);
        window.draw(peace1_2);
        window.draw(peace2_2);
        window.draw(selectButton1_2);

        window.draw(themeName_2);
        window.draw(selectBtn_2);

        // Setul 3
        window.draw(themeLabel_3);
        window.draw(square1_3);
        window.draw(square2_3);
        window.draw(square3_3);
        window.draw(square4_3);
        window.draw(peace1_3);
        window.draw(peace2_3);
        window.draw(selectButton1_3);

        window.draw(themeName_3);
        window.draw(selectBtn_3);

        // Setul 4
        window.draw(themeLabel_4);
        window.draw(square1_4);
        window.draw(square2_4);
        window.draw(square3_4);
        window.draw(square4_4);
        window.draw(peace1_4);
        window.draw(peace2_4);
        window.draw(selectButton1_4);

        window.draw(themeName_4);
        window.draw(selectBtn_4);

        if (themeAdress[0] == themeAdress[1]) {
            themeLabel.setOutlineThickness(4);
            themeLabel.setOutlineColor(sf::Color::Black);

            sf::FloatRect bounds = themeLabel.getGlobalBounds();
            SavedIconS->setPosition({
                bounds.position.x + 6,
                bounds.position.y + static_cast<float>(SavedIconT.getSize().y * 0.1f + 5)
                });
            SavedIconS->setColor(sf::Color::Black);
            window.draw(*SavedIconS);

            themeLabel_4.setOutlineThickness(0);
            themeLabel_2.setOutlineThickness(0);
            themeLabel_3.setOutlineThickness(0);
        }
        else if (themeAdress[0] == themeAdress[2]) {
            themeLabel_3.setOutlineThickness(4);
            themeLabel_3.setOutlineColor(sf::Color::White);

            sf::FloatRect bounds = themeLabel_3.getGlobalBounds();
            SavedIconS->setPosition({
                bounds.position.x + 6,
                bounds.position.y + static_cast<float>(SavedIconT.getSize().y * 0.1f + 5)
                });
            SavedIconS->setColor(sf::Color::White);
            window.draw(*SavedIconS);

            themeLabel.setOutlineThickness(0);
            themeLabel_2.setOutlineThickness(0);
            themeLabel_4.setOutlineThickness(0);
        }
        else if (themeAdress[0] == themeAdress[3]) {
            themeLabel_2.setOutlineThickness(4);
            themeLabel_2.setOutlineColor(sf::Color::White);

            sf::FloatRect bounds = themeLabel_2.getGlobalBounds();
            SavedIconS->setPosition({
                bounds.position.x + 6,
                bounds.position.y + static_cast<float>(SavedIconT.getSize().y * 0.1f + 5)
                });
            SavedIconS->setColor(sf::Color::White);
            window.draw(*SavedIconS);

            themeLabel.setOutlineThickness(0);
            themeLabel_4.setOutlineThickness(0);
            themeLabel_3.setOutlineThickness(0);
        }
        else if (themeAdress[0] == themeAdress[4]) {
            themeLabel_4.setOutlineThickness(4);

            sf::Color bg = themes[4].color_bg;
            float luminance = (0.299f * bg.r) + (0.587f * bg.g) + (0.114f * bg.b);

            sf::Color contrastColor = (luminance > 140.f) ? sf::Color::Black : sf::Color::White;

            themeLabel_4.setOutlineColor(contrastColor);

            sf::FloatRect bounds = themeLabel_4.getGlobalBounds();
            SavedIconS->setPosition({
                bounds.position.x + 6,
                bounds.position.y + static_cast<float>(SavedIconT.getSize().y * 0.1f + 5)
                });

            SavedIconS->setColor(contrastColor);
            window.draw(*SavedIconS);

            themeLabel.setOutlineThickness(0);
            themeLabel_2.setOutlineThickness(0);
            themeLabel_3.setOutlineThickness(0);
        }
        if (popUp) {
            if (selectButton1_5.getGlobalBounds().contains(mousePosF)) {
                selectButton1_5.setFillColor(themes[4].color_HoverButton);
                isHovering = true;
            }
            window.draw(popupBG);
            window.draw(themeLabel_5);
            window.draw(square1_5);
            window.draw(square2_5);
            window.draw(square3_5);
            window.draw(square4_5);
            window.draw(peace1_5);
            window.draw(peace2_5);
            window.draw(selectButton1_5);

            window.draw(themeName_5);
            window.draw(selectBtn_5);

            for (auto& txt : popUpTxt) {
                txt.setFillColor(themes[0].color_text);
                window.draw(txt);
            }
            for (const auto& color : SetColors) {
                window.draw(color);
                if (color.getGlobalBounds().contains(mousePosF)) {
                    isHovering = true;
                }
            }
            for (const auto& color : PickColors) {
                window.draw(color);
                if (color.getGlobalBounds().contains(mousePosF)) {
                    isHovering = true;
                }
            }
        }
        if (isHovering && cursorHand.has_value()) {
            window.setMouseCursor(*cursorHand);
        }
        else if (cursorArrow.has_value()) {
            window.setMouseCursor(*cursorArrow);
        }
    }
};