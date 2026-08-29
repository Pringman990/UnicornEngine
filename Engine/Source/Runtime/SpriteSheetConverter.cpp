//
// Created on 2026-08-15.
//

#include "pch.h"
#include "SpriteSheetConverter.h"

SpriteSheet SpriteSheetConverter::Convert(const ByteBuffer& data, uint32 width, uint32 height, uint32 channels)
{
    SpriteSheet sheet;

    List<bool> visited(width * height, false);

    auto Index = [width](uint32 x, uint32 y) -> uint32
    {
        return y * width + x;
    };

    auto IsSolid = [&](const uint32 x, const uint32 y) -> bool
    {
        uint32 index = Index(x, y) * channels;
        return data[index + channels - 1] > 0;
    };

    auto InBounds = [width, height](int32 x, int32 y)
    {
        return x >= 0 && y >= 0 &&
               x < static_cast<int32>(width) &&
               y < static_cast<int32>(height);
    };

    for (uint32 y = 0; y < height; y++)
    {
        for (uint32 x = 0; x < width; x++)
        {
            if (visited[Index(x, y)] || !IsSolid(x, y))
                continue;

            visited[Index(x, y)] = true;

            uint32 minX = x;
            uint32 minY = y;
            uint32 maxX = x;
            uint32 maxY = y;

            List<glm::uvec2> stack;
            stack.emplace_back(x, y);

            while (!stack.empty())
            {
                auto p = stack.back();
                stack.pop_back();

                minX = std::min(minX, p.x);
                minY = std::min(minY, p.y);
                maxX = std::max(maxX, p.x);
                maxY = std::max(maxY, p.y);

                for (int32 dy = -1; dy <= 1; dy++)
                {
                    for (int32 dx = -1; dx <= 1; dx++)
                    {
                        if (dy == 0 && dx == 0)
                            continue;

                        int32 newX = static_cast<int32>(p.x) + dx;
                        int32 newY = static_cast<int32>(p.y) + dy;

                        if (!InBounds(newX, newY))
                            continue;

                        auto ux = static_cast<uint32>(newX);
                        auto uy = static_cast<uint32>(newY);

                        if (visited[Index(ux, uy)] || !IsSolid(ux, uy))
                            continue;

                        visited[Index(newX, newY)] = true;
                        stack.emplace_back(newX, newY);
                    }
                }
            }
            sheet.frames.emplace_back(minX, minY, maxX - minX + 1, maxY - minY + 1);
        }
    }

    return sheet;
}
