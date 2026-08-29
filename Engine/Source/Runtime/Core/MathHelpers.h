#pragma once

inline bool FloatAlmostEqual(float a, float b, float epsilon)
{
    float diff = fabs(a - b);

    if (diff <= epsilon)
        return true;

    return diff <= epsilon * std::max(fabs(a), fabs(b));
}

inline bool MatrixAlmostEqual(const glm::mat4& a, const glm::mat4& b, float epsilon = 1e-4f)
{
    for (int col = 0; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
        {
            if (!FloatAlmostEqual(a[col][row], b[col][row], epsilon))
                return false;
        }
    }

    return true;
}