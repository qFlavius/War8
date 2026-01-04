#include "AIPlayer.hpp"

#include <cstring>                // std::memcpy
#include <SFML/System/Clock.hpp>  // sf::Clock
#include <SFML/System/Time.hpp>   // sf::Time

// ------------------------------------------------------------
// Generator de numere pseudo-aleatoare (generator liniar congruential)
// ------------------------------------------------------------

static std::uint32_t g_aiSeed = 0x12345678u;
static bool g_aiSeeded = false;

/*
  Seteaza valoarea initiala (seed) folosita de generatorul de numere pseudo-aleatoare.
*/
void AI_Seed(std::uint32_t seed) {
    if (seed == 0) seed = 1;
    g_aiSeed = seed;
    g_aiSeeded = true;
}

static void ensureSeeded() {
    static sf::Clock autoClock;
    if (g_aiSeeded) return;

    // folosesc timpul scurs (microsecunde) ca seed, ca sa nu fie acelasi autoplay
    std::uint32_t s = (std::uint32_t)autoClock.getElapsedTime().asMicroseconds();
    if (s == 0) s = 1;
    AI_Seed(s);
}

/*
  Intoarce urmatorul numar pseudo-aleator folosind un "generator liniar congruential".

  Formula generala (matematica) este:
      X_{n+1} = (a * X_n + c) mod m

  In implementarea aceasta:
      - X_n este starea curenta g_aiSeed (un numar pe 32 de biti)
      - a = 1664525
      - c = 1013904223
      - m = 2^32

  Observatie:
  Nu scriu explicit "mod 2^32" pentru ca tipul este std::uint32_t.
  Cand depasesc 32 de biti, se pastreaza automat doar ultimii 32 de biti,
  ceea ce este echivalent cu operatia "mod 2^32".
*/
static std::uint32_t nextPseudoRandom() {
    g_aiSeed = g_aiSeed * 1664525u + 1013904223u;
    return g_aiSeed;
}

// ------------------------------------------------------------
// Helpers tabla / mutari
// ------------------------------------------------------------

static std::uint8_t otherSide(std::uint8_t s) {
    return (s == 1) ? 2 : 1;
}

static bool inside(int r, int c) {
    return (r >= 0 && r < 8 && c >= 0 && c < 8);
}

static int countPieces(const std::uint8_t b[8][8], std::uint8_t who) {
    int cnt = 0;
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            if (b[r][c] == who) cnt++;
    return cnt;
}

/*
  Genereaza mutarile posibile pentru o piesa la 1 pas diagonal (4 directii),
  doar in celule libere.
*/
static int genMovesForPiece(const std::uint8_t b[8][8],
    int r, int c,
    AIMove outMoves[], int outMax, int outStart) {
    int k = outStart;

    static const int dr[4] = { -1, -1, +1, +1 };
    static const int dc[4] = { -1, +1, -1, +1 };

    for (int i = 0; i < 4; i++) {
        int nr = r + dr[i];
        int nc = c + dc[i];
        if (!inside(nr, nc)) continue;
        if (b[nr][nc] != 0) continue;

        if (k < outMax) {
            outMoves[k].fr = r;
            outMoves[k].fc = c;
            outMoves[k].tr = nr;
            outMoves[k].tc = nc;
            outMoves[k].valid = true;
            k++;
        }
    }
    return k;
}

static int collectMoves(const std::uint8_t b[8][8],
    std::uint8_t who,
    AIMove outMoves[], int outMax) {
    int k = 0;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (b[r][c] != who) continue;
            k = genMovesForPiece(b, r, c, outMoves, outMax, k);
        }
    }
    return k;
}

static int mobility(const std::uint8_t b[8][8], std::uint8_t who) {
    AIMove tmp[256];
    return collectMoves(b, who, tmp, 256);
}

/*
  Numara cate piese ale jucatorului "who" sunt blocate (nu au nici o mutare legala).
*/
static int countBlockedPieces(const std::uint8_t b[8][8], std::uint8_t who) {
    int cnt = 0;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (b[r][c] != who) continue;
            AIMove tmp[8];
            int m = genMovesForPiece(b, r, c, tmp, 8, 0);
            if (m == 0) cnt++;
        }
    }
    return cnt;
}

/*
  Eliminare piese blocate (conform regulii corecte):
  - dupa ce un jucator muta, se elimina automat DOAR piesele ADVERSARULUI care sunt blocate
  - eliminarea se aplica simultan (mai intai marchez, apoi sterg)
*/
static void removeBlockedOpponent(std::uint8_t b[8][8], std::uint8_t mover) {
    std::uint8_t opp = otherSide(mover);
    bool kill[8][8] = {};

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (b[r][c] != opp) continue;

            AIMove tmp[8];
            int m = genMovesForPiece(b, r, c, tmp, 8, 0);
            if (m == 0) kill[r][c] = true;
        }
    }

    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            if (kill[r][c]) b[r][c] = 0;
}

