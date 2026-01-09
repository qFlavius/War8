#pragma once
#include <SFML/Graphics.hpp>
#include <optional> 
#include <vector>
#include <string>
#include <iostream>
#include "../Themes/themesRead.h"
#include "../globalVar.hpp"
#include "../SoundEffects/SoundEffects.hpp"
#include "../Profile/playerData.hpp" 

struct SettingsMenu {
    AudioMenu audio;

    const float SCREEN_W = 1920.f;
    const float SCREEN_H = 1080.f;

    sf::Font font;
    sf::RectangleShape back, div1;

    // RENAMED BUTTONS (Fixes Shadowing Error)
    sf::RectangleShape fpsBtn, fpsNR;
    sf::Text fpsTxt, fpsNRTxt;

    sf::RectangleShape soundBtn, soundNR;
    sf::Text soundTxt, soundNRTxt;

    sf::RectangleShape musicBtn, musicNR;
    sf::Text musicTxt, musicNRTxt;

    sf::RectangleShape resetBtn;
    sf::Text resetTxt;
    sf::Text backTxt;

    std::vector<unsigned int> framerates = { 60, 120, 144, 240, 30 };
    int fpsIndex = 0;

    bool soundOn = true;
    bool musicOn = true;

    std::optional<sf::Cursor> cursorHand;
    std::optional<sf::Cursor> cursorArrow;

    SettingsMenu() :
        backTxt(font),
        fpsTxt(font), fpsNRTxt(font),
        soundTxt(font), soundNRTxt(font),
        musicTxt(font), musicNRTxt(font),
        resetTxt(font)
    {
        if (!font.openFromFile("Themes/Kanit-Medium.ttf")) {
            std::cerr << "Error loading font" << std::endl;
        }

        // Initialize state from GLOBALS
        if (soundEffects == "OFF") soundOn = false; else soundOn = true;
        if (Music == "OFF") musicOn = false; else musicOn = true;

        for (size_t i = 0; i < framerates.size(); ++i) {
            if (std::to_string(framerates[i]) == fps) {
                fpsIndex = static_cast<int>(i);
                break;
            }
        }

        button(back, backTxt, "Back", sf::Vector2f(543.f, 108.f), sf::Vector2f(0.f, SCREEN_H), sf::Vector2f(0.f, 108.f));
        div(div1, sf::Vector2f(650.f, 800.f), sf::Vector2f(SCREEN_W / 2.f, SCREEN_H / 2.f - 54.f), sf::Vector2f(650.f / 2.f, 800.f / 2.f));

        // SFML 3.0 FIX: Use position/size
        sf::FloatRect divBounds = div1.getGlobalBounds();
        float startX = divBounds.position.x;
        float startY = divBounds.position.y;
        float rightX = SCREEN_W - startX;

        float btnWidthLabel = 543.f;
        float btnWidthValue = 650.f - 543.f;
        float btnHeight = 108.f;
        float gap = 20.f;

        // FPS
        button(fpsBtn, fpsTxt, "FPS", sf::Vector2f(btnWidthLabel, btnHeight), sf::Vector2f(startX, startY), sf::Vector2f(0.f, 0.f));
        button(fpsNR, fpsNRTxt, std::to_string(framerates[fpsIndex]), sf::Vector2f(btnWidthValue, btnHeight), sf::Vector2f(rightX, startY), sf::Vector2f(btnWidthValue, 0.f));
        fpsNR.setFillColor(themes[0].color_HoverButton);

        // Sound
        float row2Y = startY + btnHeight + gap;
        button(soundBtn, soundTxt, "Sound Effects", sf::Vector2f(btnWidthLabel, btnHeight), sf::Vector2f(startX, row2Y), sf::Vector2f(0.f, 0.f));
        button(soundNR, soundNRTxt, soundOn ? "ON" : "OFF", sf::Vector2f(btnWidthValue, btnHeight), sf::Vector2f(rightX, row2Y), sf::Vector2f(btnWidthValue, 0.f));
        soundNR.setFillColor(sf::Color::Green);

        // Music
        float row3Y = row2Y + btnHeight + gap;
        button(musicBtn, musicTxt, "Music", sf::Vector2f(btnWidthLabel, btnHeight), sf::Vector2f(startX, row3Y), sf::Vector2f(0.f, 0.f));
        button(musicNR, musicNRTxt, musicOn ? "ON" : "OFF", sf::Vector2f(btnWidthValue, btnHeight), sf::Vector2f(rightX, row3Y), sf::Vector2f(btnWidthValue, 0.f));
        musicNR.setFillColor(sf::Color::Green);

        // Reset
        float divBottomY = divBounds.position.y + divBounds.size.y;
        float divCenterX = divBounds.position.x + divBounds.size.x / 2.0f;
        button(resetBtn, resetTxt, "Reset", sf::Vector2f(200.f, 80.f), sf::Vector2f(divCenterX, divBottomY - 60.f), sf::Vector2f(100.f, 40.f));

        cursorHand = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
        cursorArrow = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);
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

