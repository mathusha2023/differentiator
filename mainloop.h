#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <stddef.h>

enum Commands
{
    CMD_EXIT,
    CMD_DIFF,
    CMD_N_DIFF,
    CMD_TAYLOR,
    CMD_MAKLOREN,
};

const size_t CMD_LIST_LENGTH = 5;
const enum Commands CMD_LIST[CMD_LIST_LENGTH] = {
    CMD_EXIT,
    CMD_DIFF,
    CMD_N_DIFF,
    CMD_TAYLOR,
    CMD_MAKLOREN,
};

int mainloop(const char *data_file_name, const char *tex_file_name);

#endif // MAINLOOP_H
