#include "scribble.h"
#include "box.h"

Scribble::Scribble()
{
    m_window.create(sf::VideoMode(COLOR_BLOCK_SIDE * PAINTED_BLOCKS_PER_ROW,
                                  (COLOR_BLOCK_SIDE * PAINTED_BLOCKS_PER_ROW)),
                    "Scribble");
    m_paletteWindow.create(sf::VideoMode(COLOR_BLOCK_SIDE * PALETTE_COLORS_PER_ROW,
                                         COLOR_BLOCK_SIDE * (int)(MAX_COLOR_OPTIONS / PALETTE_COLORS_PER_ROW)),
                           "PaletteWindow");
    m_paletteWindow.setPosition(sf::Vector2i(m_paletteWindow.getPosition().x + m_window.getSize().x,
                                             m_paletteWindow.getPosition().y));
    m_selectedColor = sf::Color::White;
    m_removeDuplicateBoxes = false;
    m_mousePressed = false;
    initializeCURL();
    fillPalette();
    startPainting();
}

Scribble::~Scribble()
{
    m_window.~RenderWindow();
    m_paletteWindow.~RenderWindow();
    // delete[] m_paintColors;
    // delete[] m_paletteColors;
    cleanCURL();
}

void Scribble::cleanCURL()
{
    curl_easy_cleanup(m_curl);
    curl_global_cleanup();
}

size_t Scribble::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void Scribble::initializeCURL()
{
    m_curl = curl_easy_init();
    if (m_curl)
    {
        curl_easy_setopt(m_curl, CURLOPT_URL, "http://colormind.io/api/");
        curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, "{\"model\":\"default\"}");
        curl_easy_setopt(m_curl, CURLOPT_USERAGENT, "curl/7.54.0");
        curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, Scribble::WriteMemoryCallback);
    }
    else
    {
        std::cerr << "Problem initializing CURL" << std::endl;
        exit(1);
    }
}

void Scribble::fillPalette()
{
    struct MemoryStruct chunk;
    rapidjson::Document doc;

    chunk.memory = (char *)malloc(1);
    chunk.size = 0;

    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, (void *)&chunk);
    m_res = curl_easy_perform(m_curl);

    if (m_res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(m_res));
        exit(1);
    }
    else
        std::cout << chunk.memory;

    doc.Parse(chunk.memory);
    const rapidjson::Value &result = doc["result"];
    for (rapidjson::SizeType itr = 0; itr < result.Capacity(); ++itr)
    {
        m_paletteColors[itr] = Box(sf::Vector2f((itr % PALETTE_COLORS_PER_ROW) * COLOR_BLOCK_SIDE, (itr / PALETTE_COLORS_PER_ROW) * COLOR_BLOCK_SIDE),
                                   sf::Color(result[itr][0].GetInt(), result[itr][1].GetInt(), result[itr][2].GetInt()));
    }
}

void Scribble::draw_grid(sf::RenderWindow &window)
{
    sf::Vector2u dim(window.getSize());

    int horizontal_lines = dim.x / SCALE - 1;
    int vertical_lines = dim.y / SCALE - 1;
    sf::VertexArray vertical(sf::Lines, vertical_lines * 2);
    sf::VertexArray horizontal(sf::Lines, horizontal_lines * 2);

    for (size_t i = 0; i < horizontal_lines * 2; i += 2)
    {
        horizontal[i].position = sf::Vector2f(0, SCALE * (i / 2 + 1));
        horizontal[i + 1].position = sf::Vector2f(dim.x, SCALE * (i / 2 + 1));
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
        vertical[i].position = sf::Vector2f(SCALE * (i / 2 + 1), 0);
        vertical[i + 1].position = sf::Vector2f(SCALE * (i / 2 + 1), dim.y);
        vertical[i].color = sf::Color(35, 35, 35);
        vertical[i + 1].color = sf::Color(35, 35, 35);
    }

    window.draw(horizontal);
    window.draw(vertical);
}

