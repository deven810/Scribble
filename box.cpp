#include "box.h"
#include "scribble.h"

Box::Box()
{
    m_box.setSize(sf::Vector2f(COLOR_BLOCK_SIDE, COLOR_BLOCK_SIDE));
    m_box.setFillColor(sf::Color::Yellow);
}

Box::Box(sf::Vector2f position, sf::Color fill)
{
    m_box.setSize(sf::Vector2f(COLOR_BLOCK_SIDE, COLOR_BLOCK_SIDE));
    m_box.setFillColor(fill);
    m_box.setPosition(position);
}

Box::Box(unsigned int x, unsigned int y, sf::Color fill)
{
    m_box.setSize(sf::Vector2f(COLOR_BLOCK_SIDE, COLOR_BLOCK_SIDE));
    m_box.setFillColor(fill);
    m_box.setPosition(sf::Vector2f(x, y));
}

Box::~Box()
{
}

sf::RectangleShape Box::getShape() const
{
    return m_box;
}

sf::Color Box::getColor() const
{
    return m_box.getFillColor();
}

void Box::setPosition(sf::Vector2f pos)
{
    m_box.setPosition(pos);
}
sf::Vector2f Box::getPosition() const
{
    return m_box.getPosition();
}

bool Box::operator==(const Box &other)
{
    return (this->m_box.getPosition() == other.getPosition()) &
           (this->getColor() == other.getColor()) & 
           (this != &other);
}

bool Box::operator!=(const Box &other)
{
    return !((*this) == other) & (this != &other);
}

bool Box::operator^=(const Box &other)
{
    return (this->m_box.getPosition() == other.getPosition()) &
           (this != &other);
}