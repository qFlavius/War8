#ifndef WAR8_AIPLAYER_HPP
#define WAR8_AIPLAYER_HPP

#include <cstdint>
#include "../GameConfig.hpp"

/*
  Mutare simpla: (fr,fc) -> (tr,tc)
  fr, fc = pozitia initiala (linie, coloana)
  tr, tc = pozitia finala (linie, coloana)
*/
struct AIMove {
    int fr = 0;
    int fc = 0;
    int tr = 0;
    int tc = 0;
    bool valid = false;
};

/*
  Seteaza valoarea initiala (seed) folosita de generatorul de numere pseudo-aleatoare.
  Daca nu este apelata, se foloseste auto-initializare la primul apel (din timp).
*/
void AI_Seed(std::uint32_t seed);

/*
  Alege o mutare pentru "side" (1 = alb, 2 = negru).
  - board: matrice 8x8 cu 0 gol, 1 alb, 2 negru
  - ply: numarul de mutari deja facute (half-moves)
  - captureAfterPly: dupa cate mutari incep capturile (ex: 8)
  - diff:
      Easy = aleator, dar cu reguli simple (simulare 1 pas, penalizare autoblocare)
      Hard = cautare minimax + evaluare (cu buget de timp, dar stabil)

  Returneaza AIMove.valid=false daca nu exista mutari legale.
*/
AIMove AI_ChooseMove(const std::uint8_t board[8][8],
    std::uint8_t side,
    int ply,
    int captureAfterPly,
    Difficulty diff);

#endif // WAR8_AIPLAYER_HPP
