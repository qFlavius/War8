#ifndef WAR8_GAMECONFIG_HPP
#define WAR8_GAMECONFIG_HPP

#include <string>

/*
  Tipuri simple pentru configurarea jocului.
  Folosesc doar enum-uri si struct-uri, fara clase.
*/

enum class GameMode {
    PvP, // Player vs Player
    PvC, // Player vs Computer
    CvC  // Computer vs Computer
};

enum class Difficulty {
    Easy,
    Hard
};

struct PlayerInfo {
    std::string name;
    bool isComputer = false;
    Difficulty diff = Difficulty::Easy;
};

struct GameConfig {
    GameMode mode = GameMode::PvP;

    PlayerInfo white;
    PlayerInfo black;

    bool useTurnTimer = false;
    int turnTimeSeconds = 0; // MM*60+SS (valid doar daca useTurnTimer=true)
};

#endif // WAR8_GAMECONFIG_HPP
