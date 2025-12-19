#pragma once

extern int interfata;

//inline - copiaza functia si o pune in main -> Timp de executie mai mic
inline void CenterText(sf::Text& text, const sf::RectangleShape& rect) {
    sf::FloatRect textBounds = text.getLocalBounds();

    // Pune originea in mijlocul textului, in loc de stanga sus(default)
    text.setOrigin({
        textBounds.position.x + textBounds.size.x / 2.0f,
        textBounds.position.y + textBounds.size.y / 2.0f
        });
    //Seteaza originea in mijlocul dreptunghiului
    text.setPosition({
        rect.getPosition().x + rect.getSize().x / 2.0f,
        rect.getPosition().y + rect.getSize().y / 2.0f
        });
}