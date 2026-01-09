#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm> 
#include <iostream>
#include <optional>
#include "../Themes/themesRead.h"
#include "../globalVar.hpp"
#include "../SoundEffects/SoundEffects.hpp"

struct PlayerEntry {
    std::string name;
    int score;
};

struct LeaderboardMenu {
    AudioMenu audio;
    const float SCREEN_W = 1920.f;
    const float SCREEN_H = 1080.f;

    sf::Font font;
    sf::RectangleShape back;
    sf::Text backTxt;

    sf::RectangleShape headerShape;
    sf::RectangleShape leftColShape;
    sf::RectangleShape rightColShape;

    sf::Text titleTxt;

    std::vector<sf::Text> nameTexts;
    std::vector<sf::Text> scoreTexts;

    std::vector<PlayerEntry> players;

    std::optional<sf::Cursor> cursorHand;
    std::optional<sf::Cursor> cursorArrow;

    LeaderboardMenu() : backTxt(font), titleTxt(font) {
        if (!font.openFromFile("Themes/Kanit-Medium.ttf")) {
            std::cerr << "Error loading font" << std::endl;
        }

        cursorHand = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
        cursorArrow = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);

        button(back, backTxt, "Back", sf::Vector2f(543.f, 108.f), sf::Vector2f(0.f, SCREEN_H), sf::Vector2f(0.f, 108.f));

        float tableWidth = 700.f;
        float headerHeight = 80.f;
        float rowHeight = 60.f;
        int maxRows = 10;
        float tableHeight = maxRows * rowHeight;

        float scoreColWidth = 180.f;
        float nameColWidth = tableWidth - scoreColWidth;

        float startX = (SCREEN_W - tableWidth) / 2.f;
        float startY = 150.f;

        headerShape.setSize(sf::Vector2f(tableWidth, headerHeight));
        headerShape.setPosition(sf::Vector2f(startX, startY));
        headerShape.setFillColor(themes[0].color_buttons);

        leftColShape.setSize(sf::Vector2f(nameColWidth, tableHeight));
        leftColShape.setPosition(sf::Vector2f(startX, startY + headerHeight));
        leftColShape.setFillColor(themes[0].color_buttons);

        rightColShape.setSize(sf::Vector2f(scoreColWidth, tableHeight));
        rightColShape.setPosition(sf::Vector2f(startX + nameColWidth, startY + headerHeight));
        rightColShape.setFillColor(themes[0].color_HoverButton);

        titleTxt.setFont(font);
        titleTxt.setString("Top Wins");
        titleTxt.setCharacterSize(40);
        titleTxt.setFillColor(themes[0].color_text);

        sf::FloatRect tr = titleTxt.getLocalBounds();
        titleTxt.setOrigin(sf::Vector2f(tr.position.x + tr.size.x / 2.0f, tr.position.y + tr.size.y / 2.0f));
        titleTxt.setPosition(sf::Vector2f(startX + tableWidth / 2.f, startY + headerHeight / 2.f));

        LoadLeaderboard();
    }

    void LoadLeaderboard() {
        players.clear();
        nameTexts.clear();
        scoreTexts.clear();

        std::ifstream fin("LeaderBoard/leaderboard.txt");
        if (fin.is_open()) {
            std::string n;
            int s;
            while (fin >> n >> s) {
                players.push_back({ n, s });
            }
            fin.close();
        }

        std::sort(players.begin(), players.end(), [](const PlayerEntry& a, const PlayerEntry& b) {
            return a.score > b.score;
            });

        float startX = (SCREEN_W - 700.f) / 2.f;
        float startY = 150.f + 80.f;
        float rowHeight = 60.f;
        float nameColWidth = 700.f - 180.f;
        float scoreColWidth = 180.f;

        for (int i = 0; i < 10; i++) {

            std::string nameStr;
            std::string scoreStr;

            if (i < players.size()) {
                nameStr = std::to_string(i + 1) + ". " + players[i].name;
                scoreStr = std::to_string(players[i].score);
            }
            else {
                nameStr = std::to_string(i + 1) + ". -";
                scoreStr = "-";
            }

            sf::Text tName(font);
            tName.setString(nameStr);
            tName.setCharacterSize(30);
            tName.setFillColor(themes[0].color_text);

            sf::FloatRect nr = tName.getLocalBounds();
            tName.setOrigin(sf::Vector2f(nr.position.x + nr.size.x / 2.0f, nr.position.y + nr.size.y / 2.0f));
            tName.setPosition(sf::Vector2f(startX + nameColWidth / 2.f, startY + (i * rowHeight) + (rowHeight / 2.f)));
            nameTexts.push_back(tName);

            sf::Text tScore(font);
            tScore.setString(scoreStr);
            tScore.setCharacterSize(30);
            tScore.setFillColor(themes[0].color_text);

            sf::FloatRect sr = tScore.getLocalBounds();
            tScore.setOrigin(sf::Vector2f(sr.position.x + sr.size.x / 2.0f, sr.position.y + sr.size.y / 2.0f));
            tScore.setPosition(sf::Vector2f(startX + nameColWidth + scoreColWidth / 2.f, startY + (i * rowHeight) + (rowHeight / 2.f)));
            scoreTexts.push_back(tScore);
        }
    }

    void button(sf::RectangleShape& shape, sf::Text& text, const std::string& label, sf::Vector2f size, sf::Vector2f position, sf::Vector2f origin) {
        shape.setSize(size);
        shape.setPosition(position);
        shape.setOrigin(origin);
        shape.setFillColor(themes[0].color_buttons);
        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(35);
        text.setFillColor(themes[0].color_text);

        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(sf::Vector2f(
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
        ));

        sf::FloatRect btnBounds = shape.getGlobalBounds();
        text.setPosition(sf::Vector2f(
            btnBounds.position.x + btnBounds.size.x / 2.0f,
            btnBounds.position.y + btnBounds.size.y / 2.0f
        ));
    }

    void handleInput(const sf::Event& event, sf::RenderWindow& window) {
        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

                if (back.getGlobalBounds().contains(mousePosF)) {
                    audio.playClick();
                    interfata = 1;
                    LoadLeaderboard();
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

        if (isHovering && cursorHand.has_value()) window.setMouseCursor(*cursorHand);
        else if (cursorArrow.has_value()) window.setMouseCursor(*cursorArrow);

        window.draw(back);
        window.draw(backTxt);

        window.draw(headerShape);
        window.draw(leftColShape);
        window.draw(rightColShape);
        window.draw(titleTxt);

        for (const auto& t : nameTexts) {
            window.draw(t);
        }
        for (const auto& t : scoreTexts) {
            window.draw(t);
        }
    }
};