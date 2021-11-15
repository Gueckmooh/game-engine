#include <chrono>
#include <cmath>
#include <draw/basics/colors.hpp>
#include <draw/basics/rectangle.hpp>
#include <file_watcher/file_watcher.hpp>
#include <initializer_list>
#include <iostream>
#include <main_loop/game_data.hpp>
#include <main_loop/tilemap.hpp>
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

using namespace draw::basics;

extern "C" void processInputs(window::input::InputManager& inputManager,
                              game_data::GameData& gd, window::BitMap& /* bm */) {
    glm::tvec2<float> dT{ 0.0f, 0.0f };

    if (inputManager.isActive("up")) {
        dT = dT - glm::vec2{ 0.0f, 0.1f };
    } else if (inputManager.isActive("down")) {
        dT = dT + glm::vec2{ 0.0f, 0.1f };
    }

    if (inputManager.isActive("left")) {
        dT = dT - glm::vec2{ 0.1f, 0.0f };
    } else if (inputManager.isActive("right")) {
        dT = dT + glm::vec2{ 0.1f, 0.0f };
    }

    gd.player.pos = gd.player.pos + dT;
}

// void drawRectangle(window::BitMap& bm, float fMinX, float fMinY, float fMaxX, float
// fMaxY,
//                    float R, float G, float B) {
//     int32_t minX = lround(fMinX);
//     int32_t minY = lround(fMinY);
//     int32_t maxX = lround(fMaxX);
//     int32_t maxY = lround(fMaxY);

//     if (minX < 0) { minX = 0; }
//     if (minY < 0) { minY = 0; }
//     if (maxX > (int32_t)bm.mode().width()) { maxX = bm.mode().width(); }
//     if (maxY > (int32_t)bm.mode().height()) { maxY = bm.mode().height(); }

//     uint32_t color = ((lround(R * 255.0f) << 16) | (lround(G * 255.0f) << 8)
//                       | (lround(B * 255.0f) << 0));

//     uint8_t* Row = ((uint8_t*)bm.data() + minX * bm.mode().bytesPerPixel()
//                     + minY * bm.mode().pitch());

//     for (ssize_t Y = minY; Y < maxY; ++Y) {
//         uint32_t* Pixel = (uint32_t*)Row;
//         for (ssize_t X = minX; X < maxX; ++X) { *Pixel++ = color; }

//         Row += bm.mode().pitch();
//     }
// }

// void drawRectangle(window::BitMap& bm, Rectangle<float> rect, float R, float G, float
// B) {
//     drawRectangle(bm, rect.topLeft().x, rect.topLeft().y, rect.bottomRight().x,
//                   rect.bottomRight().y, R, G, B);
// }

// void renderTileMap(TileMap& tm, window::BitMap& bm) {
//     for (int Row = 0; Row < 9; ++Row) {
//         for (int Column = 0; Column < 17; ++Column) {
//             uint32_t TileID = tm.tiles[Row][Column];
//             float Gray      = 0.5f;
//             if (TileID == 1) { Gray = 1.0f; }

//             float MinX =
//                 0.0f + ((float)Column) * (((float)bm.mode().width()) /
//                 ((float)tm.width));
//             float MinY =
//                 0.0f + ((float)Row) * (((float)bm.mode().height()) /
//                 ((float)tm.height));
//             float MaxX = MinX + (((float)bm.mode().width()) / ((float)tm.width));
//             float MaxY = MinY + (((float)bm.mode().height()) / ((float)tm.height));

//             drawRectangle(bm, MinX, MinY, MaxX, MaxY, Gray, Gray, Gray);
//         }
//     }
// }

struct Display {
    window::BitMap& bitmap;
    float widthUnit;

    glm::vec2 cameraOrigin{ 0.0f, 0.0f };

    Display(window::BitMap& bitmap, float widthUnit)
        : bitmap(bitmap), widthUnit(widthUnit) {}

    void drawRectangle(float fMinX, float fMinY, float fMaxX, float fMaxY,
                       const Color& _color) {
        ::draw::basics::drawRectangle(bitmap, fMinX, fMinY, fMaxX, fMaxY, _color);
    }

    void drawRectangle(Rectangle<float> rect, const Color& _color) {
        drawRectangle(rect.topLeft().x, rect.topLeft().y, rect.bottomRight().x,
                      rect.bottomRight().y, _color);
    }

    float pixelPerUnit() { return ((float)bitmap.mode().width()) / widthUnit; }

    float unitToPixel(float unit) { return unit * pixelPerUnit(); }

    void drawBackground(const Color& _color) {
        drawRectangle(
            Rectangle<float>{ { 0.0f, 0.0f },
                              { bitmap.mode().width(), bitmap.mode().height() } },
            _color);
    }

    void drawRectangleInMeters(Rectangle<float> rect, const Color& _color) {
        Rectangle<float> newRectangle = {
            { unitToPixel(rect.TopLeft.x), unitToPixel(rect.TopLeft.y) },
            { unitToPixel(rect.BottomRight.x), unitToPixel(rect.BottomRight.y) },
        };
        drawRectangle(newRectangle, _color);
    }

