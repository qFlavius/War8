#include "Translations.hpp"

Language g_currentLanguage = English;

void InitTranslations() {
    SetTranslationLanguage(English);
}

void SetTranslationLanguage(Language lang) {
    g_currentLanguage = lang;
}

const char* GetTranslation(TranslationKey key) {
    if (g_currentLanguage == English) {
        switch (key) {
            // Main Menu
            case TR_MENU_TITLE: return "War8";
            case TR_MENU_PLAY: return "Play";
            case TR_MENU_LEARN: return "Learn";
            case TR_MENU_LEADERBOARD: return "Leaderboard";
            case TR_MENU_THEME: return "Themes";
            case TR_MENU_SETTINGS: return "Settings";
            case TR_MENU_EXIT: return "Exit";
            
            // Play Screen - Mode Selection
            case TR_MODE_SELECT_TITLE: return "Choose game mode";
            case TR_MODE_PVP: return "1) Player vs Player";
            case TR_MODE_PVC: return "2) Player vs Computer";
            case TR_MODE_CVC: return "3) Computer vs Computer";
            case TR_MODE_HINT: return "Click on a mode or press 1/2/3. ENTER continues. ESC menu";
            
            // Play Screen - Name Entry
            case TR_NAME_ENTRY_TITLE: return "Enter names";
            case TR_NAME_WHITE: return "WHITE:";
            case TR_NAME_BLACK: return "BLACK:";
            case TR_NAME_HINT: return "Keys: TAB switch field, ENTER continue, BACKSPACE delete, B back, ESC menu";
            
            // Play Screen - Options
            case TR_OPTIONS_TITLE: return "Game Options";
            case TR_OPTIONS_TIMER: return "Turn timer:";
            case TR_OPTIONS_TIMER_ON: return "Turn timer: ON";
            case TR_OPTIONS_TIMER_OFF: return "Turn timer: OFF";
            case TR_OPTIONS_COMPUTER_WHITE: return "Computer White:";
            case TR_OPTIONS_COMPUTER_WHITE_EASY: return "Computer White: Easy";
            case TR_OPTIONS_COMPUTER_WHITE_HARD: return "Computer White: Hard";
            case TR_OPTIONS_COMPUTER_BLACK: return "Computer Black:";
            case TR_OPTIONS_COMPUTER_BLACK_EASY: return "Computer Black: Easy";
            case TR_OPTIONS_COMPUTER_BLACK_HARD: return "Computer Black: Hard";
            case TR_OPTIONS_BACK: return "B) Back";
            case TR_OPTIONS_START: return "ENTER) Start";
            case TR_OPTIONS_HINT: return "Click on options. ENTER Start. B back. ESC menu";
            
            // Play Screen - Game HUD
            case TR_HUD_TURN: return "Turn";
            case TR_HUD_WHITE: return "WHITE";
            case TR_HUD_BLACK: return "BLACK";
            case TR_HUD_PIECES: return "Pieces:";
            case TR_HUD_CAPTURES: return "Captures:";
            
            // Play Screen - Game Over
            case TR_GAMEOVER_TITLE: return "GAME OVER";
            case TR_GAMEOVER_WINNER: return "Winner:";
            case TR_GAMEOVER_DRAW: return "RESULT: DRAW";
            case TR_GAMEOVER_REASON: return "Reason:";
            case TR_GAMEOVER_HINT: return "R = rematch | ENTER = setup | ESC = menu";
            case TR_GAMEOVER_TIMEOUT: return "Time out";
            
            // Settings
            case TR_SETTINGS_BACK: return "Back";
            case TR_SETTINGS_FPS: return "FPS";
            case TR_SETTINGS_SOUND: return "Sound Effects";
            case TR_SETTINGS_MUSIC: return "Music";
            case TR_SETTINGS_RESET: return "Reset";
            case TR_SETTINGS_LANGUAGE: return "Language";
            case TR_SETTINGS_LANGUAGE_EN: return "English";
            case TR_SETTINGS_LANGUAGE_RO: return "Romanian";
            case TR_SETTINGS_ON: return "ON";
            case TR_SETTINGS_OFF: return "OFF";
            
            // Leaderboard
            case TR_LEADERBOARD_TITLE: return "Top Wins";
            case TR_LEADERBOARD_BACK: return "Back";
            
            // Themes
            case TR_THEMES_BACK: return "Back";
            case TR_THEMES_SELECT: return "Select";
            case TR_THEMES_CONFIRM: return "Confirm";
            case TR_THEMES_DEFAULT: return "Default";
            case TR_THEMES_DEFAULT_DARK: return "Default Dark";
            case TR_THEMES_BW: return "B & W";
            case TR_THEMES_CUSTOM: return "Custom";
            
            // Learn
            case TR_LEARN_BACK: return "Back";
            case TR_LEARN_PAGE1_INTRO: return "\"War8\" is a rational strategy game, similar to chess or checkers, but with a unique piece capture mechanism. The goal is not to jump over pieces, but to surround and block them.";
            case TR_LEARN_PAGE1_OBJECTIVE_TITLE: return "1. Game Objective";
            case TR_LEARN_PAGE1_OBJECTIVE: return "\tThe main goal is to eliminate the opponent's \"army\". The player who manages to eliminate more of the opponent's pieces wins, meaning the one who remains with more pieces on the board at the end of the game.";
            case TR_LEARN_PAGE1_SETUP_TITLE: return "2. Game Preparation (Setup)";
            case TR_LEARN_PAGE1_SETUP: return "\tBoard: A standard chess board (8x8 squares) is used.\n\tPieces: Each player has 8 pieces (pawns), of different colors (White and Black).\n\tInitial placement: Pieces are placed on the first two rows of each player, but only on squares of the same color (for example, only on black or only on white), leaving a \"buffer\" space between armies.";
            case TR_LEARN_PAGE2_TITLE: return "3. How pieces move";
            case TR_LEARN_PAGE2_MOVEMENT: return "\tPlayers move alternately, one piece per turn.\n\tBasic rule: A piece can move one square, only diagonally.\n\tDirection: Unlike classic checkers, in \"War8\" a piece can move in any direction and sense (both forward and backward), as long as the adjacent diagonal square is free.";
            case TR_LEARN_PAGE3_TITLE: return "4. Piece Elimination (Prisoner Rule)";
            case TR_LEARN_PAGE3_CAPTURE: return "\tThis is the main tactical element of the game. A piece is not captured by jumping, but by blocking.\n\tWhen is a piece eliminated? A piece is considered \"blocked\" (made prisoner) if it no longer has any legal move available. This means that all adjacent diagonal squares are occupied either by own pieces, opponent pieces, or the board edge.\n\tRemoval from play: The moment a piece has been blocked as a result of the opponent's move (or even an unwise own move), it is immediately removed from the board and does not re-enter play.\n\n\tExample: If a white piece moves and occupies the only free square next to a black piece, the black piece is blocked and eliminated.";
            case TR_LEARN_PAGE4_TITLE: return "5. Special Rules and Strategy";
            case TR_LEARN_PAGE4_RULES: return "\tOpening (First moves): At the start of the game, pieces on the last row of each player are technically \"blocked\" by their own pieces in front. However, they are not eliminated at start. It is recommended that in the first 4 moves the elimination rule should not be applied, to allow players to develop their position and unlock their own pieces.\n\tAvoiding Stalemate (Non-sense): Sometimes, players can create a median \"wall\" that no one can break through (a symmetric and passive development), leading to a boring game or a draw. Players are encouraged to play offensively to avoid these \"stalemate\" situations.";
            case TR_LEARN_PAGE5_TITLE: return "6. End of Game";
            case TR_LEARN_PAGE5_END: return "The game can end in two ways:";
            case TR_LEARN_PAGE5_VICTORY: return "\tVictory:";
            case TR_LEARN_PAGE5_VICTORY_DESC: return " A player has eliminated all opponent pieces or the opponent concedes.";
            case TR_LEARN_PAGE5_SCORE: return "\tScore/Time limit:";
            case TR_LEARN_PAGE5_SCORE_DESC: return " Since games can be long, a move limit can be set (e.g.: 50 moves). After the limit expires, the player who remains with the most pieces on the board wins.";
            default: return "";
        }
    }
    else { // Romanian
        switch (key) {
            // Main Menu
            case TR_MENU_TITLE: return "Razboi in 8";
            case TR_MENU_PLAY: return "Joc";
            case TR_MENU_LEARN: return "Invatare";
            case TR_MENU_LEADERBOARD: return "Clasament";
            case TR_MENU_THEME: return "Teme";
            case TR_MENU_SETTINGS: return "Setari";
            case TR_MENU_EXIT: return "Iesire";
            
            // Play Screen - Mode Selection
            case TR_MODE_SELECT_TITLE: return "Alege modul de joc";
            case TR_MODE_PVP: return "1) Jucator vs Jucator";
            case TR_MODE_PVC: return "2) Jucator vs Computer";
            case TR_MODE_CVC: return "3) Computer vs Computer";
            case TR_MODE_HINT: return "Click pe un mod sau tastele 1/2/3. ENTER continua. ESC meniu";
            
            // Play Screen - Name Entry
            case TR_NAME_ENTRY_TITLE: return "Introdu numele";
            case TR_NAME_WHITE: return "ALB:";
            case TR_NAME_BLACK: return "NEGRU:";
            case TR_NAME_HINT: return "Taste: TAB schimba campul, ENTER continua, BACKSPACE sterge, B inapoi, ESC meniu";
            
            // Play Screen - Options
            case TR_OPTIONS_TITLE: return "Optiuni joc";
            case TR_OPTIONS_TIMER: return "Timer pe tura:";
            case TR_OPTIONS_TIMER_ON: return "Timer pe tura: ON";
            case TR_OPTIONS_TIMER_OFF: return "Timer pe tura: OFF";
            case TR_OPTIONS_COMPUTER_WHITE: return "Computer Alb:";
            case TR_OPTIONS_COMPUTER_WHITE_EASY: return "Computer Alb: Easy";
            case TR_OPTIONS_COMPUTER_WHITE_HARD: return "Computer Alb: Hard";
            case TR_OPTIONS_COMPUTER_BLACK: return "Computer Negru:";
            case TR_OPTIONS_COMPUTER_BLACK_EASY: return "Computer Negru: Easy";
            case TR_OPTIONS_COMPUTER_BLACK_HARD: return "Computer Negru: Hard";
            case TR_OPTIONS_BACK: return "B) Inapoi";
            case TR_OPTIONS_START: return "ENTER) Start";
            case TR_OPTIONS_HINT: return "Click pe optiuni. ENTER Start. B inapoi. ESC meniu";
            
            // Play Screen - Game HUD
            case TR_HUD_TURN: return "Tura";
            case TR_HUD_WHITE: return "ALB";
            case TR_HUD_BLACK: return "NEGRU";
            case TR_HUD_PIECES: return "Piese:";
            case TR_HUD_CAPTURES: return "Capturi:";
            
            // Play Screen - Game Over
            case TR_GAMEOVER_TITLE: return "FINAL";
            case TR_GAMEOVER_WINNER: return "Castigator:";
            case TR_GAMEOVER_DRAW: return "REZULTAT: REMIZA";
            case TR_GAMEOVER_REASON: return "Motiv:";
            case TR_GAMEOVER_HINT: return "R = rematch | ENTER = setup | ESC = meniu";
            case TR_GAMEOVER_TIMEOUT: return "Time out";
            
            // Settings
            case TR_SETTINGS_BACK: return "Inapoi";
            case TR_SETTINGS_FPS: return "FPS";
            case TR_SETTINGS_SOUND: return "Efecte Sonore";
            case TR_SETTINGS_MUSIC: return "Muzica";
            case TR_SETTINGS_RESET: return "Resetare";
            case TR_SETTINGS_LANGUAGE: return "Limba";
            case TR_SETTINGS_LANGUAGE_EN: return "Engleza";
            case TR_SETTINGS_LANGUAGE_RO: return "Romana";
            case TR_SETTINGS_ON: return "ON";
            case TR_SETTINGS_OFF: return "OFF";
            
            // Leaderboard
            case TR_LEADERBOARD_TITLE: return "Top Victorii";
            case TR_LEADERBOARD_BACK: return "Inapoi";
            
            // Themes
            case TR_THEMES_BACK: return "Inapoi";
            case TR_THEMES_SELECT: return "Selecteaza";
            case TR_THEMES_CONFIRM: return "Confirma";
            case TR_THEMES_DEFAULT: return "Implicit";
            case TR_THEMES_DEFAULT_DARK: return "Intunecat";
            case TR_THEMES_BW: return "Alb & Negru";
            case TR_THEMES_CUSTOM: return "Personalizat";
            
            // Learn
            case TR_LEARN_BACK: return "Inapoi";
            case TR_LEARN_PAGE1_INTRO: return "\"Razboi in 8\" este un joc de strategie rationala, similar cu sahul sau damele, dar cu un mecanism unic de capturare a pieselor. Scopul nu este sa sari peste piese, ci sa le incercuiesti si sa le blochezi.";
            case TR_LEARN_PAGE1_OBJECTIVE_TITLE: return "1. Obiectivul Jocului";
            case TR_LEARN_PAGE1_OBJECTIVE: return "\tScopul principal este sa scoti din lupta \"armata\" adversa. Castiga jucatorul care reuseste sa elimine mai multe piese ale adversarului, adica cel care ramane cu mai multe piese pe tabla la finalul jocului.";
            case TR_LEARN_PAGE1_SETUP_TITLE: return "2. Pregatirea Jocului (Setup)";
            case TR_LEARN_PAGE1_SETUP: return "\tTabla: Se foloseste o tabla standard de sah (8x8 patratele).\n\tPiesele: Fiecare jucator are cate 8 piese (pioni), de culori diferite (Alb si Negru).\n\tAsezarea initiala: Piesele se aseaza pe primele doua randuri ale fiecarui jucator, dar numai pe patratelele de aceeasi culoare (de exemplu, doar pe cele negre sau doar pe cele albe), lasand un spatiu \"tampon\" intre armate.";
            case TR_LEARN_PAGE2_TITLE: return "3. Cum se muta piesele";
            case TR_LEARN_PAGE2_MOVEMENT: return "\tJucatorii muta alternativ, cate o singura piesa per tura.\n\tRegula de baza: O piesa se poate muta un singur patratel, doar pe diagonala.\n\tDirectia: Spre deosebire de jocul de dame clasic, in \"Razboi in 8\" piesa se poate deplasa in orice directie si sens (atat inainte, cat si inapoi), atata timp cat patratelul vecin de pe diagonala este liber.";
            case TR_LEARN_PAGE3_TITLE: return "4. Eliminarea Pieselor (Regula Prizonieratului)";
            case TR_LEARN_PAGE3_CAPTURE: return "\tAcesta este elementul tactic principal al jocului. O piesa nu este capturata prin saritura, ci prin blocare.\n\tCand este o piesa eliminata? O piesa este considerata \"blocata\" (facuta prizoniera) daca nu mai are nicio mutare legala disponibila. Asta inseamna ca toate patratelele vecine de pe diagonala sunt ocupate fie de piese proprii, fie de piese adverse, fie de marginea tablei.\n\tScoaterea din joc: In momentul in care o piesa a fost blocata in urma mutarii adversarului (sau chiar a propriei mutari neispirate), ea este scoasa imediat de pe tabla si nu mai reintra in joc.\n\n\tExemplu: Daca o piesa alba muta si ocupa singurul patratel liber de langa o piesa neagra, piesa neagra este blocata si eliminata.";
            case TR_LEARN_PAGE4_TITLE: return "5. Reguli Speciale si Strategie";
            case TR_LEARN_PAGE4_RULES: return "\tDebutul (Primele mutari): La inceputul jocului, piesele de pe ultimul rand al fiecarui jucator sunt tehnic \"blocate\" de propriile piese din fata. Totusi, ele nu se elimina la start. Se recomanda ca in primele 4 mutari sa nu se aplice regula eliminarii, pentru a permite jucatorilor sa-si dezvolte pozitia si sa-si deblocheze propriile piese.\n\tEvitarea Blocajului (Non-sens): Uneori, jucatorii pot crea un \"zid\" median pe care nimeni nu-l poate strapunge (o dezvoltare simetrica si pasiva), ducand la un joc plictisitor sau la remiza. Jucatorii sunt incurajati sa joace ofensiv pentru a evita aceste situatii de \"pat\".";
            case TR_LEARN_PAGE5_TITLE: return "6. Finalul Jocului";
            case TR_LEARN_PAGE5_END: return "Partida se poate incheia in doua moduri:";
            case TR_LEARN_PAGE5_VICTORY: return "\tVictorie:";
            case TR_LEARN_PAGE5_VICTORY_DESC: return " Un jucator a eliminat toate piesele adversarului sau adversarul cedeaza.";
            case TR_LEARN_PAGE5_SCORE: return "\tScor/Limita de timp:";
            case TR_LEARN_PAGE5_SCORE_DESC: return " Deoarece partidele pot fi lungi, se poate stabili o limita de mutari (ex: 50 de mutari). Dupa expirarea limitei, castiga jucatorul care a ramas cu cele mai multe piese pe tabla.";
            default: return "";
        }
    }
}
