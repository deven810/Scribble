#ifndef BOX_H
#define BOX_H
#include <SFML/Graphics.hpp>

class Box
{
public:
    Box();
    Box(sf::Vector2f position, sf::Color fill);
    Box(unsigned int x, unsigned int y, sf::Color);
    ~Box();
    void setPosition(sf::Vector2f pos); 
    sf::Vector2f getPosition() const;
    sf::RectangleShape getShape() const;
    sf::Color getColor() const; 
    bool operator==(const Box &other);
    bool operator!=(const Box &other);
    bool operator^=(const Box &other); // True if two boxes are at the same position


private:
    sf::RectangleShape m_box;
};

#endif
