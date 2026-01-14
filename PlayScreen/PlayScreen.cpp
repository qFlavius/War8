#include "PlayScreen.hpp"

#include <cstdint>
#include <cstdio>                // snprintf
#include <fstream>               // Added for file I/O
#include <map>                   // Added for leaderboard sorting
#include <string>                // Added for string manipulation
#include <algorithm>             // Added for std::replace
#include <optional>              // Added for cursors
#include <SFML/System/Angle.hpp> // sf::degrees (SFML 3)
#include "../SoundEffects/SoundEffects.hpp"
#include "../GameConfig.hpp"
#include "../AI/AIPlayer.hpp"
#include "../Themes/themesRead.h" // Added to access themes[0]

// ============================================================================
//  PLAYSCREEN
//  - Fluxul in Play:
//      0) Aleg modul de joc (Player vs Player / Player vs Computer / Computer vs Computer)
//      1) Introduc numele (doar pentru jucatorii umani)
//      2) Optiuni (timer pe tura + dificultati pentru computer)
//      3) Jocul propriu-zis
//      4) Final: afisez castigatorul + efecte grafice
//
//  Reguli implementate:
//  - Mutare: 1 pas pe diagonala intr-o celula libera.
//  - Dupa un prag de mutari (captureAfterPly), elimin automat piesele blocate
//    (adica fara mutari legale). Asta este captura prin blocare.
//  - Final: cand cineva ramane fara piese / limita de mutari / un jucator nu mai are mutari.
//  - Castigator (normal): cine are mai multe piese ramase.
//  - Timer pe tura (optional): daca timpul expira pentru un jucator UMAN, pierde tura (nu muta nimic).
// ============================================================================


// ------------------------------ STARE JOC ------------------------------

AudioMenu audio;

// faze ecran
static const int PHASE_MODE = 0;
static const int PHASE_NAMES = 1;
static const int PHASE_OPTIONS = 2;
static const int PHASE_GAME = 3;
static const int PHASE_OVER = 4;

// 0 liber, 1 alb, 2 negru
static std::uint8_t g_board[8][8];

// tura curenta: 1 alb, 2 negru
static std::uint8_t g_turn = 1;

// selectie piesa
static bool g_hasSel = false;
static int  g_selR = 0;
static int  g_selC = 0;

// mutari posibile (maxim 4 diagonale)
static int g_movesR[4];
static int g_movesC[4];
static int g_moveCount = 0;

// contor mutari (half-moves) + prag cand incep capturile prin blocare
static int g_ply = 0;
static int g_captureAfterPly = 8;
static int g_maxPly = 200;

// capturi: cate piese adverse au fost eliminate dupa mutarile fiecaruia
static int g_capturedByWhite = 0;
static int g_capturedByBlack = 0;

// timp total scurs in joc
static sf::Clock g_gameClock;

// timer pe tura (doar pentru jucatorii umani)
static sf::Clock g_turnClock;
static bool g_useTurnTimer = false;
static int  g_turnLimitSeconds = 60; // default 01:00
static char g_turnTimeStr[6] = "01:00"; // format MM:SS
static bool g_editTurnTime = false;
static int  g_turnTimeCursor = 0; // 0,1,3,4 (pozitii in string)

// pentru mutari de computer: un mic delay, sa se vada ca "gandeste"
static sf::Clock g_aiClock;
// delay mic, doar ca sa se vada ca "gandeste"; daca e prea mare pare ca se blocheaza
static float g_aiThinkDelay = 0.15f;

// clock separat pentru seed (depinde de cat dureaza pana apesi START)
static sf::Clock g_seedClock;

// configurare (mod + cine e computer + dificultati)
static GameMode g_mode = GameMode::PvP;
static bool g_isComputerWhite = false;
static bool g_isComputerBlack = false;
static Difficulty g_diffWhite = Difficulty::Easy;
static Difficulty g_diffBlack = Difficulty::Easy;

// nume jucatori (char[] ca in codul tau)
static char g_nameWhite[24] = "";
static char g_nameBlack[24] = "";
static int  g_lenWhite = 0;
static int  g_lenBlack = 0;
static int  g_activeName = 0; // 0 = alb, 1 = negru, -1 = nimic editabil

// faza curenta
static int g_phase = PHASE_MODE;

// castigator: 0 remiza, 1 alb, 2 negru
static int g_winner = 0;
static char g_overReason[64] = ""; // motiv final (ex: "Time out")

// Cursors
static std::optional<sf::Cursor> g_cursorHand;
static std::optional<sf::Cursor> g_cursorArrow;

// ------------------------------ TEXT (SFML 3) ------------------------------

static sf::Font g_font;
static bool g_fontReady = false;

// texte generale
static sf::Text g_title(g_font);
static sf::Text g_hint(g_font);

// ecran nume
static sf::Text g_labelWhite(g_font);
static sf::Text g_labelBlack(g_font);
static sf::Text g_nameTextWhite(g_font);
static sf::Text g_nameTextBlack(g_font);

// ecran game hud
static sf::Text g_hudTurn(g_font);
static sf::Text g_hudRemain(g_font);
static sf::Text g_hudLeft(g_font);
static sf::Text g_hudRight(g_font);
static sf::Text g_hudTime(g_font);

// ecran final
static sf::Text g_overTitle(g_font);
static sf::Text g_overWinner(g_font);
static sf::Text g_overHint(g_font);
static sf::Text g_overReasonText(g_font);


// ------------------------------ LAYOUT / RAMA ------------------------------

static float g_tile = 100.f;
static float g_frame = 30.f;
static sf::Vector2f g_origin(0.f, 0.f);


// ------------------------------ CONFETTI (final) ------------------------------

static const int CONF_N = 90;
static sf::Vector2f g_confPos[CONF_N];
static sf::Vector2f g_confVel[CONF_N];
static float g_confSize[CONF_N];
static std::uint32_t g_rng = 1;
static sf::Clock g_overClock;


// ============================================================================
//  Functii mici ajutatoare (logica joc)
// ============================================================================

/*
  Schimba jucatorul: 1 <-> 2.
*/
static std::uint8_t otherSide(std::uint8_t p) {
    return (p == 1) ? 2 : 1;
}

/*
  Verifica daca o parte (alb / negru) este controlata de computer.
*/
static bool isComputerSide(std::uint8_t side) {
    if (side == 1) return g_isComputerWhite;
    if (side == 2) return g_isComputerBlack;
    return false;
}

