#include "main.h"
#include "clay_renderer_raylib.h"

bool debugEnabled = false;

int main(void) {
    NodeEditor node_render = { 0 };
    Font fonts[NUM_FONTS];
    
    if (node_renderer_init(&node_render, fonts) == 1) {
        fprintf(stderr, "Failed to initialize renderer\n");
        return 1;
    }
    Node node_test = { 0 };
    if (openNodeFile(ASSETS_PATH"/padding.node", &node_test) == 1) {
        return 1;
    }

    printNode(&node_test);
    while (!WindowShouldClose()) {
        // Update
        if (IsKeyPressed(KEY_D)) {
            debugEnabled = !debugEnabled;
            Clay_SetDebugModeEnabled(debugEnabled);
        }
        Clay_RenderCommandArray renderCommands = node_renderer_update(&node_render, &node_test);
        // Draw
        BeginDrawing();
            ClearBackground(BLACK);
            Clay_Raylib_Render(renderCommands, fonts);
        EndDrawing();
    }

    //free_node(&node_test);
    CloseWindow();
    return 0;
}