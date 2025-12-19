#include "PlayScreen.hpp"
#include <cstdint>
#include <cstdio>                // snprintf
#include <SFML/System/Angle.hpp> // sf::degrees (SFML 3)
#include "../SoundEffects/SoundEffects.hpp"

// ============================================================================
//  PLAYSCREEN
//  - Nu folosesc clase proprii, doar variabile globale + functii.
//  - Exista 3 stari (phase):
//      0) Introduc numele jucatorilor
//      1) Jocul propriu-zis
//      2) Final: afisez castigatorul + efecte grafice
//
//  Reguli implementate:
//  - Mutare: 1 pas pe diagonala intr-o celula libera.
//  - Dupa un prag de mutari (captureAfterPly), elimin automat piesele blocate
//    (adica fara mutari legale). Asta e "captura" prin blocare in varianta PC.
//  - Final: cand cineva ramane fara piese / limita de mutari / un jucator nu mai are mutari.
//  - Castigator: cine are mai multe piese ramase (simplu si clar).
// ============================================================================


// ------------------------------ STARE JOC ------------------------------

AudioMenu audio;

// Tabla: 0 liber, 1 alb, 2 negru
static std::uint8_t g_board[8][8];

// Tura curenta: 1 alb, 2 negru
static std::uint8_t g_turn = 1;

// Selectie: daca am selectat o piesa
static bool g_hasSel = false;
static int  g_selR = 0;
static int  g_selC = 0;

// Mutari posibile pentru piesa selectata (maxim 4 diagonale)
static int g_movesR[4];
static int g_movesC[4];
static int g_moveCount = 0;

// Contor mutari (half-moves) + prag cand incepem eliminarea pieselor blocate
static int g_ply = 0;              // creste cu 1 dupa fiecare mutare
static int g_captureAfterPly = 8;  // 4 mutari alb + 4 mutari negru
static int g_maxPly = 200;         // limita de siguranta (poti schimba)

// Capturi: cate piese adverse au fost eliminate dupa mutarile fiecaruia
static int g_capturedByWhite = 0;  // cate piese NEGRE a scos ALB
static int g_capturedByBlack = 0;  // cate piese ALBE a scos NEGRU

// Timer pentru timpul scurs de la start/reset
static sf::Clock g_gameClock;


// ------------------------------ STARI ECRAN ------------------------------
// 0 = introdu nume, 1 = joc, 2 = final
static int g_phase = 0;


// ------------------------------ NUME JUCATORI ------------------------------
// Folosesc char[]
static char g_nameWhite[24] = "";
static char g_nameBlack[24] = "";
static int  g_lenWhite = 0;
static int  g_lenBlack = 0;
static int  g_activeName = 0; // 0 = editez numele albului, 1 = editez numele negrului

// Castigator: 0 remiza, 1 alb, 2 negru
static int g_winner = 0;


// ------------------------------ TEXT (SFML 3) ------------------------------
// In SFML 3, sf::Text NU are constructor default, deci il construiesc cu fontul.
static sf::Font g_font;
static bool g_fontReady = false;

static sf::Text g_hud(g_font);

static sf::Text g_title(g_font);
static sf::Text g_labelWhite(g_font);
static sf::Text g_labelBlack(g_font);
static sf::Text g_nameTextWhite(g_font);
static sf::Text g_nameTextBlack(g_font);
static sf::Text g_hint(g_font);

static sf::Text g_overTitle(g_font);
static sf::Text g_overWinner(g_font);
static sf::Text g_overHint(g_font);


// ------------------------------ LAYOUT / RAMA ------------------------------
static float g_tile = 100.f; // marimea unui patrat
static float g_frame = 30.f;  // grosimea ramei
static sf::Vector2f g_origin(0.f, 0.f); // coltul stanga-sus al tablei (doar tabla, fara rama)


// ------------------------------ CONFETTI (final) ------------------------------
// Efect ff simplu: patrate mici care cad
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
  Random simplu (LCG). Nu e pentru joc, doar pentru efectul vizual confetti
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
  Pune piesele in pozitia initiala standard.
  Observatie: randul 0 e sus (8), randul 7 e jos (1).
*/
static void boardSetupInitialPosition() {
    boardClear();

    // ALB (jos)
    for (int c = 0; c < 8; c += 2) g_board[7][c] = 1; // a1,c1,e1,g1
    for (int c = 1; c < 8; c += 2) g_board[6][c] = 1; // b2,d2,f2,h2

    // NEGRU (sus)
    for (int c = 1; c < 8; c += 2) g_board[0][c] = 2; // b8,d8,f8,h8
    for (int c = 0; c < 8; c += 2) g_board[1][c] = 2; // a7,c7,e7,g7
}