/*
  Copiere simpla de string fara <cstring>.
*/
static void strCopy(char* dst, int dstMax, const char* src) {
    if (dstMax <= 0) return;
    int i = 0;
    while (src[i] != 0 && i < dstMax - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = 0;
}

/*
  Random simplu (generator liniar congruential).
  Nu este folosit pentru joc, doar pentru efectul vizual confetti.
*/
static std::uint32_t rngNext() {
    g_rng = 1664525u * g_rng + 1013904223u;
    return g_rng;
}
static float rng01() {
    return (rngNext() & 0x00FFFFFFu) / 16777215.0f;
}

/*
  Goleste tabla (toate celulele devin 0)
*/
static void boardClear() {
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            g_board[r][c] = 0;
}

/*
  Pozitie initiala.
  Observatie: randul 0 e sus (8), randul 7 e jos (1).
*/
static void boardSetupInitialPosition() {
    boardClear();

    // ALB (jos)
    for (int c = 0; c < 8; c += 2) g_board[7][c] = 1;
    for (int c = 1; c < 8; c += 2) g_board[6][c] = 1;

    // NEGRU (sus)
    for (int c = 1; c < 8; c += 2) g_board[0][c] = 2;
    for (int c = 0; c < 8; c += 2) g_board[1][c] = 2;
}

/*
  Calculeaza g_origin astfel incat tabla sa fie centrata pe ecran.
*/
static void updateLayout(sf::RenderWindow& w) {
    const auto sz = w.getSize();
    const float boardPx = g_tile * 8.f;

    // Pozitionez tabla centrat pe ecran.
    g_origin.x = (float(sz.x) - boardPx) / 2.f;
    g_origin.y = (float(sz.y) - boardPx) / 2.f;

    // In timpul jocului vreau spatiu deasupra tablei pentru HUD (tura + timp ramas).
    // De aceea imping tabla putin in jos. In restul meniurilor, o las perfect centrat.
    if (g_phase == PHASE_GAME || g_phase == PHASE_OVER) {
        const float headerH = 120.f;
        const float headerGap = 16.f;
        const float minTop = 18.f;
        const float neededTop = minTop + headerH + headerGap + g_frame;

        if (g_origin.y < neededTop) {
            g_origin.y = neededTop;
        }

        // Daca cumva nu mai incape jos, o ridic cat pot.
        const float boardBottom = g_origin.y + boardPx + g_frame;
        const float maxBottom = (float)sz.y - 18.f;
        if (boardBottom > maxBottom) {
            g_origin.y -= (boardBottom - maxBottom);
            if (g_origin.y < neededTop) g_origin.y = neededTop;
        }
    }
}

/*
  Converteste pozitia mouse-ului in (r,c) pe tabla.
  Returneaza false daca mouse-ul nu este pe tabla.
*/
static bool mouseToCell(sf::RenderWindow& w, int& outR, int& outC) {
    sf::Vector2i mp = sf::Mouse::getPosition(w);
    sf::Vector2f m = w.mapPixelToCoords(mp);

    sf::FloatRect br(g_origin, { g_tile * 8.f, g_tile * 8.f });
    if (!br.contains(m)) return false;

    int c = int((m.x - g_origin.x) / g_tile);
    int r = int((m.y - g_origin.y) / g_tile);

    if (r < 0 || r > 7 || c < 0 || c > 7) return false;
    outR = r;
    outC = c;
    return true;
}

/*
  Helper: verifica daca mouse-ul este in dreptunghi (pentru butoane in meniuri).
*/
static bool mouseInRect(sf::RenderWindow& w, const sf::FloatRect& r) {
    sf::Vector2i mp = sf::Mouse::getPosition(w);
    sf::Vector2f m = w.mapPixelToCoords(mp);
    return r.contains(m);
}

/*
  Genereaza mutarile legale pentru piesa din (fromR, fromC).
  Regula: 1 pas pe diagonala intr-o casuta libera, in orice directie.
*/
static int genMoves(int fromR, int fromC, int outR[4], int outC[4]) {
    if (g_board[fromR][fromC] == 0) return 0;

    static const int dr[4] = { -1, -1,  1,  1 };
    static const int dc[4] = { -1,  1, -1,  1 };

    int cnt = 0;
    for (int i = 0; i < 4; i++) {
        int nr = fromR + dr[i];
        int nc = fromC + dc[i];

        if (nr < 0 || nr > 7 || nc < 0 || nc > 7) continue;
        if (g_board[nr][nc] == 0) {
            outR[cnt] = nr;
            outC[cnt] = nc;
            cnt++;
        }
    }
    return cnt;
}

/*
  Verifica daca (r,c) este una dintre mutarile posibile curente.
*/
static bool moveInList(int r, int c) {
    for (int i = 0; i < g_moveCount; i++)
        if (g_movesR[i] == r && g_movesC[i] == c) return true;
    return false;
}

/*
  Numara cate piese are un jucator.
*/
static int countPieces(std::uint8_t side) {
    int cnt = 0;
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            if (g_board[r][c] == side) cnt++;
    return cnt;
}

/*
  Verifica daca un jucator mai are macar o mutare legala.
*/
static bool hasAnyMove(std::uint8_t side) {
    int tmpR[4], tmpC[4];
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (g_board[r][c] == side) {
                if (genMoves(r, c, tmpR, tmpC) > 0) return true;
            }
        }
    }
    return false;
}

/*
  Decide castigatorul simplu: cine are mai multe piese ramase.
*/
static int decideWinnerByPieces() {
    int w = countPieces(1);
    int b = countPieces(2);
    if (w > b) return 1;
    if (b > w) return 2;
    return 0;
}

/*
  Pregateste particulele de confetti.
*/
static void initConfetti(sf::RenderWindow& w) {
    auto sz = w.getSize();
    float W = (float)sz.x;
    float H = (float)sz.y;

    // seed mic ca sa arate diferit la fiecare final
    g_rng = 1u + (std::uint32_t)(g_gameClock.getElapsedTime().asMilliseconds() + 12345);

    for (int i = 0; i < CONF_N; i++) {
        g_confPos[i].x = rng01() * W;
        g_confPos[i].y = rng01() * H * 0.5f;
        g_confVel[i].x = (rng01() - 0.5f) * 40.f;
        g_confVel[i].y = 80.f + rng01() * 180.f;
        g_confSize[i] = 3.f + rng01() * 6.f;
    }

    g_overClock.restart();
}

static void saveWinToLeaderboard(std::string name) {
    if (name.empty()) return;

    std::replace(name.begin(), name.end(), ' ', '_');

    std::map<std::string, int> scores;
    std::ifstream fin("LeaderBoard/leaderboard.txt");
    if (fin.is_open()) {
        std::string n;
        int s;
        while (fin >> n >> s) {
            scores[n] = s;
        }
        fin.close();
    }

    scores[name]++;

    std::ofstream fout("LeaderBoard/leaderboard.txt");
    if (fout.is_open()) {
        for (const auto& pair : scores) {
            fout << pair.first << " " << pair.second << "\n";
        }
        fout.close();
    }
}

/*
  Intra in starea de final (phase=over) cu castigator calculat din piese.
*/
static void enterGameOver(sf::RenderWindow& w) {
    g_phase = PHASE_OVER;
    g_winner = decideWinnerByPieces();
    g_overReason[0] = 0;

    // Daca a castigat un jucator UMAN, salvez victoria
    if (g_winner == 1 && !g_isComputerWhite) {
        saveWinToLeaderboard(g_nameWhite);
    }
    else if (g_winner == 2 && !g_isComputerBlack) {
        saveWinToLeaderboard(g_nameBlack);
    }

    initConfetti(w);
}

/*
  Intra in starea de final (phase=over) cu castigator fortat (ex: time out).
*/
static void enterGameOverForced(sf::RenderWindow& w, int winner, const char* reason) {
    g_phase = PHASE_OVER;
    g_winner = winner;
    strCopy(g_overReason, (int)sizeof(g_overReason), reason);

    // Daca a castigat un jucator UMAN, salvez victoria
    if (g_winner == 1 && !g_isComputerWhite) {
        saveWinToLeaderboard(g_nameWhite);
    }
    else if (g_winner == 2 && !g_isComputerBlack) {
        saveWinToLeaderboard(g_nameBlack);
    }

    initConfetti(w);
}

/*
  Verifica daca meciul s-a terminat.
  Cazuri:
  - cineva are 0 piese
  - am ajuns la limita de mutari
  - unul dintre jucatori nu mai are mutari legale
*/
static void checkGameOver(sf::RenderWindow& w) {
    int wPieces = countPieces(1);
    int bPieces = countPieces(2);

    if (wPieces == 0 || bPieces == 0) { enterGameOver(w); return; }
    if (g_ply >= g_maxPly) { enterGameOver(w); return; }
    if (!hasAnyMove(1) || !hasAnyMove(2)) { enterGameOver(w); return; }
}

/*
  Elimina automat piesele ADVERSARULUI care sunt blocate (fara mutari legale).
  Important:
  - nu elimin piesele jucatorului care tocmai a mutat
  - eliminarea se aplica simultan tuturor pieselor adversarului blocate
  - contorizez capturile pentru jucatorul care a mutat
*/
static void removeBlockedOpponent(std::uint8_t mover) {
    std::uint8_t opp = otherSide(mover);

    int remR[64], remC[64];
    int remCnt = 0;

    int tmpR[4], tmpC[4];

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (g_board[r][c] != opp) continue;

            if (genMoves(r, c, tmpR, tmpC) == 0) {
                remR[remCnt] = r;
                remC[remCnt] = c;
                remCnt++;
            }
        }
    }

    for (int i = 0; i < remCnt; i++) {
        g_board[remR[i]][remC[i]] = 0;
    }

    if (mover == 1) g_capturedByWhite += remCnt;
    else if (mover == 2) g_capturedByBlack += remCnt;
}


/*
  Parseaza stringul MM:SS in secunde.
  Daca valorile sunt invalide, intoarce 0.
*/
static int parseTimeMMSS(const char s[6]) {
    if (s[0] < '0' || s[0] > '9') return 0;
    if (s[1] < '0' || s[1] > '9') return 0;
    if (s[2] != ':') return 0;
    if (s[3] < '0' || s[3] > '9') return 0;
    if (s[4] < '0' || s[4] > '9') return 0;
    int mm = (s[0] - '0') * 10 + (s[1] - '0');
    int ss = (s[3] - '0') * 10 + (s[4] - '0');
    if (mm > 59) mm = 59;
    if (ss > 59) ss = 59;
    return mm * 60 + ss;
}

/*
  Reseteaza meciul (tabla + contori + timer), dar nu schimba config-ul ales.
  Il folosesc pentru "rematch" si pentru start.
*/
static void resetMatchKeepNames() {
    boardSetupInitialPosition();

    g_turn = 1;
    g_hasSel = false;
    g_moveCount = 0;

    g_ply = 0;
    g_capturedByWhite = 0;
    g_capturedByBlack = 0;

    g_gameClock.restart();
    g_turnClock.restart();
    g_aiClock.restart();
}

