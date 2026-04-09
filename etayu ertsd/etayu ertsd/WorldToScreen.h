#pragma once
#include <cstdint>

struct Vector3 { float x, y, z; };
struct Vector2 { float x, y; };

class WorldToScreen {
public:
    static bool Project(const Vector3& world, Vector2& screen, const float matrix[4][4], int screenWidth, int screenHeight);
};