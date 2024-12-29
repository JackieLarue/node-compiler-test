#include "main.h"

#define LATEST_VERSION 1
#define NODE_SOCKET_SIZE 2
char magic_constant[11] = "kronarknode";

int errorMessage(FILE* src, char* err_msg) {
    fprintf(stderr, "%s", err_msg);
    fclose(src);
    return 1;
}

int indexErrorMessage(FILE* src, char* err_msg, size_t index) {
    fprintf(stderr, "%s at index %zu\n", err_msg, index);
    fclose(src);
    return 1;
}

int nestedIndexErrorMessage(FILE* src, char* err_msg, size_t index_1, size_t index_2) {
    fprintf(stderr, "%s at index %zu subindex %zu\n", err_msg, index_1, index_2);
    fclose(src);
    return 1;
}

int stringArrayErrorMessage(FILE* src, char* err_msg, char** str_arr, size_t index_err) {
    for (size_t i = 0; i < index_err; i++) {
        free(str_arr[i]);
    }
    free(str_arr);
    return indexErrorMessage(src, err_msg, index_err);    
}

int parseNameTable(FILE* src, StringTable* str_tbl) {
    char name_tbl_len;
    if (fread(&name_tbl_len, 1, 1, src) != 1) {
        return errorMessage(src, "Failed to read node or type name table size.\n");
    }
    str_tbl->table_size = name_tbl_len;

    if (str_tbl->table_size > 0) {
        char** node_names = malloc(str_tbl->table_size * sizeof(char*));
        if (node_names == NULL) {
            return errorMessage(src, "Failed to allocate node or type name table buffer.\n");
        }
        
        str_tbl->table = malloc(str_tbl->table_size * sizeof(StringTable));
        if (str_tbl->table == NULL) {
            return errorMessage(src, "Failed to allocate node or type name table.\n");
        }

        for (size_t i = 0; i < str_tbl->table_size; i++) {
            uint8_t str_length;
            if (fread(&str_length, 1, 1, src) != 1) {
                return stringArrayErrorMessage(src, "Failed to read string length for node or type name", node_names, i); //free previously allocated strings
            }
            str_tbl->table[i].len = str_length;

            //allocate memory for string buffer
            node_names[i] = malloc(str_length + 1); //+1 for null termination
            if (node_names[i] == NULL) {
                return stringArrayErrorMessage(src, "Failed to allocate node or type name string", node_names, i);
            }
            //allocate memory for struct string 
            str_tbl->table[i].str = malloc(str_length + 1); //+1 for null termination
            if (str_tbl->table[i].str == NULL) {
                return stringArrayErrorMessage(src, "Failed to allocate node or type name string", node_names, i);
            }

            if (fread(node_names[i], 1, str_length, src) != str_length) {
                return stringArrayErrorMessage(src, "Failed to read string", node_names, i);
            }
            node_names[i][str_length] = '\0'; //null terminate the damn thing
            strcpy(str_tbl->table[i].str, node_names[i]);
        }
    }
    return 0;    
}

