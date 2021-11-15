#include <ostream>

#include <glm/gtx/string_cast.hpp>
#include <glm/vec2.hpp>

namespace glm_utils {
bool operator<(glm::vec2& v1, glm::vec2 v2) { return v1.x < v2.x && v1.y < v2.y; }
bool operator>(glm::vec2& v1, glm::vec2 v2) { return v1.x > v2.x && v1.y > v2.y; }

bool operator<=(glm::vec2& v1, glm::vec2 v2) { return !(v1 > v2); }
bool operator>=(glm::vec2& v1, glm::vec2 v2) { return !(v1 < v2); }

// @XXX add proper operator+= and operator -= to glm ?

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

}   // namespace glm_utils
