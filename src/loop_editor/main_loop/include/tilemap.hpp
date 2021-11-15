#include <cassert>
#include <cstdint>
#include <draw/basics/rectangle.hpp>
#include <set>
#include <vector>

#include <glm/gtx/string_cast.hpp>
#include <glm/vec2.hpp>

class TileMap {
  public:
    std::vector<std::vector<uint32_t>> tiles;
    size_t width;
    size_t height;
    float tileWidth;
    glm::vec2 pos;
    TileMap(std::initializer_list<std::initializer_list<uint32_t>> init,
            glm::vec2 pos = { 0.0f, 0.0f })
        : pos(pos) {
        size_t height = init.size();
        size_t width  = 0;
        for (auto line : init) {
            tiles.push_back(line);

            if (width == 0)
                width = line.size();
            else {
                assert((width == line.size())
                       && "The width of all the lines must be the same");
            }
        }
        this->width     = width;
        this->height    = height;
        this->tileWidth = 1.5f;
    }

    uint32_t getTile(float X, float Y, float W, float H) {
        float tileW = W / ((float)width);
        float tileH = H / ((float)height);

        float tileX = X / tileW;
        float tileY = Y / tileH;

        return tiles[(size_t)tileY][(size_t)tileX];
    }

    uint32_t getTile(glm::tvec2<float> pos, glm::tvec2<float> displaySize) {
        float tileW = displaySize.x / ((float)width);
        float tileH = displaySize.y / ((float)height);

        float tileX = pos.x / tileW;
        float tileY = pos.y / tileH;

        return tiles[(size_t)tileY][(size_t)tileX];
    }
};

struct WorldMap {
    glm::vec2 tileMapSize;
    glm::ivec2 origin{ 0, 0 };
    std::vector<std::vector<TileMap*>> mapTable;
    // std::vector<TileMap*>&
    glm::ivec2 getTileToDisplay(const glm::vec2& vec) {
        int x = floorf(vec.x / tileMapSize.x);
        int y = floorf(vec.y / tileMapSize.y);
        return glm::ivec2{ x, y } + origin;
    }
    TileMap* getTileIfValid(const glm::ivec2& vec) {
        if (vec.x >= 0 && vec.y >= 0 && mapTable.size() > (unsigned)vec.y) {
            auto& row = mapTable.at(vec.y);
            if (row.size() > (unsigned)vec.x) { return row.at(vec.x); }
        }
        return nullptr;
    }
    const std::set<TileMap*>
    getTilesToRender(const draw::basics::Rectangle<float>& rect) {
        static std::set<TileMap*> tileSet;
        tileSet.clear();
        std::cout << "Inserted: ";
        auto addTileMap = [&](const glm::vec2& vec) -> void {
            glm::ivec2 coord = getTileToDisplay(vec);
            TileMap* tm      = getTileIfValid(coord);
            if (tm) {
                tileSet.insert(tm);
                std::cout << glm::to_string(coord) << "  ";
            }
        };
        addTileMap(rect.TopLeft);
        addTileMap(rect.BottomRight);
        addTileMap(rect.bottomLeft());
        addTileMap(rect.topRight());

        std::cout << std::endl;
        return tileSet;
    }
};
