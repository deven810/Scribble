#ifndef SCRIBBLE_H
#define SCRIBBLE_H
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <deque>
#include <curl/curl.h>
#include "box.h"
#include "rapidjson/include/rapidjson/document.h"

#define MAX_COLOR_OPTIONS 5
#define MAX_CURRENT_PAINT_COLORS 8 
#define SCALE 50
#define COLOR_BLOCK_SIDE 50
#define PALETTE_COLORS_PER_ROW 5
#define PAINTED_BLOCKS_PER_ROW 16

class Scribble 
{
public:
    Scribble();
    ~Scribble();

protected:
    void draw_grid(sf::RenderWindow &window);
    void draw_boxes(sf::RenderWindow &window, std::vector<Box> &color_boxes);
    void draw_palette();
    void position_paints();
    void draw_paints();
    void startPainting();
    void fillPalette();
    void initializeCURL();
    void cleanCURL();
    bool containsThisBox(Box &obj);
    static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

    sf::RenderWindow m_window; 
    sf::RenderWindow m_paletteWindow; 
    std::vector<Box> m_coloredBoxes;
    std::deque<Box> m_paintColors; 
    Box m_paletteColors[MAX_COLOR_OPTIONS];
    sf::Color m_selectedColor;
    CURL *m_curl;
    CURLcode m_res;

    struct MemoryStruct
    {
        char *memory;
        size_t size;
    };

private:
    bool m_removeDuplicateBoxes; 
    bool m_mousePressed;
};

#endif
