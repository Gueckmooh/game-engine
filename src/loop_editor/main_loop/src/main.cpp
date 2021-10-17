#include <cmath>
// #include <ctime>
#include <chrono>
#include <file_watcher/file_watcher.hpp>
#include <initializer_list>
#include <iostream>
#include <main_loop/game_data.hpp>
#include <memory>
#include <thread>
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

namespace {

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

    uint32_t getTile(Vector<float> pos, Vector<float> displaySize) {
        float tileW = displaySize.X / ((float)width);
        float tileH = displaySize.Y / ((float)height);

        float tileX = pos.X / tileW;
        float tileY = pos.Y / tileH;

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
TileMap tileMap({
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
});

bool isOk(Vector<float> vec, window::BitMap& bm) {
    return (tileMap.getTile(vec.X, vec.Y, bm.mode().width(), bm.mode().height()) == 0);
}

#if 0
extern "C" void processInputs(window::input::InputManager& inputManager,
                              game_data::GameData& gd, window::BitMap& bm) {

    int targetX = gd.player.X, targetY = gd.player.Y;

    if (inputManager.isActive("up")
        && (isOk(gd.player.leftColX(), gd.player.Y - 10, bm)
            && isOk(gd.player.rightColX(), gd.player.Y - 10, bm))) {
        targetY -= 2;
    } else if (inputManager.isActive("down")
               && (isOk(gd.player.leftColX(), gd.player.Y + 2, bm)
                   && isOk(gd.player.rightColX(), gd.player.Y + 2, bm))) {
        targetY += 2;
    }

    if (inputManager.isActive("left")
        && (isOk(gd.player.leftColX() - 2, gd.player.Y, bm))) {
        targetX -= 2;
    } else if (inputManager.isActive("right")
               && (isOk(gd.player.rightColX() + 2, gd.player.Y, bm))) {
        targetX += 2;
    }

    gd.player.X = targetX;
    gd.player.Y = targetY;
}
#else
bool checkCollision(Vector<float> vec, TileMap tMap, Vector<float> mapSize) {
    float tileX = vec.X / mapSize.X;
    float tileY = vec.Y / mapSize.Y;

    return tMap.tiles[tileY][tileX] == 0;
}

bool checkCollision(Player::ColisionArea col, TileMap tMap, Vector<float> mapSize) {
    float tileW = mapSize.X / ((float)tMap.width);
    float tileH = mapSize.Y / ((float)tMap.height);

    auto tot = Vector<float>(tileW, tileH);
    return checkCollision(col.bottomLeft(), tMap, tot)
           && checkCollision(col.bottomRight(), tMap, tot)
           && checkCollision(col.topLeft(), tMap, tot)
           && checkCollision(col.topRight(), tMap, tot);
}

extern "C" void processInputs(window::input::InputManager& inputManager,
                              game_data::GameData& gd, window::BitMap& bm) {
    Vector<float> dT{ 0.0f, 0.0f };

    if (inputManager.isActive("up")) {
        dT -= { 0.0f, 2.0f };
    } else if (inputManager.isActive("down")) {
        dT += { 0.0f, 2.0f };
    }

    if (inputManager.isActive("left")) {
        dT -= { 2.0f, 0.0f };
    } else if (inputManager.isActive("right")) {
        dT += { 2.0f, 0.0f };
    }

    auto col = gd.player.colision();
    col      = col + dT;

    bool colOk = checkCollision(col, tileMap,
                                { (float)bm.mode().width(), (float)bm.mode().height() });
    if (colOk) gd.player.pos += dT;
}

#endif

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
    int MinX = gd.player.pos.X - (gd.player.width / 2);
    int MinY = gd.player.pos.Y - gd.player.height;

    drawRectangle(bitmap, MinX, MinY, MinX + gd.player.width, MinY + gd.player.height,
                  1.0f, 0.0f, 1.0f);
    // drawRectangle(bitmap, gd.player.X, gd.player.Y, gd.player.X + 2, gd.player.Y + 2,
    //               0.0f, 0.0f, 0.0f);

    drawRectangle(bitmap, gd.player.leftColX(), gd.player.pos.Y - 2,
                  gd.player.rightColX(), gd.player.pos.Y, 0.0f, 0.0f, 0.0f);
}

extern "C" void gameUpdateAndRender(window::BitMap& bitmap, game_data::GameData& gd,
                                    window::input::InputManager& im) {
    processInputs(im, gd, bitmap);

    drawRectangle(bitmap, 0.0f, 0.0f, (float)bitmap.mode().width(),
                  (float)bitmap.mode().height(), 1.0f, 0.0f, 0.1f);

    // drawRectangle(bitmap, 0.0f, 0.0f, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f);

    renderTileMap(tileMap, bitmap);

    renderPlayer(bitmap, gd);
    // renderBitmap(bitmap, gd);

    bitmap.flush();
}

void gameUpdateAndRenderR(window::BitMap& bitmap, game_data::GameData& gd,
                          window::input::InputManager& im) {
    gameUpdateAndRender(bitmap, gd, im);
}
