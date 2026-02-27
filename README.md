Based on the rules of “Razboi in 8” (War in Eight) from Nicolae Oprisiu’s "Olimpiada Jocurilor Rationale" ("The Olympiad of Rational Games", Chapter 10).

A turn-based strategy board game built in C++ with SFML, featuring multiple game modes (Player vs Player, Player vs Computer, Computer vs Computer), an optional per-turn timer (MM:SS), and a detailed HUD (current turn, remaining time, and piece/capture stats).
The AI supports Easy (one-ply heuristic evaluation) and Hard (time-bounded minimax with alpha–beta pruning). I also implemented clean game-state management, input handling (mouse-to-board coordinate mapping), and file-based leaderboard persistence.

What I learned: event-driven programming and rendering with SFML, designing a state-machine UI flow, implementing game rules and edge cases, building game AI (heuristics + minimax/alpha–beta), file I/O for persistence, and organizing a medium-size C++ project with Git.
