#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "flecs.h"

#include "clay_renderer_raylib.h"
#include "raylib.h"
#include "raymath.h"

#define COLOR_WHITE (Clay_Color) {255, 255, 255, 255}
#define COLOR_BLACK (Clay_Color) {0, 0, 0, 255}
#define COLOR_LIGHT (Clay_Color) {224, 215, 210, 255}
#define COLOR_RED (Clay_Color) {168, 66, 28, 255}
#define COLOR_ORANGE (Clay_Color) {225, 138, 50, 255}
#define COLOR_LIGHT_HOVER (Clay_Color) {224, 215, 210, 255}
//white, black, grey, tan
#define NODE_GREY (Clay_Color) {31, 31, 31, 255}
#define NODE_TAN (Clay_Color) {215, 215, 190, 255}

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define FONT_ID_BODY_16 1
#define FONT_ID_BODY_24 0
#define NUM_FONTS 2

#define NULL_STRING \
    (Clay_String) { \
        .length = 0, .chars = NULL \
    }

#define TYPE_TABLE(type) \
    typedef struct { \
        uint8_t len, type *table \
    }##type##Table;

typedef enum BuiltinNodeType {
    //CUSTOM = 0x00 ... 0xEC,
    BUILTIN_COLLECT = 0xED,
    BUILTIN_SPLIT,
    BUILTIN_TIME,
    BUILTIN_REPEAT,
    BUILTIN_MATH,
    BUILTIN_VALUE,
    BUILTIN_REVERSE,
    BUILTIN_FILE,
    BUILTIN_SIZE,
    BUILTIN_APPLY,
    BUILTIN_TYPE,
    BUILTIN_FORMAT,
    BUILTIN_CONDITION,
    BUILTIN_OPTION,
    BUILTIN_JOIN,
    BUILTIN_BYTES,
    BUILTIN_PATH,
    BUILTIN_SETTINGS,
    BUILTIN_PORT
} BuiltinNodeType;

typedef enum BuiltinDataType {
    //CUSTOM = 0x00 ... 0xE6,
    ROOT_INPUT = 0xE7,
    ROOT_OUTPUT,
    PATH_ABSOLUTE,
    PATH_MODULE,
    PORT_VALUE,
    PORT_CHANNEL,
    PORT_DEFAULT,
    REPETITIVE_PORT_VALUE,
    REPETITIVE_PORT_DEFAULT,
    REPETITIVE_TEXT,
    REPETITIVE_NUMBER,
    REPETITIVE_TRUTH,
    REPETITIVE_BYTES,
    REPETITIVE_SELECTION,
    TEXT,
    NUMBER,
    TRUTH,
    BYTES,
    SELECTION,
    OPTION_WHEN,
    OPTION_THEN,
    SETTINGS,
    REPETITION,
    ANY,
    NONE
} BuiltinDataType;

typedef enum SocketType {
    OUTGOING_NAMED, // = 0b000,
    INCOMING_NAMED, // = 0b001,
    INCOMING_NUMBER, // = 0b010,
    INCOMING_SELECT, // = 0b011,
    INCOMING_SWITCH, // = 0b100,
    INCOMING_TEXT // = 0b101
} SocketType;

typedef struct Position16 {
    int16_t x;
    int16_t y;
} Position16;

typedef struct Connection {
    uint8_t node;
    uint8_t socket;
} Connection;

typedef struct U8String {
    uint8_t len;
    char* str;
} U8String;

//TYPE_TABLE(U8String)

typedef struct StringTable {
    uint8_t table_size;
    U8String* table;
} StringTable, NodeIDS, NodeTypes;

typedef struct U32String {
    uint32_t len;
    char* str;
} U32String;

typedef struct NodeRoots {
    Position16 in_pos;
    Position16 out_pos;
    uint8_t num_connections;
    Connection* connections;
} NodeRoots;

typedef struct Socket {
    //type+direction of socket
    SocketType type;

    bool is_repetitive;
    bool is_connected;
    bool switch_value;

    uint8_t type_index;
    uint8_t port_slot;

    Connection connection;

    U32String value;
} Socket;

typedef struct SocketTable {
    uint8_t len;
    Socket* table;
} SocketTable;

typedef struct Instance {
    uint8_t key;
    uint8_t type;
    //instance position and lengths
    Position16 pos;
    U8String name;
    SocketTable socket;
} Instance;

typedef struct InstanceTable {
    uint8_t len;
    Instance* table;
} InstanceTable;

typedef struct Node {
    //format version of this node
    uint8_t version;

    //inout positions and out connections
    NodeRoots root;

    //the ids of all nodes this node refers to
    NodeIDS id;

    //the list of non-reserved types this nodes uses
    NodeTypes type;

    //the list of node instances this node contains
    InstanceTable instance;
} Node;

int parseNode(FILE* src, Node* dest);
void printNode(Node* node);
int openNodeFile(char* path, Node* node);
int errorMessage(FILE* src, char* err_msg);
void free_node(Node* node);

//GUI Time
typedef struct
{
    Clay_Vector2 clickOrigin;
    Clay_Vector2 positionOrigin;
    bool mouseDown;
} ScrollbarData;

typedef struct NodeCamera {
    Position16 pos;
    Vector2 scale;
} NodeCamera;

typedef union Additional {
    struct Text   { char* min; char* max; char* valid; } text;
    struct Number { char* min; char* max; char* step;  } num;
    struct Switch { char* on ; char* off;              } swch;
    struct Select { int smth;                          } select;
} Additional;

typedef struct SocketGUI {
    char* name;
    Socket socket;
    //SocketDefault?
    //Additional?
    bool connective;
} SocketGUI;

typedef struct TableSocketGUI {
    size_t len;
    SocketGUI* tbl;
} TableSocketGUI;

typedef struct NodeGUI {
    Instance* inst;
    Clay_Color color;
} NodeGUI;

typedef struct NodeEditor {
    int initalized;
    Node node_buf[32];
    //Connection connections[64];
    Node* begin;
    Node* end;
    size_t node_count;
    size_t connection_count;
    Rectangle bounds;
    Node* selected;
    int show_grid;
    Vector2 scrolling;
    //Connecting connecting;
} NodeEditor;

void HandleClayErrors(Clay_ErrorData errorData);

extern Clay_Sizing layoutExpand;
extern Clay_ChildAlignment alignCenter;

void util_font_load(Font* fonts);
Clay_String U8_to_Clay(U8String string);
Clay_String U32_to_Clay(U32String string);
Clay_Vector2 clay_ray_vec2(Vector2 vec);
void render_header_button(Clay_String text);
void render_dropdown_menu_item(Clay_String text);
void sidebar_item_component();
void node_name_component(int id, Clay_String name);
void socket_box_component(int id, SocketType type, Clay_String box_name, Clay_String content);

void node_editor_push(NodeEditor* editor, Node* node);
void node_editor_pop(NodeEditor* editor, Node* node);
Node* node_editor_find(NodeEditor* editor, int id);
void node_editor_add(
    NodeEditor* editor,
    const char* name,
    Vector2 pos,
    Vector2 size,
    Color col,
    Color col_val,
    int in_count,
    int out_count
);
void node_editor_link(NodeEditor* editor, int in_id, int in_slot, int out_id, int out_slot);
int node_renderer_init(NodeEditor* editor, Font* fonts);
Clay_RenderCommandArray node_renderer_update(NodeEditor* node_editor, Node* node);