/*
  Reseteaza toate ecranele din Play si revine la alegerea modului.
*/
static void resetAllToModeSelect() {
    g_phase = PHASE_MODE;

    g_mode = GameMode::PvP;
    g_isComputerWhite = false;
    g_isComputerBlack = false;
    g_diffWhite = Difficulty::Easy;
    g_diffBlack = Difficulty::Easy;

    g_lenWhite = 0; g_nameWhite[0] = 0;
    g_lenBlack = 0; g_nameBlack[0] = 0;
    g_activeName = 0;

    g_useTurnTimer = false;
    g_turnLimitSeconds = 60;
    strCopy(g_turnTimeStr, 6, "01:00");
    g_editTurnTime = false;
    g_turnTimeCursor = 0;

    g_winner = 0;
    g_overReason[0] = 0;

    resetMatchKeepNames();
}

/*
  Alege urmatorul camp editabil pentru nume (sare peste computer).
*/
static int nextEditableNameField(int from) {
    // incerc cealalta parte
    int other = 1 - from;
    if (other == 0 && !g_isComputerWhite) return 0;
    if (other == 1 && !g_isComputerBlack) return 1;
    // daca nici celalalt nu e editabil, incerc pe cel curent
    if (from == 0 && !g_isComputerWhite) return 0;
    if (from == 1 && !g_isComputerBlack) return 1;
    return -1;
}

/*
  Initializeaza modul ales (cine este computer) si pregateste numele.
*/
static void applyModeAndPrepareNames(GameMode mode) {
    g_mode = mode;

    if (mode == GameMode::PvP) {
        g_isComputerWhite = false;
        g_isComputerBlack = false;
    }
    else if (mode == GameMode::PvC) {
        g_isComputerWhite = false;
        g_isComputerBlack = true;
    }
    else {
        g_isComputerWhite = true;
        g_isComputerBlack = true;
    }

    // reset nume la intrarea in ecranul de nume
    g_lenWhite = 0; g_nameWhite[0] = 0;
    g_lenBlack = 0; g_nameBlack[0] = 0;

    // nume default pentru computer
    if (g_isComputerWhite) {
        strCopy(g_nameWhite, 24, "AI1");
        g_lenWhite = 0;
        while (g_nameWhite[g_lenWhite] != 0 && g_lenWhite < 8) g_lenWhite++;
    }
    if (g_isComputerBlack) {
        strCopy(g_nameBlack, 24, "AI2");
        g_lenBlack = 0;
        while (g_nameBlack[g_lenBlack] != 0 && g_lenBlack < 8) g_lenBlack++;
    }

    // activeName: primul camp editabil
    if (!g_isComputerWhite) g_activeName = 0;
    else if (!g_isComputerBlack) g_activeName = 1;
    else g_activeName = -1;
}

/*
  Adauga un caracter in numele ales (0=alb, 1=negru).
  Daca acel jucator este computer, ignor.
*/
static void appendCharToName(int which, char ch) {
    if (which == 0) {
        if (g_isComputerWhite) return;
        if (g_lenWhite >= 8) return;
        g_nameWhite[g_lenWhite++] = ch;
        g_nameWhite[g_lenWhite] = '\0';
    }
    else {
        if (g_isComputerBlack) return;
        if (g_lenBlack >= 8) return;
        g_nameBlack[g_lenBlack++] = ch;
        g_nameBlack[g_lenBlack] = '\0';
    }
}

/*
  Sterge ultimul caracter (Backspace). Daca jucatorul este computer, ignor.
*/
static void backspaceName(int which) {
    if (which == 0) {
        if (g_isComputerWhite) return;
        if (g_lenWhite <= 0) return;
        g_lenWhite--; g_nameWhite[g_lenWhite] = '\0';
    }
    else {
        if (g_isComputerBlack) return;
        if (g_lenBlack <= 0) return;
        g_lenBlack--; g_nameBlack[g_lenBlack] = '\0';
    }
}

/*
  Dupa nume, trec in ecranul de optiuni.
  Daca un nume uman e gol, pun Player1 / Player2.
*/
static void goToOptionsFromNames() {
    if (!g_isComputerWhite && g_lenWhite == 0) {
        appendCharToName(0, 'P');
        appendCharToName(0, 'l');
        appendCharToName(0, 'a');
        appendCharToName(0, 'y');
        appendCharToName(0, 'e');
        appendCharToName(0, 'r');
        appendCharToName(0, '1');
    }
    if (!g_isComputerBlack && g_lenBlack == 0) {
        appendCharToName(1, 'P');
        appendCharToName(1, 'l');
        appendCharToName(1, 'a');
        appendCharToName(1, 'y');
        appendCharToName(1, 'e');
        appendCharToName(1, 'r');
        appendCharToName(1, '2');
    }

    g_editTurnTime = false;
    g_turnTimeCursor = 0;
    g_phase = PHASE_OPTIONS;
}

/*
  Porneste jocul din ecranul de optiuni.
  - parseaza timpul MM:SS
  - seteaza seed pentru inteligenta artificiala (depinde de timpul petrecut in meniu)
  - reseteaza meciul
*/
static void startGameFromOptions() {
    g_turnLimitSeconds = parseTimeMMSS(g_turnTimeStr);
    if (!g_useTurnTimer) g_turnLimitSeconds = 0;
    if (g_useTurnTimer && g_turnLimitSeconds <= 0) g_turnLimitSeconds = 60;

    // seed: depinde de cat timp ai stat pana ai apasat START + lungimi nume
    std::uint32_t seed = (std::uint32_t)g_seedClock.getElapsedTime().asMicroseconds();
    seed ^= (std::uint32_t)(g_lenWhite * 131 + g_lenBlack * 17);
    if (seed == 0) seed = 1;
    AI_Seed(seed);

    resetMatchKeepNames();
    g_phase = PHASE_GAME;
}

/*
  Aplica o mutare (fr,fc)->(tr,tc):
  - mut piesa pe tabla
  - cresc contorul de mutari
  - dupa prag, aplic eliminarea pieselor adversarului blocate
  - schimb tura
  - resetez timer-ul pe tura (doar daca urmeaza un jucator uman)
  - verific finalul
*/
static void doMove(int fr, int fc, int tr, int tc, sf::RenderWindow& w) {
    std::uint8_t mover = g_turn;

    g_board[tr][tc] = g_board[fr][fc];
    g_board[fr][fc] = 0;

    g_hasSel = false;
    g_moveCount = 0;
    g_ply++;

    if (g_ply >= g_captureAfterPly) {
        removeBlockedOpponent(mover);
    }

    g_turn = otherSide(g_turn);

    // resetez timer-ul pe tura doar pentru jucatori umani
    if (g_useTurnTimer && !isComputerSide(g_turn)) {
        g_turnClock.restart();
    }
    // pentru computer, resetez clock-ul de "gandire"
    g_aiClock.restart();

    checkGameOver(w);
}

/*
  Click pe tabla in timpul jocului:
  - daca nu e selectat nimic: selectez o piesa de-a mea
  - daca e selectat si apas pe alta piesa de-a mea: schimb selectia
  - daca apas pe un patrat valid: fac mutarea
  - altfel: deselect
*/
static void clickCellInGame(int r, int c, sf::RenderWindow& w) {
    std::uint8_t on = g_board[r][c];

    if (!g_hasSel) {
        if (on == g_turn) {
            g_selR = r; g_selC = c;
            g_hasSel = true;
            g_moveCount = genMoves(g_selR, g_selC, g_movesR, g_movesC);
        }
        return;
    }

    if (on == g_turn) {
        g_selR = r; g_selC = c;
        g_hasSel = true;
        g_moveCount = genMoves(g_selR, g_selC, g_movesR, g_movesC);
        return;
    }

    if (moveInList(r, c)) {
        doMove(g_selR, g_selC, r, c, w);
        return;
    }

    g_hasSel = false;
    g_moveCount = 0;
}


/*
  Cand expira timpul unei ture pentru un jucator uman:
  - jucatorul pierde tura (nu muta nimic)
  - se schimba tura la celalalt jucator
  - cresc contorul de mutari (ply), ca sa nu poti abuza de timeout ca sa amani finalul
*/
static void skipTurnBecauseTimeOut(sf::RenderWindow& w) {
    // anulez orice selectie / highlight
    g_hasSel = false;
    g_moveCount = 0;

    // consum o tura chiar daca nu s-a mutat nimic
    g_ply++;

    // schimb tura
    g_turn = otherSide(g_turn);

    // resetez timer-ul pe tura doar pentru jucatori umani
    if (g_useTurnTimer && !isComputerSide(g_turn) && g_turnLimitSeconds > 0) {
        g_turnClock.restart();
    }

    // pentru computer, resetez clock-ul de "gandire"
    g_aiClock.restart();

    checkGameOver(w);
}

