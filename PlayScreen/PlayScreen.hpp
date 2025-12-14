#ifndef PLAYSCREEN_HPP
#define PLAYSCREEN_HPP

#include <SFML/Graphics.hpp>

// variabila globala din proiect (interfata 1=meniu, 2=play)
extern int interfata;

void Play_Init();
void Play_Reset();
void Play_HandleEvent(const sf::Event& e, sf::RenderWindow& w);
void Play_Draw(sf::RenderWindow& w);

#endif
