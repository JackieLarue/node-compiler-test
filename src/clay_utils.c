#include "main.h"

Clay_Sizing layoutExpand = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) };
Clay_ChildAlignment alignCenter = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER };
//ScrollbarData scrollbarData = (ScrollbarData) {};

void util_font_load(Font* fonts) {
    fonts[FONT_ID_BODY_24] = LoadFontEx(ASSETS_PATH "/Roboto-Regular.ttf", 48, 0, 400);
    SetTextureFilter(fonts[FONT_ID_BODY_24].texture, TEXTURE_FILTER_BILINEAR);
    fonts[FONT_ID_BODY_16] = LoadFontEx(ASSETS_PATH "/Roboto-Regular.ttf", 32, 0, 400);
    SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
}

Clay_String U8_to_Clay(U8String string) {
    return (Clay_String) { .length = string.len, .chars = string.str };
}

Clay_String U32_to_Clay(U32String string) {
    return (Clay_String) { .length = string.len, .chars = string.str };
}

Clay_Vector2 clay_ray_vec2(Vector2 vec) {
    return (Clay_Vector2) { vec.x, vec.y };
}

double clay_dround(double d) {
    return floor(d + 0.5);
}

float clay_fmapi(int input, int input_start, int input_end, int output_start, int output_end) {
    double slope = 1.0 * (output_end - output_start) / (input_end - input_start);
    return output_start + clay_dround(slope * (input - input_start));
}

Position16 cam_apply(NodeCamera* cam, Position16 pos) {
    return (Position16) { (pos.x * cam->scale.x) - cam->pos.x, (pos.y * cam->scale.y) - cam->pos.y };
}

// void handle_node_click_interaction(
//     Clay_ElementId elementID,
//     Clay_PointerData pointerData,
//     intptr_t userData
// ) {
//     ScrollbarData* scrollBarData = (ScrollbarData*)userData;
//     if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
//         Clay_ElementId id = elementID;
//     }
// }

void handle_draggable_hover(
    Clay_ElementId elementID,
    Clay_PointerData pointerData,
    intptr_t userData
) {
    NodeCamera* camera = (NodeCamera*)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        Clay_ElementId id = elementID;
    }    
}

void render_header_button(Clay_String text) {
    CLAY({
        .layout = { .padding = { 16, 16, 8, 8 } },
        .backgroundColor = { 140, 140, 140, 255 },
        .cornerRadius = CLAY_CORNER_RADIUS(5)
    }) {
        CLAY_TEXT(
            text,
            CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = COLOR_WHITE,
            })
        );
    }
}

void render_dropdown_menu_item(Clay_String text) {
    CLAY({
        .layout = { .padding = CLAY_PADDING_ALL(16) }
    }) {
        CLAY_TEXT(
            text,
            CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = COLOR_WHITE,
            })
        );
    }
}

void sidebar_item_component() {
    CLAY({
        .layout = { 
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) }
        },
        .backgroundColor = COLOR_ORANGE
    }) {
        // children go here...
    }
}

void node_name_component(int id, Clay_String name) {
    CLAY({
        .id = CLAY_IDI("Node Name Socket", id),
        .layout = {
            .childAlignment = alignCenter,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50), },
        },
    }) {
        CLAY({
            .id = CLAY_IDI("Node Name Socket Border", id),
            .layout = { .padding = { 16, 16, 6, 6 } },
            .backgroundColor = COLOR_WHITE,
        }) {
            CLAY_TEXT(
                name,
                CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_24,
                    .fontSize = 24,
                    .textColor = COLOR_BLACK,
                })
            );
        }
    }
}

void socket_box_component(int id, SocketType type, Clay_String box_name, Clay_String content) {
    //socket visual types:
    //name - white background, black text, centered
    //output - is always concrete, i.e. all black background + white text,
    //no dividers. aligned left.
    //input - 3 types: black background, white text, white background,
    //black text, and finally, 1/3 black background, with white text, then
    //rest white background with black text. aligned right.
    Clay_ChildAlignment childAlignment;
    Clay_Sizing sizing;
    Clay_Color backGroundColor;
    Clay_Color textColor;

    Clay_String some_temp = content;
    SocketType some_type = type;

    childAlignment = alignCenter;
    sizing = (Clay_Sizing) { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) };
    backGroundColor = COLOR_WHITE;
    textColor = COLOR_BLACK;
    // switch (type) {
    //     case OUTGOING_NAMED:
    //         break;
    //     case INCOMING_NAMED:
    //         break;
    //     case INCOMING_NUMBER:
    //         break;
    //     case INCOMING_SELECT:
    //         break;
    //     case INCOMING_SWITCH:
    //         childAlignment = alignCenter;
    //         sizing = (Clay_Sizing) { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) };
    //         backGroundColor = COLOR_WHITE;
    //         textColor = COLOR_BLACK;
    //         break;
    //     case INCOMING_TEXT:
    //         break;
    // }
    CLAY({
        .id = CLAY_IDI("Node Socket", id),
        .layout = {
            .childAlignment = childAlignment,
            .sizing = sizing,
        },
    }) {
        CLAY({
            .id = CLAY_IDI("Node Socket Border", id),
            .layout = { .padding = {16, 16, 6, 6 } },
            .backgroundColor = backGroundColor,
        }) {
            CLAY_TEXT(
                box_name,
                CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_24,
                    .fontSize = 24,
                    .textColor = textColor,
                })
            );
        }
    }
}