/*
  Update in timpul jocului:
  - daca e timer pe tura si jucatorul UMAN a depasit limita -> pierde
  - daca este tura unui computer -> calculez o mutare dupa un delay
*/
static void updateGameAutomations(sf::RenderWindow& w) {
    if (g_phase != PHASE_GAME) return;

    // time out (doar pentru jucatorii umani)
    if (g_useTurnTimer && !isComputerSide(g_turn) && g_turnLimitSeconds > 0) {
        int sec = (int)g_turnClock.getElapsedTime().asSeconds();
        if (sec >= g_turnLimitSeconds) {
            // penalizare: jucatorul pierde tura
            skipTurnBecauseTimeOut(w);
            return;
        }
    }

    // mutare computer
    if (isComputerSide(g_turn)) {
        float t = g_aiClock.getElapsedTime().asSeconds();
        if (t < g_aiThinkDelay) return;

        Difficulty diff = (g_turn == 1) ? g_diffWhite : g_diffBlack;
        AIMove mv = AI_ChooseMove(g_board, g_turn, g_ply, g_captureAfterPly, diff);
        if (!mv.valid) {
            enterGameOver(w);
            return;
        }

        doMove(mv.fr, mv.fc, mv.tr, mv.tc, w);
        // doMove restarteaza deja g_aiClock
    }
}


// ============================================================================
//  UI / TEXT (stiluri)
// ============================================================================

/*
  Setez stilurile pentru toate textele (marimi + culori).
*/
static void setupTextStyles() {
    if (!g_fontReady) return;

    g_title.setCharacterSize(34);
    g_title.setFillColor(themes[0].color_text);

    g_hint.setCharacterSize(18);
    g_hint.setFillColor(themes[0].color_text);

    g_labelWhite.setCharacterSize(22);
    g_labelWhite.setFillColor(themes[0].color_text);

    g_labelBlack.setCharacterSize(22);
    g_labelBlack.setFillColor(themes[0].color_text);

    g_nameTextWhite.setCharacterSize(28);
    g_nameTextWhite.setFillColor(themes[0].color_text);

    g_nameTextBlack.setCharacterSize(28);
    g_nameTextBlack.setFillColor(themes[0].color_text);

    // HUD: numele jucatorului in tura (mai mare) + timpul ramas sub el
    g_hudTurn.setCharacterSize(34);
    g_hudTurn.setFillColor(themes[0].color_text);

    g_hudRemain.setCharacterSize(30);
    g_hudRemain.setFillColor(themes[0].color_text);

    g_hudLeft.setCharacterSize(22);
    g_hudLeft.setFillColor(themes[0].color_text);

    g_hudRight.setCharacterSize(22);
    g_hudRight.setFillColor(themes[0].color_text);

    // HUD: timpul total scurs in joc (format MM:SS)
    g_hudTime.setCharacterSize(22);
    g_hudTime.setFillColor(themes[0].color_text);

    g_overTitle.setCharacterSize(42);
    g_overTitle.setFillColor(themes[0].color_text);

    g_overWinner.setCharacterSize(54);
    g_overWinner.setFillColor(themes[0].color_text);

    g_overReasonText.setCharacterSize(22);
    g_overReasonText.setFillColor(themes[0].color_text);

    g_overHint.setCharacterSize(20);
    g_overHint.setFillColor(themes[0].color_text);
}

// Function to update text colors constantly during game loop
static void updateTextColors() {
    g_title.setFillColor(themes[0].color_text);
    g_hint.setFillColor(themes[0].color_text);
    g_labelWhite.setFillColor(themes[0].color_text);
    g_labelBlack.setFillColor(themes[0].color_text);
    g_nameTextWhite.setFillColor(themes[0].color_text);
    g_nameTextBlack.setFillColor(themes[0].color_text);
    g_hudTurn.setFillColor(themes[0].color_text);
    g_hudRemain.setFillColor(themes[0].color_text);
    g_hudLeft.setFillColor(themes[0].color_text);
    g_hudRight.setFillColor(themes[0].color_text);
    g_hudTime.setFillColor(themes[0].color_text);
    g_overTitle.setFillColor(themes[0].color_text);
    g_overWinner.setFillColor(themes[0].color_text);
    g_overReasonText.setFillColor(themes[0].color_text);
    g_overHint.setFillColor(themes[0].color_text);
}

// ============================================================================
//  DESENARE: tabla
// ============================================================================

/*
  Desenez o rama cu umbra.
*/
static void drawFrame(sf::RenderWindow& w) {
    const float boardPx = g_tile * 8.f;

    sf::RectangleShape shadow({ boardPx + 2.f * g_frame + 8.f, boardPx + 2.f * g_frame + 8.f });
    shadow.setPosition({ g_origin.x - g_frame + 6.f, g_origin.y - g_frame + 6.f });
    shadow.setFillColor(sf::Color(0, 0, 0, 70));
    w.draw(shadow);

    sf::RectangleShape frame({ boardPx + 2.f * g_frame, boardPx + 2.f * g_frame });
    frame.setPosition({ g_origin.x - g_frame, g_origin.y - g_frame });
    frame.setFillColor(themes[0].color_buttons);
    w.draw(frame);

    sf::RectangleShape innerLine({ boardPx + 2.f * (g_frame - 6.f), boardPx + 2.f * (g_frame - 6.f) });
    innerLine.setPosition({ g_origin.x - (g_frame - 6.f), g_origin.y - (g_frame - 6.f) });
    innerLine.setFillColor(sf::Color::Transparent);
    innerLine.setOutlineThickness(3.f);
    innerLine.setOutlineColor(themes[0].color_HoverButton);
    w.draw(innerLine);
}

/*
  Desenez tabla 8x8.
*/
static void drawBoard(sf::RenderWindow& w) {
    sf::RectangleShape sq({ g_tile, g_tile });
    sf::Color light = themes[0].color_BoardSquare1;
    sf::Color dark = themes[0].color_BoardSquare2;

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            bool isDark = ((r + c) & 1) != 0;
            sq.setFillColor(isDark ? dark : light);
            sq.setPosition({ g_origin.x + c * g_tile, g_origin.y + r * g_tile });
            w.draw(sq);
        }
    }
}

/*
  Desenez highlight-urile doar in timpul jocului (si doar pentru jucator uman).
*/
static void drawHighlights(sf::RenderWindow& w) {
    if (g_phase != PHASE_GAME) return;
    if (isComputerSide(g_turn)) return;
    if (!g_fontReady) return;

    sf::RectangleShape hl({ g_tile, g_tile });
    sf::Color hlColor = themes[0].color_HoverButton;
    hlColor.a = 110;
    hl.setFillColor(hlColor);

    for (int i = 0; i < g_moveCount; i++) {
        hl.setPosition({ g_origin.x + g_movesC[i] * g_tile, g_origin.y + g_movesR[i] * g_tile });
        w.draw(hl);
    }

    if (g_hasSel) {
        sf::RectangleShape selBox({ g_tile, g_tile });
        selBox.setFillColor(sf::Color::Transparent);
        selBox.setOutlineColor(themes[0].color_HoverButton);
        selBox.setOutlineThickness(5.f);
        selBox.setPosition({ g_origin.x + g_selC * g_tile, g_origin.y + g_selR * g_tile });
        w.draw(selBox);
    }
}

/*
  Desenez piesele ca cercuri.
*/
static void drawPieces(sf::RenderWindow& w) {
    float rad = g_tile * 0.28f;
    sf::CircleShape p(rad);
    p.setOrigin({ rad, rad });

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            std::uint8_t piece = g_board[r][c];
            if (piece == 0) continue;

            sf::Vector2f center{
                g_origin.x + c * g_tile + g_tile / 2.f,
                g_origin.y + r * g_tile + g_tile / 2.f
            };

            if (piece == 1) {
                p.setFillColor(themes[0].color_peace2);
                p.setOutlineColor(themes[0].color_peaceOutline2);
                p.setOutlineThickness(10.f);
            }
            else {
                p.setFillColor(themes[0].color_peace1);
                p.setOutlineColor(themes[0].color_peaceOutline1);
                p.setOutlineThickness(10.f);
            }

            p.setPosition(center);
            w.draw(p);
        }
    }
}


// ============================================================================
//  DESENARE: HUD in joc
// ============================================================================

