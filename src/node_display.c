#include "main.h"
#include "./clay/clay.h"
#include "./clay/clay_renderer_raylib.h"

// This function is new since the video was published
void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

int initializeRenderer() {
#if 0
    Clay_Raylib_Initialize(1024, 768, "Introducing Clay Demo", FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT); // Extra parameters to this function are new since the video was published

    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    float req_mem_f = clayRequiredMemory;
    req_mem_f = (req_mem_f / 1000.0f) / 1000.0f;
    printf("INFO: CLAY REQUIRED MEMORY: %fMB\n", req_mem_f);
    
    char* clay_mem = malloc(clayRequiredMemory);
    if (clay_mem == NULL) {
        printf("Failed to allocate memory for clay.");
        return 1;
    }

    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, clay_mem);
    
    Clay_Initialize(clayMemory, (Clay_Dimensions) {
       .width = GetScreenWidth(),
       .height = GetScreenHeight()
    }, (Clay_ErrorHandler) { HandleClayErrors }); // This final argument is new since the video was published
    
    Clay_SetMeasureTextFunction(Raylib_MeasureText);
    Font roboto_regular = LoadFontEx(ASSETS_PATH"/Roboto-Regular.ttf", 48, 0, 400);
    if (!IsFontValid(roboto_regular)) {
        printf("Font is invalid/didn't load.");
        return 1;
    }
    Raylib_fonts[FONT_ID_BODY_16] = (Raylib_Font) {
        .font = roboto_regular,
        .fontId = FONT_ID_BODY_16
    };
    SetTextureFilter(Raylib_fonts[FONT_ID_BODY_16].font.texture, TEXTURE_FILTER_BILINEAR);    
#endif

    return 0;
}

void updateRenderer() {

}