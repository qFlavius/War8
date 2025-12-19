#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>

struct AudioMenu {
    sf::Font font;

    sf::SoundBuffer clickBuffer, clickBuffer2;
    std::optional<sf::Sound> clickSound,keyboardSound;

    AudioMenu() {
        if (!clickBuffer.loadFromFile("SoundEffects/click.wav")) {
            std::cerr << "Eroare la incarcarea sunetului!" << std::endl;
        }
        else {
            clickSound.emplace(clickBuffer);
            clickSound->setVolume(50.f);
            clickSound->setPitch(1.0f);
        }
        if (!clickBuffer2.loadFromFile("SoundEffects/KeyBoardKey.wav")) {
            std::cerr << "Eroare la incarcarea sunetului!" << std::endl;
        }
        else {
            keyboardSound.emplace(clickBuffer2);
            keyboardSound->setVolume(200.f);
            keyboardSound->setPitch(1.0f);
        }
    }

    void playClick() {
        if (clickSound.has_value()) {
            clickSound->stop();
            clickSound->play();
        }
    }
    void playKey() {
        if (clickSound.has_value()) {
            keyboardSound->stop();
            keyboardSound->play();
        }
    }
};