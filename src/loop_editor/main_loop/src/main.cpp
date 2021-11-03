#include <chrono>
#include <cmath>
#include <file_watcher/file_watcher.hpp>
#include <initializer_list>
#include <iostream>
#include <main_loop/game_data.hpp>
#include <memory>
#include <thread>
#include <tuple>
#include <vector>

#include <audio/audio_engine.hpp>
#include <audio/examples/examples.hpp>
#include <audio/sound_data.hpp>
#include <logging/logger.hpp>
#include <window/bitmap.hpp>
#include <window/examples/examples.hpp>
#include <window/input.hpp>
#include <window/video_mode.hpp>
#include <window/window.hpp>

using namespace std::chrono_literals;

using namespace game_data;

using namespace glm_compat;

namespace {

template<typename T>
struct NormalizedVector {
    glm::tvec2<T> vect;
    glm::tvec2<int> pos;
};

template<typename T>
struct NormalizedRectangle {
    Rectangle<T> rect;
    glm::tvec2<int> tl;
    glm::tvec2<int> br;
};

class TileMap {
  public:
    std::vector<std::vector<uint32_t>> tiles;
    size_t width;
    size_t height;
    TileMap(std::initializer_list<std::initializer_list<uint32_t>> init) {
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
        this->width  = width;
        this->height = height;
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

}   // namespace

extern "C" void initInputManager(window::input::InputManager& inputManager) {
    inputManager.addMapping("up",
                            window::input::Input(window::input::key::KeyboardKey::Up));
    inputManager.addMapping("down",
                            window::input::Input(window::input::key::KeyboardKey::Down));
    inputManager.addMapping("left",
                            window::input::Input(window::input::key::KeyboardKey::Left));
    inputManager.addMapping("right",
                            window::input::Input(window::input::key::KeyboardKey::Right));
    inputManager.addMapping("jump",
                            window::input::Input(window::input::key::KeyboardKey::Space));
}

// @todo fix this shit
TileMap tileMap1({
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
});

TileMap tileMap2({
    { 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
    { 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
});

TileMap tileMap3({
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
});

TileMap tileMap4({
    { 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
});

std::vector<std::vector<TileMap*>> tiles{ { &tileMap1, &tileMap3 },
                                          { &tileMap2, &tileMap4 } };

// bool isOk(Vector<float> vec, window::BitMap& bm, TileMap& tileMap) {
//     return (tileMap.getTile(vec.X, vec.Y, bm.mode().width(), bm.mode().height()) == 0);
// }

bool checkCollision(glm::tvec2<float> vec, TileMap tMap, glm::tvec2<float> mapSize) {
    float tileX = vec.x / mapSize.x;
    float tileY = vec.y / mapSize.y;

    if (tileX >= 0 && tileX < tMap.width && tileY < tMap.height && tileY >= 0)
        return tMap.tiles[tileY][tileX] == 0;
    else
        return true;   // @note return true to permit tilemap change
}

bool checkCollision(Player::ColisionArea col, TileMap tMap, glm::tvec2<float> mapSize) {
    float tileW = mapSize.x / ((float)tMap.width);
    float tileH = mapSize.y / ((float)tMap.height);

    auto tot = glm::tvec2<float>(tileW, tileH);
    return checkCollision(col.bottomLeft(), tMap, tot)
           && checkCollision(col.bottomRight(), tMap, tot)
           && checkCollision(col.topLeft(), tMap, tot)
           && checkCollision(col.topRight(), tMap, tot);
}

bool checkCollision(NormalizedRectangle<float> col,
                    std::vector<std::vector<TileMap*>> maps, glm::tvec2<float> mapSize) {
    float tileW = mapSize.x / ((float)maps[0][0]->width);
    float tileH = mapSize.y / ((float)maps[0][0]->height);

    auto& mapTopLeft     = *maps[col.tl.y][col.tl.x];
    auto& mapBottomRight = *maps[col.br.y][col.br.x];
    auto& mapTopRight    = *maps[col.tl.y][col.br.x];
    auto& mapBottomLeft  = *maps[col.br.y][col.tl.x];

    auto tot = glm::tvec2<float>(tileW, tileH);
    return checkCollision(col.rect.bottomLeft(), mapBottomLeft, tot)
           && checkCollision(col.rect.bottomRight(), mapBottomRight, tot)
           && checkCollision(col.rect.topLeft(), mapTopLeft, tot)
           && checkCollision(col.rect.topRight(), mapTopRight, tot);
}

bool checkCollision(Rectangle<float> rect, glm::tvec2<float> vec) {
    return (vec > rect.TopLeft) && (vec < rect.BottomRight);
}

bool checkCollision(Rectangle<float> rect1, Rectangle<float> rect2) {
    return checkCollision(rect1, rect2.topLeft())
           || checkCollision(rect1, rect2.bottomRight())
           || checkCollision(rect1, rect2.bottomLeft())
           || checkCollision(rect1, rect2.topRight());
}

Rectangle<float> testCol{ { 150.0f, 150.0f }, { 200.0f, 200.0f } };

NormalizedVector<float> normalizePos(glm::tvec2<float> pos, glm::tvec2<float> size) {
    int Xindex = (int)pos.x / size.x;
    int Yindex = (int)pos.y / size.y;

    glm::tvec2<int> posVec = { Xindex, Yindex };
    auto normalizedPos     = pos - glm::tvec2<float>{ size.x * Xindex, size.y * Yindex };
    return { normalizedPos, posVec };
}

NormalizedRectangle<float> normalizePos(Rectangle<float> pos, glm::tvec2<float> size) {
    auto tl = normalizePos(pos.TopLeft, size);
    auto br = normalizePos(pos.BottomRight, size);
    return { { tl.vect, br.vect }, tl.pos, br.pos };
}

glm::tvec2<float> unNormalizePos(glm::tvec2<int>& posVec, glm::tvec2<float>& pos,
                                 glm::tvec2<float>& size) {
    float X = posVec.x * size.x + pos.x;
    float Y = posVec.y * size.y + pos.y;

    return { X, Y };
}

glm::tvec2<float> bitmapSizeVec(window::BitMap& bm) {
    return glm::tvec2<float>{
        (float)bm.mode().width(),
        (float)bm.mode().height(),
    };
}

extern "C" void processInputs(window::input::InputManager& inputManager,
                              game_data::GameData& gd, window::BitMap& bm) {
    glm::tvec2<float> dT{ 0.0f, 0.0f };

    if (inputManager.isActive("up")) {
        dT = dT - glm::vec2{ 0.0f, 2.0f };
    } else if (inputManager.isActive("down")) {
        dT = dT + glm::vec2{ 0.0f, 2.0f };
    }

    if (inputManager.isActive("left")) {
        dT = dT - glm::vec2{ 2.0f, 0.0f };
    } else if (inputManager.isActive("right")) {
        dT = dT + glm::vec2{ 2.0f, 0.0f };
    }

    auto col = gd.player.colision();
    // auto ncol = normalizePos(col, bitmapSizeVec(bm));

    auto colX = col + xproj(dT);
    auto colY = col + yproj(dT);

    auto ncolX = normalizePos(colX, bitmapSizeVec(bm));
    auto ncolY = normalizePos(colY, bitmapSizeVec(bm));

    bool colXOk = checkCollision(ncolX, tiles,
                                 { (float)bm.mode().width(), (float)bm.mode().height() });
    bool colYOk = checkCollision(ncolY, tiles,
                                 { (float)bm.mode().width(), (float)bm.mode().height() });
    if (colXOk) gd.player.pos += xproj(dT);
    if (colYOk) gd.player.pos += yproj(dT);

    auto normalized = normalizePos(gd.player.pos, bitmapSizeVec(bm));
    std::cout << normalized.pos << ", " << normalized.vect << std::endl;

    gd.mapPos = normalized.pos;
}

void drawRectangle(window::BitMap& bm, float fMinX, float fMinY, float fMaxX, float fMaxY,
                   float R, float G, float B) {
    int32_t minX = lround(fMinX);
    int32_t minY = lround(fMinY);
    int32_t maxX = lround(fMaxX);
    int32_t maxY = lround(fMaxY);

    if (minX < 0) { minX = 0; }
    if (minY < 0) { minY = 0; }
    if (maxX > (int32_t)bm.mode().width()) { maxX = bm.mode().width(); }
    if (maxY > (int32_t)bm.mode().height()) { maxY = bm.mode().height(); }

    uint32_t color = ((lround(R * 255.0f) << 16) | (lround(G * 255.0f) << 8)
                      | (lround(B * 255.0f) << 0));

    uint8_t* Row = ((uint8_t*)bm.data() + minX * bm.mode().bytesPerPixel()
                    + minY * bm.mode().pitch());

    for (ssize_t Y = minY; Y < maxY; ++Y) {
        uint32_t* Pixel = (uint32_t*)Row;
        for (ssize_t X = minX; X < maxX; ++X) { *Pixel++ = color; }

        Row += bm.mode().pitch();
    }
}

void drawRectangle(window::BitMap& bm, Rectangle<float> rect, float R, float G, float B) {
    drawRectangle(bm, rect.topLeft().x, rect.topLeft().y, rect.bottomRight().x,
                  rect.bottomRight().y, R, G, B);
}

void renderTileMap(TileMap& tm, window::BitMap& bm) {
    for (int Row = 0; Row < 9; ++Row) {
        for (int Column = 0; Column < 17; ++Column) {
            uint32_t TileID = tm.tiles[Row][Column];
            float Gray      = 0.5f;
            if (TileID == 1) { Gray = 1.0f; }

            float MinX =
                0.0f + ((float)Column) * (((float)bm.mode().width()) / ((float)tm.width));
            float MinY =
                0.0f + ((float)Row) * (((float)bm.mode().height()) / ((float)tm.height));
            float MaxX = MinX + (((float)bm.mode().width()) / ((float)tm.width));
            float MaxY = MinY + (((float)bm.mode().height()) / ((float)tm.height));

            drawRectangle(bm, MinX, MinY, MaxX, MaxY, Gray, Gray, Gray);
        }
    }
}

void renderPlayer(window::BitMap& bitmap, game_data::GameData& gd) {
    auto& pos    = gd.player.pos;
    auto npos    = normalizePos(pos, bitmapSizeVec(bitmap));
    auto nposVec = npos.vect;
    int MinX     = nposVec.x - (gd.player.width / 2);
    int MinY     = nposVec.y - gd.player.height;

    drawRectangle(bitmap, MinX, MinY, MinX + gd.player.width, MinY + gd.player.height,
                  1.0f, 0.0f, 1.0f);

    auto normalizedCol = normalizePos(gd.player.colision(), bitmapSizeVec(bitmap));
    drawRectangle(bitmap, normalizedCol.rect, 0.0f, 0.0f, 0.0f);
}

extern "C" void gameUpdateAndRender(window::BitMap& bitmap, game_data::GameData& gd,
                                    window::input::InputManager& im) {
    auto& tileMap = *tiles[gd.mapPos.y][gd.mapPos.x];
    processInputs(im, gd, bitmap);

    drawRectangle(bitmap, 0.0f, 0.0f, (float)bitmap.mode().width(),
                  (float)bitmap.mode().height(), 1.0f, 0.0f, 0.1f);

    renderTileMap(tileMap, bitmap);

    renderPlayer(bitmap, gd);

    drawRectangle(bitmap, testCol, 1.0f, 0.0f, 0.0f);

    bitmap.flush();
}

void gameUpdateAndRenderR(window::BitMap& bitmap, game_data::GameData& gd,
                          window::input::InputManager& im) {
    gameUpdateAndRender(bitmap, gd, im);
}
