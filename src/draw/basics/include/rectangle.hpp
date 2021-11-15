#include <draw/basics/colors.hpp>
#include <ostream>

#include <window/bitmap.hpp>
#include <window/video_mode.hpp>

#include <glm/gtx/string_cast.hpp>
#include <glm/vec2.hpp>

namespace draw {
namespace basics {
template<typename T>
struct Rectangle {
    glm::tvec2<T> TopLeft;
    glm::tvec2<T> BottomRight;

    bool fill = true;

    Rectangle(glm::tvec2<T> tl, glm::tvec2<T> br) : TopLeft(tl), BottomRight(br) {}
    Rectangle(Rectangle&) = default;
    Rectangle(std::initializer_list<T> _initList) {
        assert(_initList.size() == 4 && "A rectangle is composed of only 2 vec2");
        float x1, y1, x2, y2;
        for (int i = 0; auto v : _initList) {
            if (i == 0) x1 = v;
            if (i == 1) y1 = v;
            if (i == 2) x2 = v;
            if (i == 3) y2 = v;
            i++;
        }
        TopLeft     = glm::tvec2<T>{ x1, y1 };
        BottomRight = glm::tvec2<T>{ x2, y2 };
    }

    glm::tvec2<T> topLeft() const { return TopLeft; }
    glm::tvec2<T> bottomRight() const { return BottomRight; }
    glm::tvec2<T> topRight() const { return glm::tvec2<T>(BottomRight.x, TopLeft.y); }
    glm::tvec2<T> bottomLeft() const { return glm::tvec2<T>(TopLeft.x, BottomRight.y); }

    void setFill(bool _fill) { fill = _fill; }

    Rectangle operator+(const glm::tvec2<T>& other) {
        return Rectangle(TopLeft + other, BottomRight + other);
    }

    Rectangle operator-(const glm::tvec2<T>& other) {
        return Rectangle(TopLeft - other, BottomRight - other);
    }

    bool overlaps(const Rectangle<T>& other);

    friend std::ostream& operator<<(std::ostream& os, Rectangle& rect) {
        os << "{" << rect.TopLeft << ", " << rect.BottomRight << "}";
        return os;
    }
};

void drawRectangle(window::BitMap& bitmap, float minX, float minY, float maxX, float maxY,
                   const Color& _color);
void drawRectangle(window::BitMap& bitmap, const Rectangle<float>& _rect,
                   const Color& _color);

}   // namespace basics
}   // namespace draw
