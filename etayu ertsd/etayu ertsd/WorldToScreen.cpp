#include "WorldToScreen.h"

bool WorldToScreen::Project(const Vector3& world, Vector2& screen, const float matrix[4][4], int screenWidth, int screenHeight) {
    float w = matrix[3][0] * world.x + matrix[3][1] * world.y + matrix[3][2] * world.z + matrix[3][3];
    if (w < 0.01f) return false;

    float invW = 1.0f / w;
    float x = (matrix[0][0] * world.x + matrix[0][1] * world.y + matrix[0][2] * world.z + matrix[0][3]) * invW;
    float y = (matrix[1][0] * world.x + matrix[1][1] * world.y + matrix[1][2] * world.z + matrix[1][3]) * invW;

    screen.x = (screenWidth / 2.0f) * (1.0f + x);
    screen.y = (screenHeight / 2.0f) * (1.0f - y);
    return true;
}