/*
  HUD cerut:
  - sus de tot stanga: timpul total scurs
  - deasupra tablei: a cui este tura
  - stanga: status alb (capturi + piese)
  - dreapta: status negru (capturi + piese)
*/
static void drawHUD(sf::RenderWindow& w) {
    if (!g_fontReady) return;
    if (g_phase != PHASE_GAME && g_phase != PHASE_OVER) return;

    // Dimensiuni baza (rama tablei)
    const float boardPx = g_tile * 8.f;
    const float frameX = g_origin.x - g_frame;
    const float frameY = g_origin.y - g_frame;
    const float frameW = boardPx + 2.f * g_frame;
    const float frameH = boardPx + 2.f * g_frame;

    // ----------------- PANOU SUS (in rama): tura + timp ramas + timp total -----------------

    const float headerH = 120.f;
    const float headerGap = 16.f;
    float headerY = frameY - headerH - headerGap;
    if (headerY < 12.f) headerY = 12.f;

    sf::RectangleShape headerBg({ frameW, headerH });
    headerBg.setPosition({ frameX, headerY });
    headerBg.setFillColor(themes[0].color_buttons);
    headerBg.setOutlineThickness(4.f);
    headerBg.setOutlineColor(themes[0].color_text);
    w.draw(headerBg);

    // timpul total scurs (stanga sus in panou) - doar MM:SS
    {
        int sec = (int)g_gameClock.getElapsedTime().asSeconds();
        int mm = sec / 60;
        int ss = sec % 60;
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%02d:%02d", mm, ss);
        g_hudTime.setString(buf);
        g_hudTime.setOrigin({ 0.f, 0.f });
        g_hudTime.setPosition({ frameX + 16.f, headerY + 10.f });
        w.draw(g_hudTime);
    }

    // textul cu numele jucatorului care este in tura (centrat deasupra tablei)
    {
        const char* whoName = (g_turn == 1) ? g_nameWhite : g_nameBlack;
        if (whoName[0] == 0) whoName = (g_turn == 1) ? "ALB" : "NEGRU";

        g_hudTurn.setString(whoName);

        auto b = g_hudTurn.getLocalBounds();
        g_hudTurn.setOrigin({ b.position.x + b.size.x * 0.5f, b.position.y + b.size.y * 0.5f });
        g_hudTurn.setPosition({ frameX + frameW * 0.5f, headerY + 44.f });
        w.draw(g_hudTurn);
    }

    // timpul ramas sub tura (centrat), fara text de tipul "Ramas:"
    {
        char buf[16] = "";

        if (g_useTurnTimer && !isComputerSide(g_turn) && g_turnLimitSeconds > 0) {
            int used = (int)g_turnClock.getElapsedTime().asSeconds();
            int left = g_turnLimitSeconds - used;
            if (left < 0) left = 0;
            int lm = left / 60;
            int ls = left % 60;
            std::snprintf(buf, sizeof(buf), "%02d:%02d", lm, ls);
        }

        g_hudRemain.setString(buf);
        if (buf[0] != 0) {
            auto b = g_hudRemain.getLocalBounds();
            g_hudRemain.setOrigin({ b.position.x + b.size.x * 0.5f, b.position.y + b.size.y * 0.5f });
            g_hudRemain.setPosition({ frameX + frameW * 0.5f, headerY + 86.f });
            w.draw(g_hudRemain);
        }
    }

    // ----------------- PANOURI LATERALE (in rama): alb / negru -----------------

    int wPieces = countPieces(1);
    int bPieces = countPieces(2);

    const float panelW = 280.f;
    const float panelH = 210.f;
    const float panelGap = 26.f;
    const float panelY = frameY + (frameH - panelH) * 0.5f;

    // panou alb (stanga)
    {
        sf::RectangleShape bg({ panelW, panelH });
        bg.setPosition({ frameX - panelW - panelGap, panelY });
        bg.setFillColor(themes[0].color_buttons);
        bg.setOutlineThickness(4.f);
        bg.setOutlineColor(themes[0].color_text);
        w.draw(bg);

        char buf[256];
        std::snprintf(buf, sizeof(buf), "ALB\n%s\nPiese: %d\nCapturi: %d",
            g_nameWhite, wPieces, g_capturedByWhite);
        g_hudLeft.setString(buf);
        g_hudLeft.setPosition({ bg.getPosition().x + 16.f, bg.getPosition().y + 14.f });
        w.draw(g_hudLeft);
    }

    // panou negru (dreapta)
    {
        sf::RectangleShape bg({ panelW, panelH });
        bg.setPosition({ frameX + frameW + panelGap, panelY });
        bg.setFillColor(themes[0].color_buttons);
        bg.setOutlineThickness(4.f);
        bg.setOutlineColor(themes[0].color_text);
        w.draw(bg);

        char buf[256];
        std::snprintf(buf, sizeof(buf), "NEGRU\n%s\nPiese: %d\nCapturi: %d",
            g_nameBlack, bPieces, g_capturedByBlack);
        g_hudRight.setString(buf);
        g_hudRight.setPosition({ bg.getPosition().x + 16.f, bg.getPosition().y + 14.f });
        w.draw(g_hudRight);
    }
}


// ============================================================================
//  DESENARE: ecrane setup (mode / nume / optiuni)
// ============================================================================

/*
  Desenez un buton simplu (dreptunghi + text), cu highlight cand e activ/hover.
*/
static void drawButton(sf::RenderWindow& w, const sf::FloatRect& r, const char* text, bool active, bool hover) {
    sf::RectangleShape box({ r.size.x, r.size.y });
    box.setPosition({ r.position.x, r.position.y });

    sf::Color fill = themes[0].color_buttons;
    if (active) fill = themes[0].color_HoverButton;
    if (hover) fill = themes[0].color_HoverButton;

    // Slight variation for active+hover if needed, but keeping it simple for theme consistency

    box.setFillColor(fill);
    box.setOutlineThickness(3.f);
    box.setOutlineColor(active ? themes[0].color_HoverButton : themes[0].color_buttons); // Or border color if available
    w.draw(box);

    if (!g_fontReady) return;
    sf::Text t(g_font);
    t.setCharacterSize(26);
    t.setFillColor(themes[0].color_text);
    t.setString(text);
    t.setPosition({ r.position.x + 18.f, r.position.y + 16.f });
    w.draw(t);
}

/*
  Ecran: alegerea modului de joc.
*/
static void drawModeSelect(sf::RenderWindow& w) {
    if (!g_fontReady) return;

    auto sz = w.getSize();
    float W = (float)sz.x;
    float H = (float)sz.y;

    sf::RectangleShape dim({ W, H });
    dim.setFillColor(sf::Color(0, 0, 0, 60));
    w.draw(dim);

    sf::RectangleShape panel({ 820.f, 520.f });
    panel.setPosition({ (W - 820.f) / 2.f, (H - 520.f) / 2.f });
    panel.setFillColor(themes[0].color_buttons);
    panel.setOutlineThickness(4.f);
    panel.setOutlineColor(themes[0].color_text);
    w.draw(panel);

    float px = panel.getPosition().x;
    float py = panel.getPosition().y;

    g_title.setString("Alege modul de joc");
    g_title.setPosition({ px + 40.f, py + 30.f });
    w.draw(g_title);

    sf::FloatRect b1({ px + 40.f, py + 130.f }, { 740.f, 80.f });
    sf::FloatRect b2({ px + 40.f, py + 230.f }, { 740.f, 80.f });
    sf::FloatRect b3({ px + 40.f, py + 330.f }, { 740.f, 80.f });

    drawButton(w, b1, "1) Player vs Player", (g_mode == GameMode::PvP), mouseInRect(w, b1));
    drawButton(w, b2, "2) Player vs Computer", (g_mode == GameMode::PvC), mouseInRect(w, b2));
    drawButton(w, b3, "3) Computer vs Computer", (g_mode == GameMode::CvC), mouseInRect(w, b3));

    g_hint.setString("Click pe un mod sau tastele 1/2/3. ENTER continua. ESC meniu");
    g_hint.setPosition({ px + 40.f, py + 440.f });
    w.draw(g_hint);
}