/*
  Reseteaza meciul (tabla + contori + timer), dar nu sterge numele.
  Il folosesc pentru "rematch".
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
}

/*
  Reseteaza tot si te duce in ecranul de introducere nume.
*/
static void resetAllToNameEntry() {
    g_phase = 0;

    g_lenWhite = 0;
    g_lenBlack = 0;
    g_nameWhite[0] = '\0';
    g_nameBlack[0] = '\0';
    g_activeName = 0;

    g_winner = 0;
    g_overClock.restart();

    resetMatchKeepNames();
}

/*
  Calculeaza g_origin astfel incat tabla sa fie centrata pe ecran.
*/
static void updateLayout(sf::RenderWindow& w) {
    const auto sz = w.getSize();
    const float boardPx = g_tile * 8.f;

    g_origin.x = (float(sz.x) - boardPx) / 2.f;
    g_origin.y = (float(sz.y) - boardPx) / 2.f;
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
  Genereaza mutarile legale pentru piesa din (fromR, fromC).
  Regula: 1 pas pe diagonala intr-o casuta libera, in orice directie.
  Returneaza cate mutari a gasit (0..4) si umple outR/outC.
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
  Pregateste particulele de confetti (pozitie, viteza, marime).
*/
static void initConfetti(sf::RenderWindow& w) {
    auto sz = w.getSize();
    float W = (float)sz.x;
    float H = (float)sz.y;

    // un seed mic ca sa arate diferit la fiecare final
    g_rng = 1u + (std::uint32_t)(g_gameClock.getElapsedTime().asMilliseconds() + 12345);

    for (int i = 0; i < CONF_N; i++) {
        g_confPos[i].x = rng01() * W;
        g_confPos[i].y = rng01() * H * 0.5f; // pornesc de sus
        g_confVel[i].x = (rng01() - 0.5f) * 40.f;
        g_confVel[i].y = 80.f + rng01() * 180.f;
        g_confSize[i] = 3.f + rng01() * 6.f;
    }

    g_overClock.restart();
}

/*
  Intra in starea de final (phase=2), calculeaza castigatorul si porneste confetti.
*/
static void enterGameOver(sf::RenderWindow& w) {
    g_phase = 2;
    g_winner = decideWinnerByPieces();
    initConfetti(w);
}

/*
  Verifica daca meciul s-a terminat.
  Cazuri tratate:
  - cineva are 0 piese
  - am ajuns la limita de mutari
  - unul dintre jucatori nu mai are mutari legale
*/
static void checkGameOver(sf::RenderWindow& w) {
    int wPieces = countPieces(1);
    int bPieces = countPieces(2);

    if (wPieces == 0 || bPieces == 0) { enterGameOver(w); return; }
    if (g_ply >= g_maxPly) { enterGameOver(w); return; }

    if (!hasAnyMove(1) || !hasAnyMove(2)) {
        enterGameOver(w);
        return;
    }
}

/*
  Elimina toate piesele blocate (fara mutari) simultan.
  Important: contorizez capturile doar pentru piesele adversarului eliminate.
*/
static void removeBlockedAll(std::uint8_t mover) {
    int remR[64], remC[64];
    std::uint8_t remPiece[64];
    int remCnt = 0;

    int tmpR[4], tmpC[4];

    // 1) colectez toate piesele blocate
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (g_board[r][c] == 0) continue;

            if (genMoves(r, c, tmpR, tmpC) == 0) {
                remR[remCnt] = r;
                remC[remCnt] = c;
                remPiece[remCnt] = g_board[r][c];
                remCnt++;
            }
        }
    }

    // 2) elimin simultan si numar cate au fost albe / negre
    int removedWhite = 0;
    int removedBlack = 0;

    for (int i = 0; i < remCnt; i++) {
        if (remPiece[i] == 1) removedWhite++;
        else if (remPiece[i] == 2) removedBlack++;

        g_board[remR[i]][remC[i]] = 0;
    }

    // 3) actualizez contorul de capturi (doar adversarul conteaza)
    if (mover == 1) g_capturedByWhite += removedBlack;
    else if (mover == 2) g_capturedByBlack += removedWhite;
}

