#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <ostream>

namespace game_data {

template<typename T>
struct Vector {
    T X;
    T Y;

    Vector(T x, T y) : X(x), Y(y) {}
    Vector(Vector&) = default;
    Vector()        = default;

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

template<typename T>
struct Rectangle {
    Vector<T> TopLeft;
    Vector<T> BottomRight;

    Rectangle(Vector<T> tl, Vector<T> br) : TopLeft(tl), BottomRight(br) {}
    Rectangle(Rectangle&) = default;

    Vector<T> topLeft() { return TopLeft; }
    Vector<T> bottomRight() { return BottomRight; }
    Vector<T> topRight() { return Vector<T>(BottomRight.X, TopLeft.Y); }
    Vector<T> bottomLeft() { return Vector<T>(TopLeft.X, BottomRight.Y); }

    Rectangle operator+(const Vector<T>& other) {
        return Rectangle(TopLeft + other, BottomRight + other);
    }

    friend std::ostream& operator<<(std::ostream& os, Rectangle& rect) {
        os << "{" << rect.TopLeft << ", " << rect.BottomRight << "}";
        return os;
    }
};

struct Player {
    using ColisionArea = Rectangle<float>;

    Vector<float> pos;
    size_t width  = 50;
    size_t height = 50;
    Player()      = default;
    Player(float x, float y) : pos(x, y) {}
    Player(Player&) = default;

    float leftColX() { return pos.X - (((float)width) / 2); }
    float rightColX() { return pos.X + (((float)width) / 2); }

    ColisionArea colision() {
        return ColisionArea({ pos.X - (((float)width) / 2), pos.Y - 10 },
                            { pos.X + (((float)width) / 2), pos.Y });
    }
};

struct GameData {
    int backgroundX;
    int backgroundY;
    Player player;
    float jump   = 0.0f;
    size_t mapId = 0;
};

}   // namespace game_data
