#pragma once

#include "raylib.h"
#include "raymath.h"
#include "clay.h"

extern Camera Raylib_camera;

Ray GetScreenToWorldPointWithZDistance(
    Vector2 position,
    Camera camera,
    int screenWidth,
    int screenHeight,
    float zDistance
);
Clay_Dimensions Raylib_MeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* userData);
void Clay_Raylib_Initialize(int width, int height, const char *title, unsigned int flags);
void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands, Font* fonts);