/*
  Ecran: introducere nume.
  Daca un jucator este computer, campul este blocat.
*/
static void drawNameEntry(sf::RenderWindow& w) {
    if (!g_fontReady) return;

    auto sz = w.getSize();
    float W = (float)sz.x;
    float H = (float)sz.y;

    sf::RectangleShape dim({ W, H });
    dim.setFillColor(sf::Color(0, 0, 0, 60));
    w.draw(dim);

    sf::RectangleShape panel({ 820.f, 440.f });
    panel.setPosition({ (W - 820.f) / 2.f, (H - 440.f) / 2.f });
    panel.setFillColor(themes[0].color_buttons);
    panel.setOutlineThickness(4.f);
    panel.setOutlineColor(themes[0].color_text);
    w.draw(panel);

    float px = panel.getPosition().x;
    float py = panel.getPosition().y;

    g_title.setString("Introdu numele");
    g_title.setPosition({ px + 40.f, py + 30.f });
    w.draw(g_title);

    g_labelWhite.setString("ALB:");
    g_labelWhite.setPosition({ px + 40.f, py + 120.f });
    w.draw(g_labelWhite);

    g_labelBlack.setString("NEGRU:");
    g_labelBlack.setPosition({ px + 40.f, py + 230.f });
    w.draw(g_labelBlack);

    sf::RectangleShape box1({ 740.f, 60.f });
    box1.setPosition({ px + 40.f, py + 155.f });
    box1.setFillColor(g_isComputerWhite ? themes[0].color_buttons : themes[0].color_bg);
    box1.setOutlineThickness(3.f);
    if (g_isComputerWhite) box1.setOutlineColor(themes[0].color_buttons);
    else box1.setOutlineColor(g_activeName == 0 ? themes[0].color_HoverButton : themes[0].color_buttons);
    w.draw(box1);

    sf::RectangleShape box2({ 740.f, 60.f });
    box2.setPosition({ px + 40.f, py + 265.f });
    box2.setFillColor(g_isComputerBlack ? themes[0].color_buttons : themes[0].color_bg);
    box2.setOutlineThickness(3.f);
    if (g_isComputerBlack) box2.setOutlineColor(themes[0].color_buttons);
    else box2.setOutlineColor(g_activeName == 1 ? themes[0].color_HoverButton : themes[0].color_buttons);
    w.draw(box2);

    g_nameTextWhite.setString(g_nameWhite);
    g_nameTextWhite.setPosition({ px + 55.f, py + 165.f });
    w.draw(g_nameTextWhite);

    g_nameTextBlack.setString(g_nameBlack);
    g_nameTextBlack.setPosition({ px + 55.f, py + 275.f });
    w.draw(g_nameTextBlack);

    // cursor simplu (linie) la finalul campului activ
    if (g_activeName >= 0) {
        sf::RectangleShape caret({ 2.f, 32.f });
        caret.setFillColor(themes[0].color_text);

        if (g_activeName == 0 && !g_isComputerWhite) {
            auto b = g_nameTextWhite.getLocalBounds();
            caret.setPosition({ g_nameTextWhite.getPosition().x + b.size.x + 6.f,
                                g_nameTextWhite.getPosition().y + 6.f });
            w.draw(caret);
        }
        else if (g_activeName == 1 && !g_isComputerBlack) {
            auto b = g_nameTextBlack.getLocalBounds();
            caret.setPosition({ g_nameTextBlack.getPosition().x + b.size.x + 6.f,
                                g_nameTextBlack.getPosition().y + 6.f });
            w.draw(caret);
        }
    }

    g_hint.setString("Taste: TAB schimba campul, ENTER continua, BACKSPACE sterge, B inapoi, ESC meniu");
    g_hint.setPosition({ px + 40.f, py + 360.f });
    w.draw(g_hint);
}

/*
  Ecran: optiuni (timer + dificultati computer).
*/
static void drawOptions(sf::RenderWindow& w) {
    if (!g_fontReady) return;

    auto sz = w.getSize();
    float W = (float)sz.x;
    float H = (float)sz.y;

    sf::RectangleShape dim({ W, H });
    dim.setFillColor(sf::Color(0, 0, 0, 60));
    w.draw(dim);

    sf::RectangleShape panel({ 920.f, 560.f });
    panel.setPosition({ (W - 920.f) / 2.f, (H - 560.f) / 2.f });
    panel.setFillColor(themes[0].color_buttons);
    panel.setOutlineThickness(4.f);
    panel.setOutlineColor(themes[0].color_text);
    w.draw(panel);

    float px = panel.getPosition().x;
    float py = panel.getPosition().y;

    g_title.setString("Optiuni joc");
    g_title.setPosition({ px + 40.f, py + 30.f });
    w.draw(g_title);

    // timer toggle
    sf::FloatRect timerBtn({ px + 40.f, py + 120.f }, { 360.f, 70.f });
    drawButton(w, timerBtn, g_useTurnTimer ? "Timer pe tura: ON" : "Timer pe tura: OFF", g_useTurnTimer, mouseInRect(w, timerBtn));

    // time box
    sf::FloatRect timeBox({ px + 420.f, py + 120.f }, { 220.f, 70.f });
    sf::RectangleShape tb({ timeBox.size.x, timeBox.size.y });
    tb.setPosition({ timeBox.position.x, timeBox.position.y });
    tb.setFillColor(g_useTurnTimer ? themes[0].color_bg : themes[0].color_buttons);
    tb.setOutlineThickness(3.f);
    tb.setOutlineColor(g_editTurnTime ? themes[0].color_HoverButton : themes[0].color_text);
    w.draw(tb);

    sf::Text tTime(g_font);
    tTime.setCharacterSize(30);
    tTime.setFillColor(themes[0].color_text);
    tTime.setString(g_turnTimeStr);
    tTime.setPosition({ timeBox.position.x + 32.f, timeBox.position.y + 16.f });
    w.draw(tTime);

    // caret in box de timp
    if (g_editTurnTime) {
        // pozitionare simpla: fiecare caracter are latime aproximativa
        // nu fac masuratori perfecte ca sa ramana simplu
        float baseX = tTime.getPosition().x;
        float baseY = tTime.getPosition().y;

        int cursorPos = g_turnTimeCursor;
        float dx = 0.f;
        if (cursorPos == 0) dx = 0.f;
        else if (cursorPos == 1) dx = 18.f;
        else if (cursorPos == 3) dx = 18.f * 3.f;
        else if (cursorPos == 4) dx = 18.f * 4.f;

        sf::RectangleShape caret({ 2.f, 34.f });
        caret.setFillColor(themes[0].color_text);
        caret.setPosition({ baseX + dx + 2.f, baseY + 2.f });
        w.draw(caret);
    }

    // dificultati computer
    float y = py + 220.f;

    if (g_isComputerWhite) {
        sf::FloatRect dW({ px + 40.f, y }, { 600.f, 70.f });
        const char* label = (g_diffWhite == Difficulty::Easy)
            ? "Computer Alb: Easy"
            : "Computer Alb: Hard";
        drawButton(w, dW, label, true, mouseInRect(w, dW));
        y += 90.f;
    }

    if (g_isComputerBlack) {
        sf::FloatRect dB({ px + 40.f, y }, { 600.f, 70.f });
        const char* label = (g_diffBlack == Difficulty::Easy)
            ? "Computer Negru: Easy"
            : "Computer Negru: Hard";
        drawButton(w, dB, label, true, mouseInRect(w, dB));
        y += 90.f;
    }

    // butoane Start / Back
    sf::FloatRect backBtn({ px + 40.f, py + 460.f }, { 220.f, 70.f });
    sf::FloatRect startBtn({ px + 660.f, py + 460.f }, { 220.f, 70.f });
    drawButton(w, backBtn, "B) Inapoi", false, mouseInRect(w, backBtn));
    drawButton(w, startBtn, "ENTER) Start", true, mouseInRect(w, startBtn));

    g_hint.setString("Click pe optiuni. ENTER Start. B inapoi. ESC meniu");
    g_hint.setPosition({ px + 40.f, py + 540.f - 30.f });
    w.draw(g_hint);
}


// ============================================================================
//  DESENARE: final
// ============================================================================

/*
  Desenez confetti.
  Important SFML 3: setRotation primeste sf::Angle, deci folosesc sf::degrees(...)
*/
static void drawConfetti(sf::RenderWindow& w) {
    float t = g_overClock.getElapsedTime().asSeconds();
    auto sz = w.getSize();
    float W = (float)sz.x;
    float H = (float)sz.y;

    sf::RectangleShape piece({ 8.f, 8.f });

    for (int i = 0; i < CONF_N; i++) {
        float x = g_confPos[i].x + g_confVel[i].x * t;
        float y = g_confPos[i].y + g_confVel[i].y * t;

        while (x < -20.f) x += (W + 40.f);
        while (x > W + 20.f) x -= (W + 40.f);
        while (y > H + 20.f) y -= (H + 60.f);

        float s = g_confSize[i];
        piece.setSize({ s, s });
        piece.setPosition({ x, y });
        piece.setRotation(sf::degrees(t * 120.f + i * 7.f));

        sf::Color col((std::uint8_t)((i * 50) % 255),
            (std::uint8_t)((i * 90) % 255),
            (std::uint8_t)((i * 140) % 255), 220);
        piece.setFillColor(col);
        w.draw(piece);
    }
}

