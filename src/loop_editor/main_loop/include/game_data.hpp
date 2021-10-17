#pragma once

#include <cstdint>

namespace game_data {

struct Player {
    int X;
    int Y;
    int width  = 50;
    int height = 50;
    Player()   = default;
    Player(int x, int y) : X(x), Y(y) {}
    Player(Player&) = default;

    uint32_t leftColX() { return X - (width / 2); }

    uint32_t rightColX() { return X + (width / 2); }
};

struct GameData {
    int backgroundX;
    int backgroundY;
    Player player;
    float jump = 0.0f;
};

}   // namespace game_data