int parseInstances(FILE* src, NodeInstances* inst) {
    uint8_t inst_count;
    if (fread(&inst_count, 1, 1, src) != 1) {
        return errorMessage(src, "Failed to read instance count.\n");
    }
    inst->instance_count = inst_count;
    
    inst->instance_table = malloc(inst->instance_count * sizeof(NodeInstances));
    if (inst->instance_table == NULL) {
        return errorMessage(src, "Failed to allocate instance table.\n");
    }
    for (size_t i = 0; i < inst->instance_count; i++) {
        uint8_t key_type_flags[6] = { 0 };
        if (fread(key_type_flags, 1, 6, src) != 6) {
            return errorMessage(src, "Failed to read instance key or type or flags.\n");
        }
        inst->instance_table[i].key  = key_type_flags[0];
        inst->instance_table[i].type = key_type_flags[1];

 		inst->instance_table[i].pos.x = ((uint16_t)(key_type_flags[2]             ) <<2 | (uint16_t)((key_type_flags[2] >>(8-2)) & 0b00000011));
 		inst->instance_table[i].pos.y = ((uint16_t)(key_type_flags[3] & 0b00111111) <<4 | (uint16_t)((key_type_flags[3] >>(8-4)) & 0b00001111));

        inst->instance_table[i].name_len = (key_type_flags[4] & 0b00001111) <<2 | ((key_type_flags[5] >>(8-2)) & 0b00000011);
        inst->instance_table[i].sock_len =  key_type_flags[5] & 0b00111111;

        inst->instance_table[i].name = malloc(inst->instance_table[i].name_len + 1); //+1 for null termination
        if (inst->instance_table[i].name == NULL) {
            return indexErrorMessage(src, "Failed to allocate instance name string", i);
        }
        if (fread(inst->instance_table[i].name, 1, inst->instance_table[i].name_len, src) != inst->instance_table[i].name_len) {
            return indexErrorMessage(src, "Failed to read instance name string", i);
        }
        inst->instance_table[i].name[inst->instance_table[i].name_len] = '\0'; //null terminate the damn thing (x2!)

        inst->instance_table[i].sockets = malloc(inst->instance_table[i].sock_len * sizeof(Socket));
        if (inst->instance_table[i].sockets == NULL) {
            return indexErrorMessage(src, "Failed to allocate instance socket", i);
        }
        for (size_t j = 0; j < inst->instance_table[i].sock_len; j++) {
            uint8_t socket_flags;
            if (fread(&socket_flags, sizeof(socket_flags), 1, src) != 1) {
                return errorMessage(src, "Failed to read instance socket flags.\n");
            }
            inst->instance_table[i].sockets[j].type = (SocketType)(socket_flags >> 3 & 0b111);
            inst->instance_table[i].sockets[j].is_connected =  (socket_flags & 0b00000010) != 0;
            inst->instance_table[i].sockets[j].is_repetitive = (socket_flags & 0b00000100) != 0;
            inst->instance_table[i].sockets[j].switch_value =  (socket_flags & 0b00000001) != 0;

            uint8_t type_port[2] = { 0 };
            if (fread(type_port, sizeof(type_port), 1, src) != 1) {
                return errorMessage(src, "Failed to read socket type index or socket port slot\n");
            }
            inst->instance_table[i].sockets[j].type_index = type_port[0];
            inst->instance_table[i].sockets[j].port_slot  = type_port[1];

            if (inst->instance_table[i].sockets[j].type != OUTGOING_NAMED) {
                if (inst->instance_table[i].sockets[j].is_connected) {
                    NodeAndSocket ns;
                    if (fread(&ns, sizeof(NodeAndSocket), 1, src) != 1) {
                        return errorMessage(src, "Failed to read instance socket incoming node and socket\n");
                    }
                    inst->instance_table[i].sockets[j].connection = ns;
                }
                else if (inst->instance_table[i].sockets[j].type != INCOMING_SWITCH) {
                    uint32_t len_be;
                    if (fread(&len_be, sizeof(uint32_t), 1, src) != 1) {
                        return errorMessage(src, "Failed to read instance socket value length\n");
                    }
                    uint32_t len = ((len_be>>24) & 0x000000ff) | // move byte 3 to byte 0
                                   ((len_be<<8 ) & 0x00ff0000) | // move byte 1 to byte 2
                                   ((len_be>>8 ) & 0x0000ff00) | // move byte 2 to byte 1
                                   ((len_be<<24) & 0xff000000); // byte 0 to byte 3
                    inst->instance_table[i].sockets[j].value.len = len;
                    
                    inst->instance_table[i].sockets[j].value.str = malloc(inst->instance_table[i].sockets[j].value.len + 1); //+1 for blah blah you get it already
                    if (inst->instance_table[i].sockets[j].value.str == NULL) {
                        return nestedIndexErrorMessage(src, "Failed to allocate instance socket value string", i, j);
                    }
                    size_t fread_value = fread(inst->instance_table[i].sockets[j].value.str, 1, len, src);
                    if (fread_value != len) {
                        //inst->instance_table[i].sockets[j].value.str[len] = '\0';
                        //printf("%s\n", inst->instance_table[i].sockets[j].value.str);
                        //printf("Fread elements returned, len value: %zu, %u\n", fread_value, len);
                        return nestedIndexErrorMessage(src, "Failed to read instance socket value string", i, j);
                    }
                    
                    inst->instance_table[i].sockets[j].value.str[len] = '\0';
                }
            }
        }
    }

    return 0;
}