/*
  Ecran final: overlay intunecat + confetti + text cu castigatorul.
*/
static void drawGameOver(sf::RenderWindow& w) {
    if (!g_fontReady) return;

    auto sz = w.getSize();
    float W = (float)sz.x;
    float H = (float)sz.y;

    sf::RectangleShape dim({ W, H });
    dim.setFillColor(sf::Color(0, 0, 0, 150));
    w.draw(dim);

    drawConfetti(w);

    sf::RectangleShape banner({ 900.f, 360.f });
    banner.setPosition({ (W - 900.f) / 2.f, (H - 360.f) / 2.f });

    // Applying buttons color with some alpha for the banner background
    sf::Color bannerColor = themes[0].color_buttons;
    bannerColor.a = 220;
    banner.setFillColor(bannerColor);

    banner.setOutlineThickness(4.f);
    banner.setOutlineColor(themes[0].color_HoverButton);
    w.draw(banner);

    float bx = banner.getPosition().x;
    float by = banner.getPosition().y;

    g_overTitle.setString("FINAL");
    g_overTitle.setPosition({ bx + 40.f, by + 25.f });
    w.draw(g_overTitle);

    char winnerBuf[256];
    if (g_winner == 1) {
        std::snprintf(winnerBuf, sizeof(winnerBuf), "Castigator: %s (ALB)", g_nameWhite);
    }
    else if (g_winner == 2) {
        std::snprintf(winnerBuf, sizeof(winnerBuf), "Castigator: %s (NEGRU)", g_nameBlack);
    }
    else {
        std::snprintf(winnerBuf, sizeof(winnerBuf), "REZULTAT: REMIZA");
    }

    g_overWinner.setString(winnerBuf);
    g_overWinner.setPosition({ bx + 40.f, by + 95.f });
    w.draw(g_overWinner);

    if (g_overReason[0] != 0) {
        char rbuf[128];
        std::snprintf(rbuf, sizeof(rbuf), "Motiv: %s", g_overReason);
        g_overReasonText.setString(rbuf);
        g_overReasonText.setPosition({ bx + 40.f, by + 175.f });
        w.draw(g_overReasonText);
    }

    g_overHint.setString("R = rematch | ENTER = setup | ESC = meniu");
    g_overHint.setPosition({ bx + 40.f, by + 310.f });
    w.draw(g_overHint);
}


// ============================================================================
//  FUNCTII PUBLICE (apelate din main.cpp)
// ============================================================================

// variabila globala din proiect (interfata 1=meniu, 2=play)
extern int interfata;

/*
  Initializare modul Play:
  - incarc fontul o singura data
  - setez stilurile textelor
  - pornesc in ecranul de alegere mod
*/
void Play_Init() {
    if (!g_fontReady) {
        g_fontReady = g_font.openFromFile("Themes/Kanit-Medium.ttf");
        setupTextStyles();
    }
    g_cursorHand = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
    g_cursorArrow = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);
    resetAllToModeSelect();
}

/*
  Reset cand intri in Play (dupa ce apesi Play din meniu).
*/
void Play_Reset() {
    resetAllToModeSelect();
}

/*
  Handler de evenimente:
  - ESC: inapoi la meniu
  - PHASE_MODE: click / 1/2/3 pentru mod, ENTER continua
  - PHASE_NAMES: scrie nume (doar umani), TAB, BACKSPACE, ENTER, B inapoi
  - PHASE_OPTIONS: click toggle, edit timp MM:SS, ENTER start, B inapoi
  - PHASE_GAME: click mutare (doar daca e tura unui om), R rematch
  - PHASE_OVER: R rematch, ENTER setup
*/
void Play_HandleEvent(const sf::Event& e, sf::RenderWindow& w) {
    // ESC merge in orice faza
    if (const auto* k = e.getIf<sf::Event::KeyPressed>()) {
        if (k->code == sf::Keyboard::Key::Escape) {
            audio.playClick();
            g_hasSel = false;
            g_moveCount = 0;
            interfata = 1;
            return;
        }
    }

    // ------------------- PHASE_MODE -------------------
    if (g_phase == PHASE_MODE) {
        if (const auto* k = e.getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::Num1) { audio.playClick(); applyModeAndPrepareNames(GameMode::PvP); return; }
            if (k->code == sf::Keyboard::Key::Num2) { audio.playClick(); applyModeAndPrepareNames(GameMode::PvC); return; }
            if (k->code == sf::Keyboard::Key::Num3) { audio.playClick(); applyModeAndPrepareNames(GameMode::CvC); return; }
            if (k->code == sf::Keyboard::Key::Enter) {
                audio.playClick();
                // daca nu s-a selectat nimic explicit, raman PvP
                applyModeAndPrepareNames(g_mode);
                g_phase = PHASE_NAMES;
                return;
            }
        }

        if (const auto* mb = e.getIf<sf::Event::MouseButtonPressed>()) {
            if (mb->button != sf::Mouse::Button::Left) return;
            audio.playClick();

            auto sz = w.getSize();
            float W = (float)sz.x;
            float H = (float)sz.y;
            float px = (W - 820.f) / 2.f;
            float py = (H - 520.f) / 2.f;

            sf::FloatRect b1({ px + 40.f, py + 130.f }, { 740.f, 80.f });
            sf::FloatRect b2({ px + 40.f, py + 230.f }, { 740.f, 80.f });
            sf::FloatRect b3({ px + 40.f, py + 330.f }, { 740.f, 80.f });

            if (mouseInRect(w, b1)) { applyModeAndPrepareNames(GameMode::PvP); g_phase = PHASE_NAMES; return; }
            if (mouseInRect(w, b2)) { applyModeAndPrepareNames(GameMode::PvC); g_phase = PHASE_NAMES; return; }
            if (mouseInRect(w, b3)) { applyModeAndPrepareNames(GameMode::CvC); g_phase = PHASE_NAMES; return; }
        }

        return;
    }

    // ------------------- PHASE_NAMES -------------------
    if (g_phase == PHASE_NAMES) {
        if (const auto* k = e.getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::B) {
                audio.playClick();
                g_phase = PHASE_MODE;
                return;
            }
            if (k->code == sf::Keyboard::Key::Tab) {
                audio.playClick();
                if (g_activeName >= 0) g_activeName = nextEditableNameField(g_activeName);
                return;
            }
            if (k->code == sf::Keyboard::Key::Backspace) {
                audio.playClick();
                if (g_activeName >= 0) backspaceName(g_activeName);
                return;
            }
            if (k->code == sf::Keyboard::Key::Enter) {
                audio.playClick();

                // In modul Player vs Player am doua campuri editabile.
                // Vreau ca ENTER sa treaca de la ALB la NEGRU, iar apoi sa porneasca ecranul de optiuni.
                // In modurile unde exista un singur jucator uman, ENTER duce direct la optiuni.
                if (g_activeName == 0) {
                    // If white player name is empty, populate with default "Player1"
                    if (!g_isComputerWhite && g_lenWhite == 0) {
                        appendCharToName(0, 'P');
                        appendCharToName(0, 'l');
                        appendCharToName(0, 'a');
                        appendCharToName(0, 'y');
                        appendCharToName(0, 'e');
                        appendCharToName(0, 'r');
                        appendCharToName(0, '1');
                    }
                    if (!g_isComputerBlack) {
                        g_activeName = 1;
                        return;
                    }
                    goToOptionsFromNames();
                    return;
                }

                if (g_activeName == 1) {
                    // If black player name is empty, populate with default "Player2"
                    if (!g_isComputerBlack && g_lenBlack == 0) {
                        appendCharToName(1, 'P');
                        appendCharToName(1, 'l');
                        appendCharToName(1, 'a');
                        appendCharToName(1, 'y');
                        appendCharToName(1, 'e');
                        appendCharToName(1, 'r');
                        appendCharToName(1, '2');
                    }
                    goToOptionsFromNames();
                    return;
                }

                // Daca nu am camp editabil (Computer vs Computer), trec direct mai departe.
                goToOptionsFromNames();
                return;
            }
        }

        if (const auto* te = e.getIf<sf::Event::TextEntered>()) {
            if (g_activeName < 0) return;
            char32_t u = te->unicode;
            if (u >= 32 && u <= 126) {
                char ch = (char)u;
                if (ch != '|' && ch != '\n' && ch != '\r' && ch != '\t') {
                    audio.playKey();
                    appendCharToName(g_activeName, ch);
                }
            }
            return;
        }

        return;
    }

    // ------------------- PHASE_OPTIONS -------------------
    if (g_phase == PHASE_OPTIONS) {
        if (const auto* k = e.getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::B) {
                audio.playClick();
                g_phase = PHASE_NAMES;
                return;
            }
            if (k->code == sf::Keyboard::Key::Enter) {
                audio.playClick();
                startGameFromOptions();
                return;
            }
            if (k->code == sf::Keyboard::Key::Backspace) {
                if (g_editTurnTime) {
                    audio.playClick();
                    // backspace in edit timp: mut cursor inapoi si pun 0
                    if (g_turnTimeCursor == 4) g_turnTimeCursor = 3;
                    else if (g_turnTimeCursor == 3) g_turnTimeCursor = 1;
                    else if (g_turnTimeCursor == 1) g_turnTimeCursor = 0;
                    else g_turnTimeCursor = 0;

                    if (g_turnTimeCursor == 0 || g_turnTimeCursor == 1 || g_turnTimeCursor == 3 || g_turnTimeCursor == 4) {
                        g_turnTimeStr[g_turnTimeCursor] = '0';
                    }
                    return;
                }
            }
        }

        if (const auto* te = e.getIf<sf::Event::TextEntered>()) {
            if (!g_editTurnTime) return;
            if (!g_useTurnTimer) return;

            char32_t u = te->unicode;
            if (u >= '0' && u <= '9') {
                char digit = (char)u;
                bool valid = false;

                // Validate based on cursor position
                if (g_turnTimeCursor == 0) {
                    // First minute digit: only 0-5
                    valid = (digit >= '0' && digit <= '5');
                }
                else if (g_turnTimeCursor == 1) {
                    // Second minute digit: check if total minutes would be valid (00-59)
                    int firstMin = g_turnTimeStr[0] - '0';
                    int totalMin = firstMin * 10 + (digit - '0');
                    valid = (totalMin <= 59);
                }
                else if (g_turnTimeCursor == 3) {
                    // First second digit: only 0-5
                    valid = (digit >= '0' && digit <= '5');
                }
                else if (g_turnTimeCursor == 4) {
                    // Second second digit: check if total seconds would be valid (00-59)
                    int firstSec = g_turnTimeStr[3] - '0';
                    int totalSec = firstSec * 10 + (digit - '0');
                    valid = (totalSec <= 59);
                }

                if (valid) {
                    audio.playKey();
                    g_turnTimeStr[g_turnTimeCursor] = digit;

                    // avans cursor: 0->1->3->4->0...
                    if (g_turnTimeCursor == 0) g_turnTimeCursor = 1;
                    else if (g_turnTimeCursor == 1) g_turnTimeCursor = 3;
                    else if (g_turnTimeCursor == 3) g_turnTimeCursor = 4;
                    else g_turnTimeCursor = 0;
                }
            }
            return;
        }

        if (const auto* mb = e.getIf<sf::Event::MouseButtonPressed>()) {
            if (mb->button != sf::Mouse::Button::Left) return;
            audio.playClick();

            auto sz = w.getSize();
            float W = (float)sz.x;
            float H = (float)sz.y;
            float px = (W - 920.f) / 2.f;
            float py = (H - 560.f) / 2.f;

            sf::FloatRect timerBtn({ px + 40.f, py + 120.f }, { 360.f, 70.f });
            sf::FloatRect timeBox({ px + 420.f, py + 120.f }, { 220.f, 70.f });

            float y = py + 220.f;
            sf::FloatRect dW({ px + 40.f, y }, { 600.f, 70.f });
            if (g_isComputerWhite) y += 90.f;
            sf::FloatRect dB({ px + 40.f, y }, { 600.f, 70.f });

            sf::FloatRect backBtn({ px + 40.f, py + 460.f }, { 220.f, 70.f });
            sf::FloatRect startBtn({ px + 660.f, py + 460.f }, { 220.f, 70.f });

            if (mouseInRect(w, timerBtn)) {
                g_useTurnTimer = !g_useTurnTimer;
                if (!g_useTurnTimer) g_editTurnTime = false;
                return;
            }

            if (mouseInRect(w, timeBox)) {
                if (g_useTurnTimer) {
                    g_editTurnTime = true;
                    g_turnTimeCursor = 0;
                }
                return;
            }

            if (g_isComputerWhite && mouseInRect(w, dW)) {
                g_diffWhite = (g_diffWhite == Difficulty::Easy) ? Difficulty::Hard : Difficulty::Easy;
                return;
            }

            if (g_isComputerBlack && mouseInRect(w, dB)) {
                g_diffBlack = (g_diffBlack == Difficulty::Easy) ? Difficulty::Hard : Difficulty::Easy;
                return;
            }

            if (mouseInRect(w, backBtn)) {
                g_phase = PHASE_NAMES;
                return;
            }
            if (mouseInRect(w, startBtn)) {
                startGameFromOptions();
                return;
            }

            // click in afara timeBox -> ies din edit
            g_editTurnTime = false;
        }

        return;
    }

    // ------------------- PHASE_OVER -------------------
    if (g_phase == PHASE_OVER) {
        if (const auto* k = e.getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::R) {
                audio.playClick();
                resetMatchKeepNames();
                g_phase = PHASE_GAME;
                return;
            }
            if (k->code == sf::Keyboard::Key::Enter) {
                audio.playClick();
                resetAllToModeSelect();
                return;
            }
        }
        return;
    }

    // ------------------- PHASE_GAME -------------------
    if (g_phase == PHASE_GAME) {
        if (const auto* k = e.getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::R) {
                audio.playClick();
                resetMatchKeepNames();
                return;
            }
        }

        if (const auto* mb = e.getIf<sf::Event::MouseButtonPressed>()) {
            if (mb->button != sf::Mouse::Button::Left) return;

            // daca e tura computerului, ignor click
            if (isComputerSide(g_turn)) return;

            audio.playClick();
            updateLayout(w);

            int r, c;
            if (!mouseToCell(w, r, c)) {
                g_hasSel = false;
                g_moveCount = 0;
                return;
            }

            clickCellInGame(r, c, w);
        }
        return;
    }
}

