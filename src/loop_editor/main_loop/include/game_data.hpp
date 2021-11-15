#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <draw/basics/rectangle.hpp>
#include <ostream>

#include <glm/gtx/string_cast.hpp>
#include <glm/vec2.hpp>

// glm compat
namespace glm_compat {
bool operator<(glm::vec2& v1, glm::vec2 v2) { return v1.x < v2.x && v1.y < v2.y; }
bool operator>(glm::vec2& v1, glm::vec2 v2) { return v1.x > v2.x && v1.y > v2.y; }

bool operator<=(glm::vec2& v1, glm::vec2 v2) { return !(v1 > v2); }
bool operator>=(glm::vec2& v1, glm::vec2 v2) { return !(v1 < v2); }

// @todo add proper operator+= and operator -= to glm ?

glm::vec2 xproj(glm::vec2& vec) { return glm::vec2{ vec.x, 0.0f }; }
glm::vec2 yproj(glm::vec2& vec) { return glm::vec2{ 0.0f, vec.y }; }

std::ostream& operator<<(std::ostream& os, glm::vec2& vec) {
    os << glm::to_string(vec);
    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, glm::tvec2<T>& vec) {
    os << glm::to_string(vec);
    return os;
}

// glm::vec2 operator +=(glm::vec2& v1, glm::vec2 v2) { return (v1 + v2); }

}   // namespace glm_compat

namespace game_data {

template<typename T>
struct Vector {
    T X;
    T Y;

    Vector(T x, T y) : X(x), Y(y) {}
    Vector(Vector&)  = default;
    Vector(Vector&&) = default;
    Vector()         = default;

    constexpr Vector& operator=(const Vector&) = default;

    Vector(std::initializer_list<T> ilist) {
        assert(ilist.size() == 2 && "A 2D vector should have 2 composant");
        for (int i = 0; auto v : ilist) {
            if (i == 0) X = v;
            if (i == 1) Y = v;
            i++;
        }
    }

    Vector Xproj() { return Vector(X, 0); }
    Vector Yproj() { return Vector(0, Y); }

    Vector operator-() const { return Vector(-X, -Y); }

    Vector operator+(const Vector& other) { return Vector(X + other.X, Y + other.Y); }
    Vector operator-(const Vector& other) { return (*this) + (-other); }
    Vector operator+=(const Vector& other) {
        X += other.X;
        Y += other.Y;
        return *this;
    }
    Vector operator-=(const Vector& other) { return (*this) += (-other); }

    Vector operator+(const Vector&& other) { return Vector(X + other.X, Y + other.Y); }
    Vector operator-(const Vector&& other) { return (*this) + (-other); }
    Vector operator+=(const Vector&& other) {
        X += other.X;
        Y += other.Y;
        return *this;
    }
    Vector operator-=(const Vector&& other) { return (*this) += (-other); }

    bool operator>(const Vector& other) { return X > other.X && Y > other.Y; }
    bool operator<(const Vector& other) { return X < other.X && Y < other.Y; }
    bool operator>=(const Vector& other) { return X >= other.X && Y >= other.Y; }
    bool operator<=(const Vector& other) { return X <= other.X && Y <= other.Y; }

    friend std::ostream& operator<<(std::ostream& os, Vector<T>& v) {
        os << "[" << v.X << ", " << v.Y << "]";
        return os;
    }

    void print(std::ostream& os) { os << "[" << X << ", " << Y << "]"; }
};

struct Player {
    using ColisionArea = draw::basics::Rectangle<float>;

    glm::vec2 pos;

    float width;
    float height;
    Player() = default;
    Player(float x, float y, float width = 1.2f, float height = 1.2f)
        : pos(x, y), width(width), height(height) {}
    Player(Player&) = default;

    float leftColX() { return pos.x - (((float)width) / 2); }
    float rightColX() { return pos.x + (((float)width) / 2); }

    ColisionArea getColision() const {
        glm::vec2 vtl = pos - glm::vec2{ .5f * width, height / 4 };
        glm::vec2 vbr = pos + glm::vec2{ .5f * width, 0.0f };
        ColisionArea r{ vtl, vbr };
        return r;
    }

    draw::basics::Rectangle<float> getRect() const {
        glm::vec2 vtl = pos - glm::vec2{ .5f * width, height };
        glm::vec2 vbr = pos + glm::vec2{ .5f * width, 0.0f };
        draw::basics::Rectangle<float> r{ vtl, vbr };
        return r;
    }
};

struct GameData {
    Player player;
    glm::tvec2<int> mapPos{ 0, 0 };
};

}   // namespace game_data
