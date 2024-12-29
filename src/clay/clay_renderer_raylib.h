#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef CLAY_OVERFLOW_TRAP
#include "signal.h"
#endif

#define CLAY_IMPLEMENTATION
#include "clay.h"

typedef struct Raylib_Font
{
    uint32_t fontId;
    Font font;
} Raylib_Font;

typedef enum CustomLayoutElementType
{
    CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL
} CustomLayoutElementType;

typedef struct CustomLayoutElement_3DModel
{
    Model model;
    float scale;
    Vector3 position;
    Matrix rotation;
} CustomLayoutElement_3DModel;

typedef struct CustomLayoutElement
{
    CustomLayoutElementType type;
    union {
        CustomLayoutElement_3DModel model;
    };
} CustomLayoutElement;

extern Raylib_Font Raylib_fonts[10];
extern Camera Raylib_camera;
extern uint32_t measureCalls;

Ray GetScreenToWorldPointWithZDistance(Vector2 position, Camera camera, int screenWidth, int screenHeight, float zDistance);
Clay_Dimensions Raylib_MeasureText(Clay_String *text, Clay_TextElementConfig *config);
void Clay_Raylib_Initialize(int width, int height, const char *title, unsigned int flags);
void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands);