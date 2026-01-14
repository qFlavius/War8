#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include "../Themes/themesRead.h"
#include "../globalVar.hpp"
#include "../SoundEffects/SoundEffects.hpp"
#include "../lang/Translations.hpp"

struct LearnMenu {
    AudioMenu audio;

    const float SCREEN_W = 1920.f;
    const float SCREEN_H = 1080.f;

    const int BODY_FONT_SIZE = 36;
    const int HEADER_FONT_SIZE = 48;

    int currentPageNum = 1;

    sf::Color lastThemeBgColor;
    std::string lastLanguage;

    sf::Font font, font2;

    std::vector<sf::Texture> pageTextures;
    std::optional<sf::Sprite> pageSprite;

    sf::RectangleShape back, previousPage, nextPage, currentPage;
    sf::Text backTxt, previousTxt, nextTxt, currentTxt;

    sf::RectangleShape FirstPage;
    std::vector<sf::Text> pageContent;

    std::optional<sf::Cursor> cursorHand;
    std::optional<sf::Cursor> cursorArrow;

    LearnMenu() :
        backTxt(font),
        previousTxt(font),
        nextTxt(font),
        currentTxt(font)
    {
        if (!font.openFromFile("Themes/Inter_18pt-Bold.ttf")) {
            std::cerr << "Error loading font" << std::endl;
        }
        if (!font2.openFromFile("Themes/Inter_18pt-Regular.ttf")) {
            std::cerr << "Error loading font" << std::endl;
        }

        pageTextures.resize(5);
        for (int i = 0; i < 5; ++i) {
            std::string filename = "Learn/fig" + std::to_string(i + 1) + ".png";
            if (!pageTextures[i].loadFromFile(filename)) {
                std::cerr << "Error loading " << filename << std::endl;
            }
        }

        if (!pageTextures.empty()) {
            pageSprite.emplace(pageTextures[0]);
        }

        cursorHand = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
        cursorArrow = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);

        button(back, backTxt, std::string(GetTranslation(TR_LEARN_BACK)), sf::Vector2f(543.f, 108.f), sf::Vector2f(0.f, SCREEN_H), sf::Vector2f(0.f, 108.f));

        button(currentPage, currentTxt, "1/5", sf::Vector2f(543.f, 108.f), sf::Vector2f(SCREEN_W, SCREEN_H), sf::Vector2f(543.f, 108.f));
        button(previousPage, previousTxt, "<", sf::Vector2f(206.f, 108.f), sf::Vector2f(SCREEN_W - 543.f, SCREEN_H), sf::Vector2f(0.f, 108.f));
        button(nextPage, nextTxt, ">", sf::Vector2f(206.f, 108.f), sf::Vector2f(SCREEN_W, SCREEN_H), sf::Vector2f(206.f, 108.f));
        currentPage.setFillColor(sf::Color::Transparent);

        loadPage(1);