    glm::vec2 getOriginVector() {
        return glm::tvec2<float>{ unitToPixel(cameraOrigin.x),
                                  unitToPixel(cameraOrigin.y) };
    }

    void renderPlayer(const Player& player) {
        // @todo move the difference to drawRectangleInMeters
        drawRectangleInMeters(player.getRect() - cameraOrigin, Color::Blue());
        drawRectangleInMeters(player.getColision() - cameraOrigin, Color::Black());
    }

    void renderTileMap(const TileMap& tm) {
        auto colFloor = Color::White();
        auto colWall  = Color::Gray();
        auto tileSize = unitToPixel(tm.tileWidth);
        auto xshift   = unitToPixel(tm.pos.x);
        auto yshift   = unitToPixel(tm.pos.y);
        auto origin   = getOriginVector();
        for (size_t Row = 0; Row < tm.height; ++Row) {
            for (size_t Column = 0; Column < tm.width; ++Column) {
                uint32_t TileID = tm.tiles[Row][Column];
                Rectangle<float> rect{ xshift + (float)Column * tileSize,
                                       yshift + (float)Row * tileSize,
                                       xshift + ((float)Column + 1.0f) * tileSize,
                                       yshift + ((float)Row + 1.0f) * tileSize };
                rect = rect - origin;
                drawRectangle(rect, TileID == 0 ? colFloor : colWall);
            }
        }
    }

    void displayGrid() {
        float pixPerUnit = pixelPerUnit();

        // print lines
        for (float Row = 0; Row < bitmap.mode().height(); Row += pixPerUnit) {
            Rectangle<float> rect{ 0.0f, (float)Row, (float)bitmap.mode().width(),
                                   (float)Row + 1.0f };
            drawRectangle(rect, Color::Black());
        }
        for (float Column = 0; Column < bitmap.mode().width(); Column += pixPerUnit) {
            Rectangle<float> rect{ (float)Column, 0.0f, (float)Column + 1.0f,
                                   (float)bitmap.mode().height() };
            drawRectangle(rect, Color::Black());
        }
    }

    void renderWorld(WorldMap& world) {
        auto& set = world.getTilesToRender(Rectangle<float>{
            cameraOrigin, cameraOrigin + glm::vec2{ widthUnit, (widthUnit / 16) * 9 } });
        for (auto tm : set) { renderTileMap(*tm); }
    }
};

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
    inputManager.addMapping("shiftLeft",
                            window::input::Input(window::input::key::KeyboardKey::Q));
    inputManager.addMapping("shiftRight",
                            window::input::Input(window::input::key::KeyboardKey::D));
    inputManager.addMapping("shiftDown",
                            window::input::Input(window::input::key::KeyboardKey::S));
    inputManager.addMapping("shiftUp",
                            window::input::Input(window::input::key::KeyboardKey::Z));
}

TileMap tileMap1(
    {
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
        { 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1 },
        { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
    },
    glm::vec2{ 0.0f, .0f });

TileMap tileMap2(
    {
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
        { 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1 },
        { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    },
    glm::vec2{ 24.0f, .0f });

TileMap tileMap3(
    {
        { 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
        { 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1 },
        { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    },
    glm::vec2{ 0.0f, 13.5f });

// struct Camera {
//     glm::vec2 origin;
//     float width;
//     Display& display;

//     Color bgColor;

//     Camera(glm::vec2 origin, float width, Display& display)
//         : origin(origin), width(width), display(display) {}
//     void setBGColor(Color color) { bgColor = color; }

//     void shot() { d.drawBackground(bgColor); }
// };

WorldMap World;

extern "C" void initializeGame() {
    World.tileMapSize = glm::vec2{ tileMap1.width * tileMap1.tileWidth,
                                   tileMap1.height * tileMap1.tileWidth };
    World.mapTable.push_back(std::vector<TileMap*>{ &tileMap1, &tileMap2 });
    World.mapTable.push_back(std::vector<TileMap*>{ &tileMap3 });
}

extern "C" void gameUpdateAndRender(window::BitMap& bitmap, game_data::GameData& gd,
                                    window::input::InputManager& im) {
    // auto& tileMap = *tiles[gd.mapPos.y][gd.mapPos.x];
    processInputs(im, gd, bitmap);
    Display d{ bitmap, 24.0 };   // height = 13.5

    d.cameraOrigin = gd.player.pos - glm::vec2{ 12.0f, 6.75f };

    d.drawBackground(Color::Magenta());

    d.renderWorld(World);

    // d.renderTileMap(tileMap1);
    // d.renderTileMap(tileMap2);

    d.renderPlayer(gd.player);

    d.displayGrid();

    // drawRectangle(bitmap, 0.0f, 0.0f, (float)bitmap.mode().width(),
    //               (float)bitmap.mode().height(), 1.0f, 0.0f, 0.1f);

    // renderTileMap(tileMap, bitmap);

    // renderPlayer(bitmap, gd);

    // drawRectangle(bitmap, testCol, 1.0f, 0.0f, 0.0f);

    // bitmap.flush();
}

void gameUpdateAndRenderR(window::BitMap& bitmap, game_data::GameData& gd,
                          window::input::InputManager& im) {
    gameUpdateAndRender(bitmap, gd, im);
}
