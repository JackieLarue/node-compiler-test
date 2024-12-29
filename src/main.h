#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "./clay/clay.h"
#include "./clay/clay_renderer_raylib.h"
#include "raylib.h"

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
    OUTGOING_NAMED,  // = 0b000,
    INCOMING_NAMED,  // = 0b001,
    INCOMING_NUMBER, // = 0b010,
    INCOMING_SELECT, // = 0b011,
    INCOMING_SWITCH, // = 0b100,
    INCOMING_TEXT    // = 0b101
} SocketType;

typedef struct Position16 {
    uint16_t x;
    uint16_t y;
} Position16;

typedef struct NodeAndSocket {
    uint8_t node;
    uint8_t socket;
} NodeAndSocket;

typedef struct U8String {
    uint8_t len;
    char* str;
} U8String;

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
    NodeAndSocket* connections;
} NodeRoots;

typedef struct Socket {
    //type+direction of socket
    SocketType type;

    bool is_repetitive;
    bool is_connected;
    bool switch_value;

    uint8_t type_index;
    uint8_t port_slot;

    NodeAndSocket connection;
    
    U32String value;
} Socket;

typedef struct InstanceEntry {
    uint8_t key;
    uint8_t type;
    //instance position and lengths
    Position16 pos;
    uint8_t name_len; //technically 6 bits, not 8
    uint8_t sock_len; //ditto
    
    char* name;
    Socket* sockets;
} InstanceEntry;

typedef struct NodeInstances {
    uint8_t instance_count;
    InstanceEntry* instance_table;
} NodeInstances; 

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
    NodeInstances instance;
} Node;

int parseNode(FILE* src, Node* dest);
void printNode(Node* node);
int openNodeFile(char* path, Node* node);
int errorMessage(FILE* src, char* err_msg);

//GUI Time

typedef struct {
    Clay_String title;
    Clay_String contents;
} Document;

typedef struct {
    Document *documents;
    uint32_t length;
} DocumentArray;

extern DocumentArray documents;
extern uint32_t selectedDocumentIndex;

void RenderHeaderButton(Clay_String text);
void RenderDropdownMenuItem(Clay_String text);
void HandleSidebarInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
void HandleClayErrors(Clay_ErrorData errorData);

int initializeRenderer();
void updateRenderer();

extern const int FONT_ID_BODY_16;
extern Clay_Color COLOR_WHITE;