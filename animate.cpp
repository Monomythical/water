#include <cstdio>
#include <cmath>
#include <string>
#include "raylib.h"
#include "waterpool.hpp"

const int WIDTH = 160;
const int HEIGHT = 160;
using PoolType = Sapphire::WaterPool<WIDTH, HEIGHT>;

const int PIXELS_PER_CELL = 4;


struct RenderContext
{
    const int screenWidth  = WIDTH * PIXELS_PER_CELL;
    const int screenHeight = HEIGHT * PIXELS_PER_CELL;
    float zoom = 8000.0f;     // pixels per meter
    float xCenter = 0.05f;
    float yCenter = 0.00f;

    int xScreen(float x) const
    {
        return (screenWidth/2) + static_cast<int>(round(zoom * (x - xCenter)));
    }

    int yScreen(float y) const
    {
        return (screenHeight/2) - static_cast<int>(round(zoom * (y - yCenter)));
    }

    int scale(float r) const
    {
        return static_cast<int>(round(zoom * r));
    }

    static Color cellColor(const Sapphire::WaterCell& cell)
    {
        using namespace Sapphire;

        if (cell.wet == 0.0f)
            return WHITE;

        float g = 128.0f * (cell.pos + 1.0f);
        if (g > 255.0f)
            g = 255.0f;
        else if (g < 0.0f)
            g = 0.0f;
        return CLITERAL(Color){0, static_cast<unsigned char>(g), 32, 255};
    }

    void draw(const PoolType& pool)
    {
        using namespace Sapphire;

        for (int i = 0; i < WIDTH; ++i)
        {
            for (int j = 0; j < HEIGHT; ++j)
            {
                const WaterCell& cell = pool.getCell(i, j);
                Color color = cellColor(cell);
                DrawRectangle(i*PIXELS_PER_CELL, j*PIXELS_PER_CELL, PIXELS_PER_CELL, PIXELS_PER_CELL, color);
            }
        }
    }
};


int main(int argc, const char *argv[])
{
    using namespace Sapphire;

    float dt = 1.0f / 48000.0f;
    float halflife = 0.07f;
    float c = 2.0f;             // speed of waves in meters/second
    float s = 0.001f;           // grid spacing in meters
    float k = (c*c) / (s*s);    // propagation constant [second^(-2)]
    PoolType pool;
    RenderContext render;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int r = 1 + i*i + j*j;
            pool.getCell(i + WIDTH/5, j + HEIGHT/2).vel = +20000.0f / r;
        }
    }

    // Create reflective barriers.
    for (int i = 30; i+10 < WIDTH; ++i)
    {
        pool.getCell(i, HEIGHT/2-7).wet = 0.0f;
        pool.getCell(i-13, HEIGHT/2+17).wet = 0.0f;
    }

    InitWindow(render.screenWidth, render.screenHeight, "Water Simulation by Don Cross");
    SetTargetFPS(240);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        render.draw(pool);
        EndDrawing();
        pool.update(dt, halflife, k);
    }
    CloseWindow();
    return 0;
}