/*
  Aplica o mutare (fr,fc)->(tr,tc):
  - mut piesa pe tabla
  - cresc contorul de mutari
  - dupa prag, aplic eliminarea pieselor blocate
  - schimb tura
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
        removeBlockedAll(mover);
    }

    g_turn = otherSide(g_turn);

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


// ============================================================================
//  Editare nume 
// ============================================================================

/*
  Adauga un caracter in numele ales (0=alb, 1=negru).
*/
static void appendCharToName(int which, char ch) {
    if (which == 0) {
        if (g_lenWhite >= 23) return;
        g_nameWhite[g_lenWhite++] = ch;
        g_nameWhite[g_lenWhite] = '\0';
    }
    else {
        if (g_lenBlack >= 23) return;
        g_nameBlack[g_lenBlack++] = ch;
        g_nameBlack[g_lenBlack] = '\0';
    }
}

/*
  Sterge ultimul caracter (Backspace).
*/
static void backspaceName(int which) {
    if (which == 0) {
        if (g_lenWhite <= 0) return;
        g_lenWhite--;
        g_nameWhite[g_lenWhite] = '\0';
    }
    else {
        if (g_lenBlack <= 0) return;
        g_lenBlack--;
        g_nameBlack[g_lenBlack] = '\0';
    }
}

/*
  Cand apas ENTER pe al doilea camp, pornesc jocul.
  Daca numele e gol, pun P1 / P2 ca fallback.
*/
static void startGameFromNames() {
    if (g_lenWhite == 0) { appendCharToName(0, 'P'); appendCharToName(0, '1'); }
    if (g_lenBlack == 0) { appendCharToName(1, 'P'); appendCharToName(1, '2'); }

    resetMatchKeepNames();
    g_phase = 1;
}


// ============================================================================
//  UI / TEXT (stiluri)
// ============================================================================

/*
  Setez stilurile pentru toate textele (marimi + culori).
  Apelez dupa ce fontul a fost incarcat cu succes.
*/
static void setupTextStyles() {
    if (!g_fontReady) return;

    g_hud.setCharacterSize(20);
    g_hud.setFillColor(sf::Color::Black);

    g_title.setCharacterSize(34);
    g_title.setFillColor(sf::Color::Black);

    g_labelWhite.setCharacterSize(22);
    g_labelWhite.setFillColor(sf::Color::Black);

    g_labelBlack.setCharacterSize(22);
    g_labelBlack.setFillColor(sf::Color::Black);

    g_nameTextWhite.setCharacterSize(28);
    g_nameTextWhite.setFillColor(sf::Color::Black);

    g_nameTextBlack.setCharacterSize(28);
    g_nameTextBlack.setFillColor(sf::Color::Black);

    g_hint.setCharacterSize(18);
    g_hint.setFillColor(sf::Color::Black);

    g_overTitle.setCharacterSize(42);
    g_overTitle.setFillColor(sf::Color::White);

    g_overWinner.setCharacterSize(54);
    g_overWinner.setFillColor(sf::Color::White);

    g_overHint.setCharacterSize(20);
    g_overHint.setFillColor(sf::Color(230, 230, 230));
}


// ============================================================================
//  DESENARE
// ============================================================================

/*
  Desenez o rama cu umbra
  Tabla propriu-zisa ramane 8x8 in interior
*/
static void drawFrame(sf::RenderWindow& w) {
    const float boardPx = g_tile * 8.f;

    // umbra
    sf::RectangleShape shadow({ boardPx + 2.f * g_frame + 8.f, boardPx + 2.f * g_frame + 8.f });
    shadow.setPosition({ g_origin.x - g_frame + 6.f, g_origin.y - g_frame + 6.f });
    shadow.setFillColor(sf::Color(0, 0, 0, 70));
    w.draw(shadow);

    // rama lemn
    sf::RectangleShape frame({ boardPx + 2.f * g_frame, boardPx + 2.f * g_frame });
    frame.setPosition({ g_origin.x - g_frame, g_origin.y - g_frame });
    frame.setFillColor(sf::Color(120, 70, 25));
    w.draw(frame);

    // linie interioara (accent)
    sf::RectangleShape innerLine({ boardPx + 2.f * (g_frame - 6.f), boardPx + 2.f * (g_frame - 6.f) });
    innerLine.setPosition({ g_origin.x - (g_frame - 6.f), g_origin.y - (g_frame - 6.f) });
    innerLine.setFillColor(sf::Color::Transparent);
    innerLine.setOutlineThickness(3.f);
    innerLine.setOutlineColor(sf::Color(220, 180, 120, 180));
    w.draw(innerLine);
}