static void applyMove(std::uint8_t b[8][8],
    const AIMove& mv,
    int newPly,
    int captureAfterPly) {
    std::uint8_t p = b[mv.fr][mv.fc];
    b[mv.fr][mv.fc] = 0;
    b[mv.tr][mv.tc] = p;

    if (newPly >= captureAfterPly) {
        removeBlockedOpponent(b, p);
    }
}

/*
  Distanta simpla fata de zona centrala (folosita ca bonus).
*/
static int centerDist(int r, int c) {
    int dr = (r < 4) ? (3 - r) : (r - 4);
    int dc = (c < 4) ? (3 - c) : (c - 4);
    return dr + dc;
}

// ------------------------------------------------------------
// Easy: aleator cu reguli simple (simulare 1 pas)
// ------------------------------------------------------------

static AIMove chooseMoveEasySmart(const std::uint8_t board[8][8],
    std::uint8_t side,
    int ply,
    int captureAfterPly) {
    AIMove out{};
    out.valid = false;

    AIMove moves[256];
    int n = collectMoves(board, side, moves, 256);
    if (n == 0) return out;

    const int INF = 2000000000;

    int bestScore = -INF;
    int bestIdx[256];
    int bestCount = 0;

    std::uint8_t opp = otherSide(side);

    for (int i = 0; i < n; i++) {
        std::uint8_t nb[8][8];
        std::memcpy(nb, board, sizeof(nb));

        applyMove(nb, moves[i], ply + 1, captureAfterPly);

        int myP = countPieces(nb, side);
        int opP = countPieces(nb, opp);

        int myM = mobility(nb, side);
        int opM = mobility(nb, opp);

        // Penalizare daca piesa mutata ramane fara mutari legale (autoblocare)
        int blockedPenalty = 0;
        {
            AIMove tmp[8];
            int m = genMovesForPiece(nb, moves[i].tr, moves[i].tc, tmp, 8, 0);
            if (m == 0) blockedPenalty = 2000;
        }

        int centerBonus = 20 - centerDist(moves[i].tr, moves[i].tc);
        if (centerBonus < 0) centerBonus = 0;

        int score =
            (myP - opP) * 1200 +
            (myM - opM) * 10 +
            centerBonus -
            blockedPenalty;

        // zgomot mic ca sa nu fie perfect determinist in egalitati
        score += (int)(nextPseudoRandom() & 3u);

        if (score > bestScore) {
            bestScore = score;
            bestIdx[0] = i;
            bestCount = 1;
        }
        else if (score == bestScore) {
            bestIdx[bestCount++] = i;
        }
    }

    int pick = 0;
    if (bestCount > 1) {
        pick = (int)(nextPseudoRandom() % (std::uint32_t)bestCount);
    }

    out = moves[bestIdx[pick]];
    out.valid = true;
    return out;
}

// ------------------------------------------------------------
// Hard: minimax alpha-beta cu oprire pe buget de timp
// ------------------------------------------------------------

static int evalBoard(const std::uint8_t b[8][8],
    std::uint8_t rootSide,
    int ply,
    int captureAfterPly) {
    std::uint8_t opp = otherSide(rootSide);

    int myP = countPieces(b, rootSide);
    int opP = countPieces(b, opp);

    // stari terminale: daca unul ramane fara piese
    if (opP == 0) return +1000000;
    if (myP == 0) return -1000000;

    int myM = mobility(b, rootSide);
    int opM = mobility(b, opp);

    // piese blocate (important mai ales dupa activarea capturii)
    int myBlocked = countBlockedPieces(b, rootSide);
    int opBlocked = countBlockedPieces(b, opp);

    int center = 0;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            int dist = centerDist(r, c);
            if (b[r][c] == rootSide) center -= dist;
            else if (b[r][c] == opp)  center += dist;
        }
    }

    int blockedWeight = (ply >= captureAfterPly) ? 500 : 120;

    int score =
        (myP - opP) * 2000 +
        (myM - opM) * 25 +
        (opBlocked - myBlocked) * blockedWeight +
        center;

    return score;
}