int parseNode(FILE* src, Node* dest) {
    char magic[11] = { 0 };
    if (fread(magic, 11, 1, src) != 1) {
        return errorMessage(src, "Failed to read magic.\n");
    }

    if (strncmp(magic_constant, magic, 11) != 0) {
        return errorMessage(src, "Not a valid kronark node file extension.\n");
    }

    uint8_t version;
    if(fread(&version, 1, 1, src) != 1) {
        return errorMessage(src, "Failed to read version\n");
    }

    if (version > LATEST_VERSION) {
        return errorMessage(src, "Sorry bub, either your file is from the future or it got corrupted.\n");
    }
    dest->version = version;

    uint8_t pos[6] = { 0 };
    if (fread(&pos, 1, 6, src) != 6) {
        return errorMessage(src, "Failed to read node root pos.\n");
    }
    dest->root.in_pos.x  = ((uint16_t)(pos[0]             ) <<2 | (uint16_t)((pos[1]>>6) & 0b00000011));
    dest->root.in_pos.y  = ((uint16_t)(pos[1] & 0b00111111) <<4 | (uint16_t)((pos[2]>>4) & 0b00001111));
    dest->root.out_pos.x = ((uint16_t)(pos[2] & 0b00001111) <<6 | (uint16_t)((pos[3]>>2) & 0b00111111));
    dest->root.out_pos.y = ((uint16_t)(pos[3] & 0b00000011) <<8 | (uint16_t)( pos[4]                 ));
    dest->root.num_connections = pos[5];
    
    dest->root.connections = malloc(dest->root.num_connections * NODE_SOCKET_SIZE);
    if (dest->root.connections == NULL) {
        return errorMessage(src, "Failed to allocate root connections.\n");
    }
    if (fread(dest->root.connections, NODE_SOCKET_SIZE, dest->root.num_connections, src) != dest->root.num_connections) {
        return errorMessage(src, "Failed to read root connections.\n");
    }

    if (parseNameTable(src, &dest->id) == 1) {
        return 1;
    }
    if (parseNameTable(src, &dest->type) == 1) {
        return 1;
    }

    if (parseInstances(src, &dest->instance) == 1) {
        return 1;
    }

    return 0;
}

//opens nodefile, pass in path and returns node pointer + whether it succeceded
int openNodeFile(char* path, Node* node) {
    FILE* nodeFile = fopen(path, "rb");
    if (nodeFile == NULL) {
        errorMessage(nodeFile, "\nFailed to open node file.");
        return 1;
    }
    if (parseNode(nodeFile, node) == 1) {
        fprintf(stderr, "\nWhoops, an error occured parsing the node."); //since 
        return 1;
    }
    fclose(nodeFile);
    return 0;
}

void printNode(Node* node) {
    printf("Node Version: %d\n", node->version);
    printf("Input Root \n\tX: %u \n\tY: %u\n", node->root.in_pos.x, node->root.in_pos.y); //should be 432 and 508 
    printf("Output Root \n\tX: %u \n\tY: %u\n", node->root.out_pos.x, node->root.out_pos.y); // should be 560 and 494
    printf("Output Root Connections (Amount: %u):\n", node->root.num_connections);
    for (size_t i = 0; i < node->root.num_connections; i++) {
        printf("\tIndex %zu: \n\t\tNode ID: %u \n\t\tSocket ID: %u\n", i, node->root.connections[i].node, node->root.connections[i].socket);
    }
    printf("Node Names (Amount: %u):\n", node->id.table_size);
    for (size_t j = 0; j < node->id.table_size; j++) {
        printf("\t%s\n", node->id.table[j].str);
    }
    printf("Node Types (Amount: %u):\n", node->type.table_size);
    for (size_t j = 0; j < node->type.table_size; j++) {
        printf("\t%s\n", node->type.table[j].str);
    }
    printf("Node Instances (Amount: %u):\n", node->instance.instance_count);
    for (size_t i = 0; i < node->instance.instance_count; i++) {
        printf("\tNode Instance %zu: %s\n", i, node->instance.instance_table[i].name);
        printf("\t\tInstance Key: %u\n\t\tInstance Type: %u\n", node->instance.instance_table[i].key, node->instance.instance_table[i].type);
        printf("\t\tInstance Position: \n\t\t\tX: %u \n\t\t\tY: %u\n", node->instance.instance_table[i].pos.x, node->instance.instance_table[i].pos.y);
    }
}