/*
  Desenez tabla 8x8 patrate alternante
*/
static void drawBoard(sf::RenderWindow& w) {
    sf::RectangleShape sq({ g_tile, g_tile });
    sf::Color light(235, 235, 235);
    sf::Color dark(120, 120, 120);

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
  Desenez highlight-urile:
  - galben pentru destinatiile posibile
  - albastru pentru patratul selectat
*/
static void drawHighlights(sf::RenderWindow& w) {
    sf::RectangleShape hl({ g_tile, g_tile });
    hl.setFillColor(sf::Color(255, 215, 0, 110));

    for (int i = 0; i < g_moveCount; i++) {
        hl.setPosition({ g_origin.x + g_movesC[i] * g_tile, g_origin.y + g_movesR[i] * g_tile });
        w.draw(hl);
    }

    if (g_hasSel) {
        sf::RectangleShape selBox({ g_tile, g_tile });
        selBox.setFillColor(sf::Color::Transparent);
        selBox.setOutlineColor(sf::Color(0, 180, 255));
        selBox.setOutlineThickness(5.f);
        selBox.setPosition({ g_origin.x + g_selC * g_tile, g_origin.y + g_selR * g_tile });
        w.draw(selBox);
    }
}

/*
  Desenez piesele ca cercuri, cu outline sa se vada bine pe orice patrat
*/
static void drawPieces(sf::RenderWindow& w) {
    float rad = g_tile * 0.38f;
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
                p.setFillColor(sf::Color(245, 245, 245));
                p.setOutlineColor(sf::Color::Black);
                p.setOutlineThickness(3.f);
            }
            else {
                p.setFillColor(sf::Color(35, 35, 35));
                p.setOutlineColor(sf::Color::White);
                p.setOutlineThickness(3.f);
            }

            p.setPosition(center);
            w.draw(p);
        }
    }
}

/*
  HUD: tura + nume + capturi + piese ramase + timp
  Daca fontul nu s-a incarcat, nu desenez textul sa nu crape
*/
static void drawHUD(sf::RenderWindow& w) {
    if (!g_fontReady) return;

    int sec = (int)g_gameClock.getElapsedTime().asSeconds();
    int mm = sec / 60;
    int ss = sec % 60;

    int wPieces = countPieces(1);
    int bPieces = countPieces(2);

    char buf[256];
    const char* turnName = (g_turn == 1) ? "ALB" : "NEGRU";

    std::snprintf(
        buf, sizeof(buf),
        "Tura: %s | %s(ALB) capturi:%d piese:%d  vs  %s(NEGRU) capturi:%d piese:%d | Timp %02d:%02d",
        turnName,
        g_nameWhite, g_capturedByWhite, wPieces,
        g_nameBlack, g_capturedByBlack, bPieces,
        mm, ss
    );

    g_hud.setString(buf);
    g_hud.setPosition({ g_origin.x - g_frame, g_origin.y - g_frame - 40.f });
    w.draw(g_hud);
}

