#include "main.h"
#include "./clay/clay.h"
#include "./clay/clay_renderer_raylib.h"
#include "raylib.h"

int main(void) {
    if (initializeRenderer() == 1) {
        fprintf(stderr, "Failed to initialize renderer\n");
        return 1;
    }
    Node bytes_size;
    if (openNodeFile(ASSETS_PATH"/bytes_size.node", &bytes_size) == 1) {
        return 1;
    }

    while (!WindowShouldClose()) {
        updateRenderer();
    }

    return 0;
}