void Scribble::draw_palette()
{
    for (size_t i = 0; i < MAX_COLOR_OPTIONS; i++)
    {
        m_paletteWindow.draw(m_paletteColors[i].getShape());
    }
}

void Scribble::position_paints()
{
    for (size_t i = 0; i < m_paintColors.size(); i++)
    {
        m_paintColors[i].setPosition(sf::Vector2f(i * COLOR_BLOCK_SIDE, m_window.getSize().y - COLOR_BLOCK_SIDE));
    }
}

void Scribble::draw_paints()
{
    for (size_t i = 0; i < m_paintColors.size(); i++)
    {
        m_window.draw(m_paintColors[i].getShape());
    }
}

void Scribble::draw_boxes(sf::RenderWindow &window, std::vector<Box> &color_boxes)
{
    for (std::vector<Box>::iterator i = color_boxes.begin(); i != color_boxes.end(); ++i)
    {
        m_window.draw((*i).getShape());
    }
}

bool Scribble::containsThisBox(Box &obj)
{
    for (size_t i = 0; i < m_coloredBoxes.size(); i++)
    {
        if (m_coloredBoxes[i] == obj)
            return true;
    }
    return false;
}

void Scribble::startPainting()
{
    while (m_window.isOpen() && m_paletteWindow.isOpen())
    {
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                m_window.clear();
                m_window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                m_mousePressed = true;
                int x = event.mouseButton.x;
                int y = event.mouseButton.y;
                x -= x % SCALE;
                y -= y % SCALE;

                if (y < m_window.getSize().y - COLOR_BLOCK_SIDE)
                {
                    Box add(sf::Vector2f(x, y), m_selectedColor);
                    if(!containsThisBox(add))
                        m_coloredBoxes.push_back(add);
                    m_removeDuplicateBoxes = true;
                }
                else
                {
                    int index = x / COLOR_BLOCK_SIDE;
                    if (index < m_paintColors.size())
                    {
                        m_selectedColor = m_paintColors[index].getColor();
                    }
                }
            }
            else if (event.type == sf::Event::MouseMoved && m_mousePressed)
            {
                int x = event.mouseMove.x;
                int y = event.mouseMove.y;
                x -= x % COLOR_BLOCK_SIDE;
                y -= y % COLOR_BLOCK_SIDE;

                if (y < (m_window.getSize().y - COLOR_BLOCK_SIDE))
                {
                    Box add(sf::Vector2f(x, y), m_selectedColor);
                    if (!(m_coloredBoxes.back() == add)) {
                        m_coloredBoxes.push_back(add);
                    }
                    m_removeDuplicateBoxes = true;
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                m_mousePressed = false;
            }
            else if (m_removeDuplicateBoxes)
            {
                m_removeDuplicateBoxes = false;
                for (std::vector<Box>::iterator i = m_coloredBoxes.begin(); i != m_coloredBoxes.end(); ++i)
                {
                    for (std::vector<Box>::iterator j = i + 1; j != m_coloredBoxes.end(); ++j)
                    {
                        if ((*i) ^= (*j))
                        {
                            m_coloredBoxes.erase(i);
                            break;
                        }
                    }
                }
            }
        }

        while (m_paletteWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                m_paletteWindow.clear();
                m_paletteWindow.close();
            }

            if (event.type == sf::Event::MouseButtonReleased)
            {
                int x = event.mouseButton.x;
                int y = event.mouseButton.y;

                if (m_paintColors.size() >= MAX_CURRENT_PAINT_COLORS)
                {
                    m_paintColors.pop_back();
                }
                m_paintColors.push_front(m_paletteColors[(y / COLOR_BLOCK_SIDE) * PALETTE_COLORS_PER_ROW +
                                                         (x / COLOR_BLOCK_SIDE)]);
                position_paints();
            }
        }

        m_window.clear();
        m_paletteWindow.clear();
        draw_grid(m_window);
        draw_palette();
        draw_paints();
        draw_boxes(m_window, m_coloredBoxes);

        m_paletteWindow.display();
        m_window.display();
    }
}