static int minimaxTimed(std::uint8_t b[8][8],
    std::uint8_t toMove,
    int ply,
    int captureAfterPly,
    int depthLeft,
    std::uint8_t rootSide,
    int alpha,
    int beta,
    const sf::Clock& timer,
    int budgetMillis) {
    if ((int)timer.getElapsedTime().asMilliseconds() >= budgetMillis) {
        return evalBoard(b, rootSide, ply, captureAfterPly);
    }

    if (depthLeft <= 0) {
        return evalBoard(b, rootSide, ply, captureAfterPly);
    }

    AIMove moves[256];
    int n = collectMoves(b, toMove, moves, 256);

    if (n == 0) {
        // daca jucatorul curent nu are mutari, e rau pentru el
        if (toMove == rootSide) return -1000000;
        return +1000000;
    }

    const int INF = 2000000000;
    bool isMax = (toMove == rootSide);

    if (isMax) {
        int best = -INF;
        for (int i = 0; i < n; i++) {
            if ((int)timer.getElapsedTime().asMilliseconds() >= budgetMillis) break;

            std::uint8_t nb[8][8];
            std::memcpy(nb, b, sizeof(nb));
            applyMove(nb, moves[i], ply + 1, captureAfterPly);

            int val = minimaxTimed(nb, otherSide(toMove), ply + 1, captureAfterPly,
                depthLeft - 1, rootSide, alpha, beta,
                timer, budgetMillis);

            if (val > best) best = val;
            if (best > alpha) alpha = best;
            if (beta <= alpha) break;
        }
        return best;
    }
    else {
        int best = +INF;
        for (int i = 0; i < n; i++) {
            if ((int)timer.getElapsedTime().asMilliseconds() >= budgetMillis) break;

            std::uint8_t nb[8][8];
            std::memcpy(nb, b, sizeof(nb));
            applyMove(nb, moves[i], ply + 1, captureAfterPly);

            int val = minimaxTimed(nb, otherSide(toMove), ply + 1, captureAfterPly,
                depthLeft - 1, rootSide, alpha, beta,
                timer, budgetMillis);

            if (val < best) best = val;
            if (best < beta) beta = best;
            if (beta <= alpha) break;
        }
        return best;
    }
}

static AIMove chooseMoveHardTimed(const std::uint8_t board[8][8],
    std::uint8_t side,
    int ply,
    int captureAfterPly) {
    AIMove out{};
    out.valid = false;

    AIMove moves[256];
    int n = collectMoves(board, side, moves, 256);
    if (n == 0) return out;

    int totalPieces = countPieces(board, 1) + countPieces(board, 2);

    // buget de timp mai generos ca sa fie "hard" cu adevarat, dar fara lag mare
    int budgetMillis = (totalPieces >= 14) ? 90 : 140;
    int maxDepth = (totalPieces >= 14) ? 7 : 9;

    const int INF = 2000000000;

    // iterative deepening stabil:
    // pastrez cea mai buna mutare de la ultima adancime COMPLETA
    int lastCompleteBestIdx = 0;
    int lastCompleteBestVal = -INF;

    sf::Clock timer;

    for (int depth = 2; depth <= maxDepth; depth++) {
        if ((int)timer.getElapsedTime().asMilliseconds() >= budgetMillis) break;

        int bestValThisDepth = -INF;
        int bestIdxThisDepth = lastCompleteBestIdx;

        bool completed = true;

        for (int i = 0; i < n; i++) {
            if ((int)timer.getElapsedTime().asMilliseconds() >= budgetMillis) {
                completed = false;
                break;
            }

            std::uint8_t nb[8][8];
            std::memcpy(nb, board, sizeof(nb));
            applyMove(nb, moves[i], ply + 1, captureAfterPly);

            int val = minimaxTimed(nb, otherSide(side), ply + 1, captureAfterPly,
                depth - 1, side, -INF, +INF,
                timer, budgetMillis);

            if (val > bestValThisDepth) {
                bestValThisDepth = val;
                bestIdxThisDepth = i;
            }
        }

        if (!completed) break;

        lastCompleteBestIdx = bestIdxThisDepth;
        lastCompleteBestVal = bestValThisDepth;

        // mic bonus: daca adancimea a fost complet calculata, incerc sa continui
        // (nu e nevoie sa folosesc lastCompleteBestVal aici)
        (void)lastCompleteBestVal;
    }

    out = moves[lastCompleteBestIdx];
    out.valid = true;
    return out;
}

AIMove AI_ChooseMove(const std::uint8_t board[8][8],
    std::uint8_t side,
    int ply,
    int captureAfterPly,
    Difficulty diff) {
    ensureSeeded();

    AIMove out{};
    out.valid = false;

    if (side != 1 && side != 2) return out;

    if (diff == Difficulty::Easy) {
        return chooseMoveEasySmart(board, side, ply, captureAfterPly);
    }

    return chooseMoveHardTimed(board, side, ply, captureAfterPly);
}
