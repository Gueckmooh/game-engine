#include <draw/basics/rectangle.hpp>
#include <iostream>

#include <window/bitmap.hpp>
#include <window/video_mode.hpp>

#include "glm_utils.hpp"

using namespace glm_utils;

namespace draw {
namespace basics {
void drawRectangle(window::BitMap& bitmap, float _minX, float _minY, float _maxX,
                   float _maxY, const Color& _color) {
    int32_t minX = lround(_minX);
    int32_t minY = lround(_minY);
    int32_t maxX = lround(_maxX);
    int32_t maxY = lround(_maxY);

    if (minX < 0) { minX = 0; }
    if (minY < 0) { minY = 0; }
    if (maxX > (int32_t)bitmap.mode().width()) { maxX = bitmap.mode().width(); }
    if (maxY > (int32_t)bitmap.mode().height()) { maxY = bitmap.mode().height(); }

    uint32_t color = ((lround(_color.R * 255.0f) << 16) | (lround(_color.G * 255.0f) << 8)
                      | (lround(_color.B * 255.0f) << 0));

    uint8_t* Row = ((uint8_t*)bitmap.data() + minX * bitmap.mode().bytesPerPixel()
                    + minY * bitmap.mode().pitch());

    for (ssize_t Y = minY; Y < maxY; ++Y) {
        uint32_t* Pixel = (uint32_t*)Row;
        for (ssize_t X = minX; X < maxX; ++X) { *Pixel++ = color; }

        Row += bitmap.mode().pitch();
    }
}

void drawRectangle(window::BitMap& bitmap, const Rectangle<float>& _rect,
                   const Color& _color) {
    drawRectangle(bitmap, _rect.TopLeft.x, _rect.TopLeft.y, _rect.BottomRight.x,
                  _rect.BottomRight.y, _color);
}

template<typename T>
bool Rectangle<T>::overlaps(const Rectangle<T>& other) {
    auto& v1 = other.TopLeft;
    auto& v2 = other.BottomRight;
    auto v3  = glm::tvec2<T>{ other.TopLeft.x, other.BottomRight.y };
    auto v4  = glm::tvec2<T>{ other.BottomRight.x, other.TopLeft.y };
    return (v1 > TopLeft && v1 < BottomRight) || (v2 > TopLeft && v2 < BottomRight)
           || (v3 > TopLeft && v3 < BottomRight) || (v4 > TopLeft && v4 < BottomRight);
}

}   // namespace basics
}   // namespace draw
