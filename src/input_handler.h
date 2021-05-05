#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "edi.h"


// TODO: just pass pointer to buffer...
void handle_input(struct edi* e);
void handle_command_input(struct edi* e);

static void handle_cursor_movement(struct edit_buffer* buffer);
static void handle_enter_key(struct edit_buffer* buffer);
static void handle_backspace_key(struct edit_buffer* buffer);

#endif