        // SFML 3.0 FIX: Use position/size
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

    void updateTextCenter(sf::RectangleShape& shape, sf::Text& text) {
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

    void saveAndSync() {
        fps = std::to_string(framerates[fpsIndex]);
        soundEffects = soundOn ? "ON" : "OFF";
        Music = musicOn ? "ON" : "OFF";
        SavePlayerData();
    }

    void handleInput(const sf::Event& event, sf::RenderWindow& window) {
        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

                if (back.getGlobalBounds().contains(mousePosF)) {
                    saveAndSync();
                    audio.playClick(); 
                    interfata = 1;
                }
                else if (fpsNR.getGlobalBounds().contains(mousePosF)) {
                    fpsIndex++;
                    if (fpsIndex >= framerates.size()) fpsIndex = 0;
                    unsigned int newLimit = framerates[fpsIndex];
                    window.setFramerateLimit(newLimit);
                    fpsNRTxt.setString(std::to_string(newLimit));
                    updateTextCenter(fpsNR, fpsNRTxt);
                    
                    saveAndSync();
                    audio.playClick();
                }
                else if (soundNR.getGlobalBounds().contains(mousePosF)) {
                    soundOn = !soundOn;
                    soundNRTxt.setString(soundOn ? "ON" : "OFF");
                    updateTextCenter(soundNR, soundNRTxt);
                    
                    saveAndSync();
                    audio.playClick();
                }
                else if (musicNR.getGlobalBounds().contains(mousePosF)) {
                    musicOn = !musicOn;
                    musicNRTxt.setString(musicOn ? "ON" : "OFF");
                    updateTextCenter(musicNR, musicNRTxt);

                    // Music Control Logic
                    if (musicOn) {
                        if (bgMusic.getStatus() != sf::Music::Status::Playing) bgMusic.play();
                    } else {
                        bgMusic.pause();
                    }

                    saveAndSync();
                    audio.playClick();
                }
                else if (resetBtn.getGlobalBounds().contains(mousePosF)) {
                    fpsIndex = 0;
                    window.setFramerateLimit(framerates[fpsIndex]);
                    fpsNRTxt.setString(std::to_string(framerates[fpsIndex]));
                    updateTextCenter(fpsNR, fpsNRTxt);

                    soundOn = true;
                    soundNRTxt.setString("ON");
                    updateTextCenter(soundNR, soundNRTxt);

                    musicOn = true;
                    musicNRTxt.setString("ON");
                    updateTextCenter(musicNR, musicNRTxt);
                    if (bgMusic.getStatus() != sf::Music::Status::Playing) bgMusic.play();

                    saveAndSync();
                    audio.playClick();
                }
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

        bool isHovering = false;

        // Hover logic using new button names
        if (back.getGlobalBounds().contains(mousePosF)) { back.setFillColor(themes[0].color_HoverButton); isHovering = true; }
        else back.setFillColor(themes[0].color_buttons);

        if (fpsNR.getGlobalBounds().contains(mousePosF)) { fpsNR.setFillColor(themes[0].color_buttons); isHovering = true; }
        else fpsNR.setFillColor(themes[0].color_HoverButton);

        if (soundNR.getGlobalBounds().contains(mousePosF)) isHovering = true;
        soundNR.setFillColor(soundOn ? sf::Color::Green : sf::Color::Red);

        if (musicNR.getGlobalBounds().contains(mousePosF)) isHovering = true;
        musicNR.setFillColor(musicOn ? sf::Color::Green : sf::Color::Red);

        if (resetBtn.getGlobalBounds().contains(mousePosF)) { resetBtn.setFillColor(themes[0].color_HoverButton); isHovering = true; }
        else resetBtn.setFillColor(themes[0].color_buttons);

        if (isHovering && cursorHand.has_value()) window.setMouseCursor(*cursorHand);
        else if (cursorArrow.has_value()) window.setMouseCursor(*cursorArrow);

        window.draw(back); window.draw(backTxt);
        window.draw(div1);
        window.draw(fpsBtn); window.draw(fpsTxt);
        window.draw(fpsNR); window.draw(fpsNRTxt);
        window.draw(soundBtn); window.draw(soundTxt);
        window.draw(soundNR); window.draw(soundNRTxt);
        window.draw(musicBtn); window.draw(musicTxt);
        window.draw(musicNR); window.draw(musicNRTxt);
        window.draw(resetBtn); window.draw(resetTxt);
    }
};