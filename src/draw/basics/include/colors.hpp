namespace draw {
namespace basics {

struct Color {
    float R, G, B;
    Color() : R(0.0f), G(0.0f), B(0.0f) {}
    Color(float R, float G, float B) : R(R), G(G), B(B) {}

    static Color Red() { return Color(1.0f, 0.0f, 0.0f); }
    static Color Green() { return Color(0.0f, 1.0f, 0.0f); }
    static Color Blue() { return Color(0.0f, 0.0f, 1.0f); }
    static Color Cyan() { return Color(0.0f, 1.0f, 1.0f); }
    static Color Magenta() { return Color(1.0f, 0.0f, 1.0f); }
    static Color Yellow() { return Color(1.0f, 1.0f, 0.0f); }
    static Color White() { return Color(1.0f, 1.0f, 1.0f); }
    static Color Black() { return Color(0.0f, 0.0f, 0.0f); }
    static Color Gray(float scale = 0.5) { return Color(scale, scale, scale); }
};

}   // namespace basics
}   // namespace draw
