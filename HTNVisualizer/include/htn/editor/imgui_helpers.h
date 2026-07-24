#pragma once
#include <cmath>
#include <imgui.h>

inline ImVec2 ImNormalized(const ImVec2& v)
{
    const float lenSq = v.x * v.x + v.y * v.y;
    if (lenSq <= 0.0f)
        return ImVec2(0.0f, 0.0f);
    const float invLen = 1.0f / std::sqrt(lenSq);
    return ImVec2(v.x * invLen, v.y * invLen);
}