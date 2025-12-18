#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include "../Themes/themesRead.h"
#include "../globalVar.hpp"

inline void playerData() {
	std::ifstream fin("Profile/playerData.txt");
    if (!fin.is_open()) return;

    std::string key, value;

    while (fin >> key >> value) {
        for (auto& c : key) c = tolower(c);

        if (key == "theme:")
            themeAdress[0] = value;
    }

}