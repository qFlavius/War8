#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include "../Themes/themesRead.h"
#include "../globalVar.hpp"

inline void LoadPlayerData() {
    std::ifstream fin("Profile/playerData.txt");

    // Daca fisierul nu exista, nu facem nimic (ramane default)
    if (!fin.is_open()) return;

    std::string key, value;
    while (fin >> key >> value) {
        for (auto& c : key) c = tolower(c);

        if (key == "theme:") {
            themeAdress[0] = value;
        }
    }
    fin.close();
    std::cout << "[INFO] Date jucator incarcate: " << themeAdress[0] << std::endl;
}

inline void SavePlayerData() {
    std::ofstream fout("Profile/playerData.txt");

    if (!fout.is_open()) {
        std::cerr << "[EROARE] Nu pot salva in Profile/playerData.txt" << std::endl;
        return;
    }

    fout << "theme: " << themeAdress[0] << std::endl;
    fout.close();
    std::cout << "[INFO] Tema salvata: " << themeAdress[0] << std::endl;
}