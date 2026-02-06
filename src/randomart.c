#include "../vendor/raylib/raylib.h"
#include <stdbool.h>


int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [shaders] example - multi sample2d");

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(YELLOW);
            DrawRectangle(10,10,GetScreenWidth()-20,GetScreenHeight()-20,RED);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