static void updateCursor(sf::RenderWindow& w) {
    if (!g_cursorHand.has_value() || !g_cursorArrow.has_value()) return;

    bool hover = false;
    sf::Vector2i mp = sf::Mouse::getPosition(w);
    sf::Vector2f m = w.mapPixelToCoords(mp);

    if (g_phase == PHASE_MODE) {
        auto sz = w.getSize();
        float W = (float)sz.x;
        float H = (float)sz.y;
        float px = (W - 820.f) / 2.f;
        float py = (H - 520.f) / 2.f;

        if (sf::FloatRect({ px + 40.f, py + 130.f }, { 740.f, 80.f }).contains(m)) hover = true;
        else if (sf::FloatRect({ px + 40.f, py + 230.f }, { 740.f, 80.f }).contains(m)) hover = true;
        else if (sf::FloatRect({ px + 40.f, py + 330.f }, { 740.f, 80.f }).contains(m)) hover = true;
    }
    else if (g_phase == PHASE_OPTIONS) {
        auto sz = w.getSize();
        float W = (float)sz.x;
        float H = (float)sz.y;
        float px = (W - 920.f) / 2.f;
        float py = (H - 560.f) / 2.f;

        if (sf::FloatRect({ px + 40.f, py + 120.f }, { 360.f, 70.f }).contains(m)) hover = true;
        else if (sf::FloatRect({ px + 420.f, py + 120.f }, { 220.f, 70.f }).contains(m)) hover = true;

        float y = py + 220.f;
        if (g_isComputerWhite) {
            if (sf::FloatRect({ px + 40.f, y }, { 600.f, 70.f }).contains(m)) hover = true;
            y += 90.f;
        }
        if (g_isComputerBlack) {
            if (sf::FloatRect({ px + 40.f, y }, { 600.f, 70.f }).contains(m)) hover = true;
        }

        if (sf::FloatRect({ px + 40.f, py + 460.f }, { 220.f, 70.f }).contains(m)) hover = true;
        else if (sf::FloatRect({ px + 660.f, py + 460.f }, { 220.f, 70.f }).contains(m)) hover = true;
    }
    else if (g_phase == PHASE_GAME) {
        if (!isComputerSide(g_turn)) {
            sf::FloatRect br(g_origin, { g_tile * 8.f, g_tile * 8.f });
            if (br.contains(m)) hover = true;
        }
    }

    if (hover) w.setMouseCursor(*g_cursorHand);
    else w.setMouseCursor(*g_cursorArrow);
}

/*
  Functia de desen:
  - in setup (mode/nume/optiuni) desenez tabla ca fundal + overlay
  - in joc desenez tabla + highlight + HUD
  - in final desenez overlay-ul de final peste joc
*/
void Play_Draw(sf::RenderWindow& w) {
    updateCursor(w);
    updateTextColors(); // Ensure text colors are updated every frame
    updateLayout(w);

    // update automatizari (time out + mutari computer)
    updateGameAutomations(w);

    // fundal comun: tabla
    drawFrame(w);
    drawBoard(w);
    drawHighlights(w);
    drawPieces(w);

    if (g_phase == PHASE_MODE) {
        drawModeSelect(w);
        return;
    }

    if (g_phase == PHASE_NAMES) {
        drawNameEntry(w);
        return;
    }

    if (g_phase == PHASE_OPTIONS) {
        drawOptions(w);
        return;
    }

    if (g_phase == PHASE_GAME) {
        drawHUD(w);
        return;
    }

    if (g_phase == PHASE_OVER) {
        // in spate ramane tabla, iar deasupra overlay final
        drawHUD(w);
        drawGameOver(w);
        return;
    }
}