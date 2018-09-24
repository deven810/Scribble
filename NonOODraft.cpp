#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>

int m_scale = 50;
int m_color_block_width = 60;
int m_color_block_height = 80;
int m_numOfColors = 3;

void draw_grid(sf::RenderWindow &window)
{
    sf::Vector2u dim(window.getSize());

    int horizontal_lines = dim.x / m_scale - 1;
    int vertical_lines = dim.y / m_scale - 1;
    sf::VertexArray vertical(sf::Lines, vertical_lines * 2);
    sf::VertexArray horizontal(sf::Lines, horizontal_lines * 2);

    for (size_t i = 0; i < horizontal_lines * 2; i += 2)
    {
        horizontal[i].position = sf::Vector2f(0, m_scale * (i / 2 + 1));
        horizontal[i + 1].position = sf::Vector2f(dim.x, m_scale * (i / 2 + 1));
        if (i < (horizontal_lines * 2 - 2))
        {
            horizontal[i].color = sf::Color(35, 35, 35);
            horizontal[i + 1].color = sf::Color(35, 35, 35);
        }
        else
        {
            horizontal[i].color = sf::Color(145, 145, 145);
            horizontal[i + 1].color = sf::Color(145, 145, 145);
        }
    }
    for (size_t i = 0; i < vertical_lines * 2; i += 2)
    {
        vertical[i].position = sf::Vector2f(m_scale * (i / 2 + 1), 0);
        vertical[i + 1].position = sf::Vector2f(m_scale * (i / 2 + 1), dim.y);
        vertical[i].color = sf::Color(35, 35, 35);
        vertical[i + 1].color = sf::Color(35, 35, 35);
    }

    window.draw(horizontal);
    window.draw(vertical);
}

void draw_boxes(sf::RenderWindow &window, std::vector<sf::CircleShape> &color_boxes)
{
    for (std::vector<sf::CircleShape>::iterator i = color_boxes.begin(); i != color_boxes.end(); ++i)
    {
        window.draw(*i);
    }
}

void fill_palette(sf::RectangleShape *paletteBoxes, sf::Color *palette, sf::RenderWindow &pal_win)
{
    sf::RectangleShape color_block(sf::Vector2f(m_color_block_width, m_color_block_height));
    palette[0] = sf::Color::Green;
    palette[1] = sf::Color::Cyan;
    palette[2] = sf::Color::Yellow;

    for (size_t i = 0; i < m_numOfColors; i++)
    {
        paletteBoxes[i] = (color_block);
        paletteBoxes[i].setFillColor(palette[i]);
        paletteBoxes[i].setPosition(i * m_color_block_width, 0);
    }
}

void draw_palette(sf::RectangleShape *paletteBoxes, sf::RenderWindow &window)
{
    for (size_t i = 0; i < m_numOfColors; i++)
    {
        window.draw(paletteBoxes[i]);
    }
}

int main()
{
    sf::Color paintColor = sf::Color::Magenta;
    sf::Color palette[m_numOfColors];
    sf::RectangleShape paletteBoxes[m_numOfColors];
    std::vector<sf::CircleShape> color_boxes;
    float a = m_scale / std::sqrt(2);
    sf::RenderWindow paletteWindow(sf::VideoMode(m_numOfColors * m_color_block_width, m_color_block_height), "PaletteWindow");
    sf::RenderWindow window(sf::VideoMode(600, 600), "Scribble");

    fill_palette(paletteBoxes, palette, paletteWindow);
    sf::CircleShape box(a, 4);
    box.setFillColor(sf::Color::Cyan);
    box.setPosition(50, 50);
    box.setRotation(45);
    box.setOrigin(0, a);
    paletteWindow.setPosition(sf::Vector2i(window.getPosition().x + window.getSize().x, window.getPosition().y));

    while (window.isOpen() && paletteWindow.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.clear();
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                int x = event.mouseButton.x;
                int y = event.mouseButton.y;
                if (y < window.getSize().y - m_scale)
                {
                    x -= x % m_scale;
                    y -= y % m_scale;
                    color_boxes.push_back(sf::CircleShape(box));
                    color_boxes.back().setPosition(x, y);
                    color_boxes.back().setFillColor(paintColor);
                }
            }
        }

        while (paletteWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                paletteWindow.clear();
                paletteWindow.close();
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                int x = event.mouseButton.x;
                paintColor = palette[(int)(x / m_color_block_width)];
            }
        }

        window.clear();
        paletteWindow.clear();
        draw_grid(window);
        draw_boxes(window, color_boxes);
        draw_palette(paletteBoxes, paletteWindow);
        window.draw(box);

        paletteWindow.display();
        window.display();
    }
    return 0;
}