        lastThemeBgColor = themes[0].color_bg;
        lastLanguage = language;
    }

    void div(sf::RectangleShape& shape, sf::Vector2f size, sf::Vector2f position, sf::Vector2f origin) {
        shape.setSize(size);
        shape.setPosition(position);
        shape.setOrigin(origin);
        shape.setFillColor(sf::Color::Transparent);
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

    void wrapText(sf::Text& text, float maxWidth) {
        std::string str = text.getString();
        std::stringstream ss(str);
        std::string paragraph;
        std::string finalString = "";

        while (std::getline(ss, paragraph)) {
            if (paragraph.empty()) {
                finalString += "\n";
                continue;
            }

            std::string indent = "";
            size_t firstNonSpace = paragraph.find_first_not_of(" \t");
            if (firstNonSpace != std::string::npos && firstNonSpace > 0) {
                indent = paragraph.substr(0, firstNonSpace);
            }

            std::stringstream wordStream(paragraph);
            std::string word;
            std::string currentLine = indent;
            bool firstWord = true;

            while (wordStream >> word) {
                std::string testLine;
                if (firstWord) {
                    testLine = currentLine + word;
                }
                else {
                    testLine = currentLine + " " + word;
                }

                sf::Text tempText = text;
                tempText.setString(testLine);

                if (tempText.getLocalBounds().size.x > maxWidth) {
                    finalString += currentLine + "\n";
                    currentLine = word;
                }
                else {
                    currentLine = testLine;
                }
                firstWord = false;
            }
            finalString += currentLine + "\n";
        }

        if (!finalString.empty() && finalString.back() == '\n') {
            finalString.pop_back();
        }

        text.setString(finalString);
    }

    void getPageLayout(float& startX, float& currentY, float& maxWidth) {
        div(FirstPage,
            sf::Vector2f(SCREEN_W - 100.f, SCREEN_H - 200.f),
            sf::Vector2f(SCREEN_W / 2.f, SCREEN_H / 2.f - 108.f / 2.f),
            sf::Vector2f((SCREEN_W - 100.f) / 2.f, (SCREEN_H - 200.f) / 2.f)
        );

        pageContent.clear();

        sf::FloatRect bounds = FirstPage.getGlobalBounds();
        currentY = bounds.position.y + 20.f;
        startX = bounds.position.x + 20.f;
        float containerW = bounds.size.x;
        float containerH = bounds.size.y;
        maxWidth = containerW * 0.65f - 40.f;

        float imgCenterX = bounds.position.x + (containerW * 0.70f) + ((containerW * 0.30f) / 2.0f);
        float imgCenterY = bounds.position.y + (containerH / 2.0f);

        if (pageSprite.has_value()) {
            sf::FloatRect spriteBounds = pageSprite->getLocalBounds();
            pageSprite->setOrigin(sf::Vector2f(spriteBounds.size.x / 2.0f, spriteBounds.size.y / 2.0f));
            pageSprite->setPosition(sf::Vector2f(imgCenterX - 40.f, imgCenterY));
        }
    }

    void centerContent(float initialY, float finalY) {
        sf::FloatRect bounds = FirstPage.getGlobalBounds();
        float containerCenterY = bounds.position.y + bounds.size.y / 2.0f;

        float contentCenterY = (initialY + finalY) / 2.0f;

        float shiftY = containerCenterY - contentCenterY;

        for (auto& txt : pageContent) {
            txt.move(sf::Vector2f(0.f, shiftY));
        }
    }

    void addBlock(std::string str, int fontSize, sf::Color color, float startX, float& currentY, float maxWidth) {
        const sf::Font& selectedFont = (fontSize == BODY_FONT_SIZE) ? font2 : font;
        sf::Text t(selectedFont);
        t.setString(str);
        t.setCharacterSize(fontSize);
        t.setFillColor(color);
        wrapText(t, maxWidth);
        t.setPosition(sf::Vector2f(startX, currentY));
        pageContent.push_back(t);
        currentY += t.getGlobalBounds().size.y + 20.f;
    }

    void addMixedBlock(std::string boldText, std::string regText, float startX, float& currentY, float maxWidth) {
        sf::Text tTitle(font);
        tTitle.setString(boldText);
        tTitle.setCharacterSize(BODY_FONT_SIZE);
        tTitle.setFillColor(themes[0].color_text);
        tTitle.setPosition(sf::Vector2f(startX, currentY));

        float titleWidth = tTitle.getGlobalBounds().size.x;

        sf::Text tBody(font2);
        tBody.setCharacterSize(BODY_FONT_SIZE);
        tBody.setFillColor(themes[0].color_text);

        std::string padding = "\t";
        sf::Text tempMeasurement = tBody;

        do {
            padding += " ";
            tempMeasurement.setString(padding);
        } while (tempMeasurement.getGlobalBounds().size.x < titleWidth);

        std::string fullBodyString = padding + regText;

        tBody.setString(fullBodyString);
        wrapText(tBody, maxWidth);
        tBody.setPosition(sf::Vector2f(startX, currentY));

        pageContent.push_back(tBody);
        pageContent.push_back(tTitle);

        currentY += tBody.getGlobalBounds().size.y + 20.f;
    }

    void page1() {
        float startX, currentY, maxWidth;
        getPageLayout(startX, currentY, maxWidth);
        float initialY = currentY;

        addBlock(GetTranslation(TR_LEARN_PAGE1_INTRO), BODY_FONT_SIZE, themes[0].color_text, startX, currentY, maxWidth);

        currentY += 40.f;
        addBlock(GetTranslation(TR_LEARN_PAGE1_OBJECTIVE_TITLE), HEADER_FONT_SIZE, themes[0].color_text, startX, currentY, maxWidth);

        addBlock(GetTranslation(TR_LEARN_PAGE1_OBJECTIVE), BODY_FONT_SIZE, themes[0].color_text, startX, currentY, maxWidth);

        currentY += 40.f;
        addBlock(GetTranslation(TR_LEARN_PAGE1_SETUP_TITLE), HEADER_FONT_SIZE, themes[0].color_text, startX, currentY, maxWidth);

        addBlock(GetTranslation(TR_LEARN_PAGE1_SETUP), BODY_FONT_SIZE, themes[0].color_text, startX, currentY, maxWidth);

        centerContent(initialY, currentY);
    }

    void page2() {
        float startX, currentY, maxWidth;
        getPageLayout(startX, currentY, maxWidth);
        float initialY = currentY;

        addBlock(GetTranslation(TR_LEARN_PAGE2_TITLE), HEADER_FONT_SIZE, themes[0].color_text, startX, currentY, maxWidth);

        addBlock(GetTranslation(TR_LEARN_PAGE2_MOVEMENT), BODY_FONT_SIZE, themes[0].color_text, startX, currentY, maxWidth);

        centerContent(initialY, currentY);
    }

    void page3() {
        float startX, currentY, maxWidth;
        getPageLayout(startX, currentY, maxWidth);
        float initialY = currentY;

        addBlock(GetTranslation(TR_LEARN_PAGE3_TITLE), HEADER_FONT_SIZE, themes[0].color_text, startX, currentY, maxWidth);

        addBlock(GetTranslation(TR_LEARN_PAGE3_CAPTURE), BODY_FONT_SIZE, themes[0].color_text, startX, currentY, maxWidth);

        centerContent(initialY, currentY);
    }

    void page4() {
        float startX, currentY, maxWidth;
        getPageLayout(startX, currentY, maxWidth);
        float initialY = currentY;

        addBlock(GetTranslation(TR_LEARN_PAGE4_TITLE), HEADER_FONT_SIZE, themes[0].color_text, startX, currentY, maxWidth);

        addBlock(GetTranslation(TR_LEARN_PAGE4_RULES), BODY_FONT_SIZE, themes[0].color_text, startX, currentY, maxWidth);

        centerContent(initialY, currentY);
    }

    void page5() {
        float startX, currentY, maxWidth;
        getPageLayout(startX, currentY, maxWidth);
        float initialY = currentY;

        addBlock(GetTranslation(TR_LEARN_PAGE5_TITLE), HEADER_FONT_SIZE, themes[0].color_text, startX, currentY, maxWidth);

        addBlock(GetTranslation(TR_LEARN_PAGE5_END), BODY_FONT_SIZE, themes[0].color_text, startX, currentY, maxWidth);

        addMixedBlock(GetTranslation(TR_LEARN_PAGE5_VICTORY), GetTranslation(TR_LEARN_PAGE5_VICTORY_DESC), startX, currentY, maxWidth);

        addMixedBlock(GetTranslation(TR_LEARN_PAGE5_SCORE), GetTranslation(TR_LEARN_PAGE5_SCORE_DESC), startX, currentY, maxWidth);

        centerContent(initialY, currentY);
    }

    void loadPage(int page) {
        if (pageSprite.has_value() && (page - 1 < pageTextures.size())) {
            pageSprite->setTexture(pageTextures[page - 1], true);
        }

        if (page == 1) page1();
        else if (page == 2) page2();
        else if (page == 3) page3();
        else if (page == 4) page4();
        else if (page == 5) page5();

        currentTxt.setString(std::to_string(currentPageNum) + "/5");

        sf::FloatRect textRect = currentTxt.getLocalBounds();
        currentTxt.setOrigin(sf::Vector2f(
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
        ));

        sf::FloatRect btnBounds = currentPage.getGlobalBounds();
        currentTxt.setPosition(sf::Vector2f(
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

                    currentPageNum = 1;
                    loadPage(1);
                }
                else if (nextPage.getGlobalBounds().contains(mousePosF)) {
                    audio.playClick();
                    currentPageNum++;
                    if (currentPageNum > 5) currentPageNum = 1;
                    loadPage(currentPageNum);
                }
                else if (previousPage.getGlobalBounds().contains(mousePosF)) {
                    audio.playClick();
                    currentPageNum--;
                    if (currentPageNum < 1) currentPageNum = 5;
                    loadPage(currentPageNum);
                }
            }
        }
    }

    void update(const sf::Vector2f& mousePosF, sf::RenderWindow& window) {
        bool isHovering = false;

        if (back.getGlobalBounds().contains(mousePosF)) {
            back.setFillColor(themes[0].color_HoverButton);
            isHovering = true;
        }
        else if (previousPage.getGlobalBounds().contains(mousePosF)) {
            previousPage.setFillColor(themes[0].color_HoverButton);
            isHovering = true;
        }
        else if (nextPage.getGlobalBounds().contains(mousePosF)) {
            nextPage.setFillColor(themes[0].color_HoverButton);
            isHovering = true;
        }
        else {
            back.setFillColor(themes[0].color_buttons);
            previousPage.setFillColor(themes[0].color_buttons);
            nextPage.setFillColor(themes[0].color_buttons);
        }

        backTxt.setFillColor(themes[0].color_text);

        if (isHovering && cursorHand.has_value()) {
            window.setMouseCursor(*cursorHand);
        }
        else if (cursorArrow.has_value()) {
            window.setMouseCursor(*cursorArrow);
        }
    }

    void updateTexts() {
        backTxt.setString(GetTranslation(TR_LEARN_BACK));
        sf::FloatRect textRect = backTxt.getLocalBounds();
        backTxt.setOrigin(sf::Vector2f(
            textRect.position.x + textRect.size.x / 2.0f,
            textRect.position.y + textRect.size.y / 2.0f
        ));
        sf::FloatRect btnBounds = back.getGlobalBounds();
        backTxt.setPosition(sf::Vector2f(
            btnBounds.position.x + btnBounds.size.x / 2.0f,
            btnBounds.position.y + btnBounds.size.y / 2.0f
        ));
        
        loadPage(currentPageNum);
    }

    void updateColors() {
        sf::Color txtColor = themes[0].color_text;

        previousTxt.setFillColor(txtColor);
        nextTxt.setFillColor(txtColor);
        currentTxt.setFillColor(txtColor);

        for (auto& text : pageContent) {
            text.setFillColor(txtColor);
        }
    }

    void draw(sf::RenderWindow& window) {
        // DETECT THEME OR LANGUAGE CHANGE
        if (themes[0].color_bg != lastThemeBgColor || language != lastLanguage) {
            loadPage(currentPageNum); // Re-generate all text with new colors/language
            lastThemeBgColor = themes[0].color_bg; // Update tracker
            lastLanguage = language; // Update language tracker
            updateTexts(); // Update button texts
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

        update(mousePosF, window);
        updateColors();

        window.draw(back);
        window.draw(backTxt);
        window.draw(previousPage);
        window.draw(nextPage);
        window.draw(currentPage);
        window.draw(previousTxt);
        window.draw(currentTxt);
        window.draw(nextTxt);

        window.draw(FirstPage);

        if (pageSprite.has_value()) {
            window.draw(*pageSprite);
        }

        for (const auto& txt : pageContent) {
            window.draw(txt);
        }
    }
};