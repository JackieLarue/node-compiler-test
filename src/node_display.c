#include "main.h"
#include "clay_renderer_raylib.h"

// This function is new since the video was published
void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

int node_renderer_init(NodeEditor *editor, Font* fonts) {
    Clay_Raylib_Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, "Node Edit Test", FLAG_WINDOW_RESIZABLE /*| FLAG_WINDOW_HIGHDPI*/ | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    
    util_font_load(fonts);
    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    char* clay_mem = malloc(clayRequiredMemory);
    if (clay_mem == NULL) {
        printf("Failed to allocate memory for Clay.");
        return 1;
    }
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, clay_mem);
    
    Clay_Initialize(clayMemory, (Clay_Dimensions) {
       .width = (float) GetScreenWidth(),
       .height = (float) GetScreenHeight()
    }, (Clay_ErrorHandler) { .errorHandlerFunction = HandleClayErrors }); // This final argument is new since the video was published
    
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    memset(editor, 0, sizeof(*editor));
    editor->begin = NULL;
    editor->end = NULL;
    //node_editor_add(editor, "Source", (Vector2) { 40.0f, 10.0f }, (Vector2) { 180.0f, 220.0f }, NODE_GREY, RED, 0, 1);
    //node_editor_add(editor, "Source", (Vector2) { 40.0f, 260.0f }, (Vector2) { 180.0f, 220.0f }, NODE_GREY, GREEN, 0, 1);
    //node_editor_add(editor, "Combine", (Vector2) { 400.0f, 100.0f }, (Vector2) { 180.0f, 220.0f }, NODE_GREY, BLUE, 2, 2);
    //node_editor_link(editor, 0, 0, 2, 0);
    //node_editor_link(editor, 1, 0, 2, 1);
    editor->show_grid = true;

    return 0;
}

void socket_draw_loop() {

}

void node_draw_loop(Node* node, Clay_Color color, int scale) {
    for (int i = 0; i < node->instance.len; i++) {
        Instance it = node->instance.table[i];
        
        float y_size = it.socket.len * 2 + 4;
        float offset_x = (it.pos.x + 500) * scale;
        float offset_y = (it.pos.y + 500) * scale;
        CLAY({
            .id = CLAY_IDI("NodeInstanceKey", it.key),
            .floating = {
                .offset = { offset_x, offset_y },
                .attachTo = CLAY_ATTACH_TO_PARENT,
            },
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                //.padding = {  },
                //.childGap = {  },
                .sizing = { 
                    .width  = CLAY_SIZING_GROW(0), 
                    .height = CLAY_SIZING_FIXED((it.socket.len + 1) * scale),
                },
            },
            .backgroundColor = color,
        }) {
            node_name_component(it.key, U8_to_Clay(it.name));
            //socket_draw_loop(node_editor, it);
        }
    }
    // for (int n = 0; n < node_editor->connection_count; n++) {
        
    // }
}
//for kronark nodes, the ui seems to be as follows:
//name
//space
//all other properties
//the width is fixed, as well as the height

Clay_RenderCommandArray node_renderer_update(NodeEditor* node_editor, Node* node) { 
    Clay_Dimensions layoutDimensions = {
        .width = GetScreenWidth(),
        .height = GetScreenHeight()
    };
    Clay_Vector2 mousePosition = clay_ray_vec2(GetMousePosition());
    Clay_Vector2 scrollDelta = clay_ray_vec2(GetMouseWheelMoveV());

    Clay_SetLayoutDimensions(layoutDimensions);
    Clay_SetPointerState(mousePosition, IsMouseButtonDown(0));
    Clay_UpdateScrollContainers(true, scrollDelta, GetFrameTime());

    int some_stupid_val = node_editor->show_grid;

    Clay_BeginLayout();
    CLAY({ 
        .id = CLAY_ID("OuterContainer"), 
        .layout = { 
            .sizing = layoutExpand, 
            .padding = CLAY_PADDING_ALL(16), 
            .childGap = 16 
        }, 
        .backgroundColor = COLOR_WHITE, 
    }) {
        CLAY({ 
            .id = CLAY_ID("MainContent"), 
            .layout = { .sizing = layoutExpand }, 
            .backgroundColor = COLOR_LIGHT,
            .scroll = { .vertical = true, .horizontal = true },
        }) {
            node_draw_loop(node, NODE_GREY, 1.0f);
        }
    }
    
    return Clay_EndLayout();
}