/*
  Ecran de introducere nume - un panel peste fundal
*/
static void drawNameEntry(sf::RenderWindow& w) {
    if (!g_fontReady) return;

    auto sz = w.getSize();
    float W = (float)sz.x;
    float H = (float)sz.y;

    // overlay usor
    sf::RectangleShape dim({ W, H });
    dim.setFillColor(sf::Color(0, 0, 0, 60));
    w.draw(dim);

    // panel
    sf::RectangleShape panel({ 820.f, 420.f });
    panel.setPosition({ (W - 820.f) / 2.f, (H - 420.f) / 2.f });
    panel.setFillColor(sf::Color(245, 235, 220));
    panel.setOutlineThickness(4.f);
    panel.setOutlineColor(sf::Color(120, 70, 25));
    w.draw(panel);

    float px = panel.getPosition().x;
    float py = panel.getPosition().y;

    g_title.setString("Introdu numele jucatorilor");
    g_title.setPosition({ px + 40.f, py + 30.f });
    w.draw(g_title);

    g_labelWhite.setString("Player 1 (ALB):");
    g_labelWhite.setPosition({ px + 40.f, py + 120.f });
    w.draw(g_labelWhite);

    g_labelBlack.setString("Player 2 (NEGRU):");
    g_labelBlack.setPosition({ px + 40.f, py + 230.f });
    w.draw(g_labelBlack);

    // box input alb
    sf::RectangleShape box1({ 740.f, 60.f });
    box1.setPosition({ px + 40.f, py + 155.f });
    box1.setFillColor(sf::Color::White);
    box1.setOutlineThickness(3.f);
    box1.setOutlineColor(g_activeName == 0 ? sf::Color(0, 180, 255) : sf::Color(170, 170, 170));
    w.draw(box1);

    // box input negru
    sf::RectangleShape box2({ 740.f, 60.f });
    box2.setPosition({ px + 40.f, py + 265.f });
    box2.setFillColor(sf::Color::White);
    box2.setOutlineThickness(3.f);
    box2.setOutlineColor(g_activeName == 1 ? sf::Color(0, 180, 255) : sf::Color(170, 170, 170));
    w.draw(box2);

    g_nameTextWhite.setString(g_nameWhite);
    g_nameTextWhite.setPosition({ px + 55.f, py + 165.f });
    w.draw(g_nameTextWhite);

    g_nameTextBlack.setString(g_nameBlack);
    g_nameTextBlack.setPosition({ px + 55.f, py + 275.f });
    w.draw(g_nameTextBlack);

    // cursor simplu (linie) la finalul campului activ
    sf::RectangleShape caret({ 2.f, 32.f });
    caret.setFillColor(sf::Color::Black);

    if (g_activeName == 0) {
        auto b = g_nameTextWhite.getLocalBounds(); // SFML 3: FloatRect are .size
        caret.setPosition({ g_nameTextWhite.getPosition().x + b.size.x + 6.f,
                            g_nameTextWhite.getPosition().y + 6.f });
        w.draw(caret);
    }
    else {
        auto b = g_nameTextBlack.getLocalBounds();
        caret.setPosition({ g_nameTextBlack.getPosition().x + b.size.x + 6.f,
                            g_nameTextBlack.getPosition().y + 6.f });
        w.draw(caret);
    }

    g_hint.setString("Taste: scrie nume, TAB schimba campul, ENTER continua, BACKSPACE sterge, ESC meniu");
    g_hint.setPosition({ px + 40.f, py + 350.f });
    w.draw(g_hint);
}

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

        // un wrap simplu ca sa nu dispara complet
        while (x < -20.f) x += (W + 40.f);
        while (x > W + 20.f) x -= (W + 40.f);
        while (y > H + 20.f) y -= (H + 60.f);

        float s = g_confSize[i];
        piece.setSize({ s, s });
        piece.setPosition({ x, y });

        // SFML 3: trebuie sf::Angle
        piece.setRotation(sf::degrees(t * 120.f + i * 7.f));

        sf::Color col((std::uint8_t)((i * 50) % 255),
            (std::uint8_t)((i * 90) % 255),
            (std::uint8_t)((i * 140) % 255), 220);
        piece.setFillColor(col);

        w.draw(piece);
    }
}

/*
  Trofeu simplu din forme (ca sa para mai "festiv").
*/
static void drawTrophy(sf::RenderWindow& w, sf::Vector2f center) {
    sf::ConvexShape cup(6);
    cup.setPoint(0, { -90.f, -80.f });
    cup.setPoint(1, { 90.f, -80.f });
    cup.setPoint(2, { 60.f,  10.f });
    cup.setPoint(3, { 20.f,  45.f });
    cup.setPoint(4, { -20.f,  45.f });
    cup.setPoint(5, { -60.f,  10.f });
    cup.setPosition(center);
    cup.setFillColor(sf::Color(255, 215, 0, 230));
    w.draw(cup);

    sf::CircleShape h1(18.f);
    h1.setFillColor(sf::Color(255, 215, 0, 200));
    h1.setPosition({ center.x - 130.f, center.y - 55.f });
    w.draw(h1);

    sf::CircleShape h2(18.f);
    h2.setFillColor(sf::Color(255, 215, 0, 200));
    h2.setPosition({ center.x + 94.f, center.y - 55.f });
    w.draw(h2);

    sf::RectangleShape base({ 180.f, 22.f });
    base.setFillColor(sf::Color(120, 70, 25, 230));
    base.setPosition({ center.x - 90.f, center.y + 70.f });
    w.draw(base);

    sf::RectangleShape base2({ 110.f, 26.f });
    base2.setFillColor(sf::Color(90, 50, 18, 230));
    base2.setPosition({ center.x - 55.f, center.y + 44.f });
    w.draw(base2);
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

    sf::RectangleShape banner({ 900.f, 320.f });
    banner.setPosition({ (W - 900.f) / 2.f, (H - 320.f) / 2.f });
    banner.setFillColor(sf::Color(20, 20, 20, 220));
    banner.setOutlineThickness(4.f);
    banner.setOutlineColor(sf::Color(255, 215, 0, 220));
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

    g_overHint.setString("R = rematch | ENTER = schimba nume | ESC = meniu");
    g_overHint.setPosition({ bx + 40.f, by + 260.f });
    w.draw(g_overHint);

    drawTrophy(w, { bx + 760.f, by + 150.f });
}


// ============================================================================
//  FUNCTII PUBLICE (apelate din main.cpp)
// ============================================================================

/*
  Initializare modul Play:
  - incarc fontul o singura data
  - setez stilurile textelor
  - pornesc in ecranul de nume
*/
void Play_Init() {
    if (!g_fontReady) {
        g_fontReady = g_font.openFromFile("Themes/Kanit-Medium.ttf");
        setupTextStyles();
    }
    resetAllToNameEntry();
}

/*
  Reset cand intri in Play (de obicei imediat dupa ce apesi Play din meniu).
*/
void Play_Reset() {
    resetAllToNameEntry();
}

/*
  Handler de evenimente:
  - ESC: inapoi la meniu
  - In phase 0: scriu nume (TextEntered), TAB schimba campul, ENTER continua
  - In phase 1: click pentru select/move, R pentru rematch cu aceleasi nume
  - In phase 2: R rematch, ENTER inapoi la nume
*/
void Play_HandleEvent(const sf::Event& e, sf::RenderWindow& w) {
    // ESC merge in orice faza: ies din play
    if (const auto* k = e.getIf<sf::Event::KeyPressed>()) {
        if (k->code == sf::Keyboard::Key::Escape) {
            audio.playClick();
            g_hasSel = false;
            g_moveCount = 0;
            interfata = 1;
            return;
        }
    }

    // ------------------- FAZA 0: NUME -------------------
    if (g_phase == 0) {
        if (const auto* k = e.getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::Tab) {
                audio.playClick();
                g_activeName = 1 - g_activeName;
                return;
            }
            if (k->code == sf::Keyboard::Key::Backspace) {
                audio.playClick();
                backspaceName(g_activeName);
                return;
            }
            if (k->code == sf::Keyboard::Key::Enter) {
                audio.playClick();
                // prima apasare ENTER -> trec la campul 2, a doua -> incep jocul
                if (g_activeName == 0) g_activeName = 1;
                else startGameFromNames();
                return;
            }
        }

        // TextEntered: ia caracterele scrise
        if (const auto* te = e.getIf<sf::Event::TextEntered>()) {
            char32_t u = te->unicode;
            // accepta doar ASCII simplu
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

    // ------------------- FAZA 2: FINAL -------------------
    if (g_phase == 2) {
        if (const auto* k = e.getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::R) {
                audio.playClick();
                resetMatchKeepNames();
                g_phase = 1;
                return;
            }
            if (k->code == sf::Keyboard::Key::Enter) {
                audio.playClick();
                resetAllToNameEntry();
                return;
            }
        }
        return;
    }

    // ------------------- FAZA 1: JOC -------------------
    if (const auto* k = e.getIf<sf::Event::KeyPressed>()) {
        if (k->code == sf::Keyboard::Key::R) {
            resetMatchKeepNames();
            return;
        }
    }

    if (const auto* mb = e.getIf<sf::Event::MouseButtonPressed>()) {
        audio.playClick();
        if (mb->button != sf::Mouse::Button::Left) return;

        updateLayout(w);

        int r, c;
        if (!mouseToCell(w, r, c)) {
            g_hasSel = false;
            g_moveCount = 0;
            return;
        }

        clickCellInGame(r, c, w);
    }
}

/*
  Functia de desen:
  - In phase 0: desenez panelul de nume
  - In phase 1: desenez rama + tabla + piese + HUD
  - In phase 2: peste joc desenez overlay-ul de final
*/
void Play_Draw(sf::RenderWindow& w) {
    updateLayout(w);

    if (g_phase == 0) {
        drawNameEntry(w);
        return;
    }

    drawFrame(w);
    drawBoard(w);
    drawHighlights(w);
    drawPieces(w);
    drawHUD(w);

    if (g_phase == 2) {
        drawGameOver(w);
    }
}
